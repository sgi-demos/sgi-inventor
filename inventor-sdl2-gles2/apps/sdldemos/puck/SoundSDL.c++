//
// SoundSDL.c++ — SDL2 audio implementation of the Puck Sound class.
//
// The 1994 original (Sound.c++) drove IRIX dmedia (AL) from a forked
// audio process. This implementation reproduces the same static Sound
// interface (playOnce / playForDuration over an Effect enum) on a plain
// SDL audio device with a small software mixer — the same mixer used by
// the slotcar port. Sounds ship as WAV conversions of the original
// AIFF/AIFC files under data/sounds/ (SDL_LoadWAV has no AIFF parser).
//
// Note: the CD's SOUNDS directory does not include gunShot.aiff even
// though the original fileNames table lists it; a missing file simply
// silences that effect (one stderr note, then quiet).
//
// Works identically native and under Emscripten (WebAudio via SDL;
// browsers resume the audio context on the first user gesture).
//
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Inventor/SbBasic.h>
#include "Sound.h"

SbBool Sound::doAudio  = FALSE;
SbBool Sound::hasAudio = FALSE;

// Effect -> file, same order as the Effect enum (original fileNames
// table, .wav conversions). Local: Sound.h declares fileNames private
// and this implementation does not define the original's other private
// statics.
static const char *effectFiles[] = {
    "data/sounds/chant.wav",
    "data/sounds/gunShot.wav",
    "data/sounds/shotPuck.wav",
    "data/sounds/puckWall.wav",
    "data/sounds/puckPillar.wav",
    "data/sounds/puckPost.wav",
    "data/sounds/ahh.wav",
    "data/sounds/cheer.wav",
    "data/sounds/boo.wav",
    "data/sounds/bell.wav",
    "data/sounds/victory.wav",
    "data/sounds/youLost.wav",
};

#define DEV_RATE    44100
#define DEV_CHANS   2
#define NUM_CHANS   12          // simultaneous one-shots
#define NUM_EFFECTS 12

struct PCM {
    Sint16     *data;           // interleaved stereo
    int         frames;
    int         rate;
    SbBool      loaded, failed;
};

struct Chan {
    PCM        *pcm;
    double      pos;            // source frame position
    double      step;           // source frames per output frame
    SbBool      playing;
    SbBool      loop;
    long        stopAfter;      // playForDuration limit (-1 = none)
};

static SDL_AudioDeviceID sdlDev = 0;
static PCM  sounds[NUM_EFFECTS];
static Chan chans[NUM_CHANS];

// Mix callback: accumulate every playing channel with linear-interp
// rate conversion.
static void
mixCB(void *, Uint8 *stream, int len)
{
    Sint16 *out = (Sint16 *) stream;
    int outFrames = len / (sizeof(Sint16) * DEV_CHANS);
    memset(stream, 0, len);

    for (int c = 0; c < NUM_CHANS; c++) {
	Chan *ch = &chans[c];
	if (!ch->playing || !ch->pcm)
	    continue;
	PCM *p = ch->pcm;
	for (int f = 0; f < outFrames; f++) {
	    if (ch->stopAfter == 0) { ch->playing = FALSE; break; }
	    if (ch->stopAfter > 0) ch->stopAfter--;

	    int i0 = (int) ch->pos;
	    double frac = ch->pos - i0;
	    int i1 = i0 + 1;
	    if (i1 >= p->frames) i1 = ch->loop ? 0 : i0;
	    for (int k = 0; k < DEV_CHANS; k++) {
		Sint32 s0 = p->data[i0 * 2 + k];
		Sint32 s1 = p->data[i1 * 2 + k];
		Sint32 s = (Sint32)(s0 + (s1 - s0) * frac);
		Sint32 acc = out[f * DEV_CHANS + k] + s;
		if (acc >  32767) acc =  32767;
		if (acc < -32768) acc = -32768;
		out[f * DEV_CHANS + k] = (Sint16) acc;
	    }
	    ch->pos += ch->step;
	    if (ch->pos >= p->frames) {
		if (ch->loop) ch->pos -= p->frames;
		else { ch->playing = FALSE; break; }
	    }
	}
    }
}

// Load (once) the PCM for an effect; NULL if the file is unavailable.
static PCM *
loadEffect(int effect)
{
    if (effect < 0 || effect >= NUM_EFFECTS)
	return NULL;
    PCM *p = &sounds[effect];
    if (p->loaded) return p;
    if (p->failed) return NULL;

    SDL_AudioSpec spec;
    Uint8 *buf; Uint32 bytes;
    if (!SDL_LoadWAV(effectFiles[effect], &spec, &buf, &bytes)) {
	fprintf(stderr, "Sound: cannot load %s: %s\n",
		effectFiles[effect], SDL_GetError());
	p->failed = TRUE;
	return NULL;
    }
    // Convert to S16 stereo at the file's own rate (rate conversion
    // happens in the mixer via the channel step).
    SDL_AudioCVT cvt;
    SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
		      AUDIO_S16SYS, DEV_CHANS, spec.freq);
    if (cvt.needed) {
	cvt.len = bytes;
	cvt.buf = (Uint8 *) malloc(bytes * cvt.len_mult);
	memcpy(cvt.buf, buf, bytes);
	SDL_ConvertAudio(&cvt);
	SDL_FreeWAV(buf);
	p->data = (Sint16 *) cvt.buf;
	p->frames = cvt.len_cvt / (sizeof(Sint16) * DEV_CHANS);
    } else {
	p->data = (Sint16 *) buf;
	p->frames = bytes / (sizeof(Sint16) * DEV_CHANS);
    }
    p->rate = spec.freq;
    p->loaded = TRUE;
    return p;
}

static int
freeChan()
{
    for (int i = 0; i < NUM_CHANS; i++)
	if (!chans[i].playing)
	    return i;
    return -1;
}

void
Sound::init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
	fprintf(stderr, "Sound: no audio: %s\n", SDL_GetError());
	hasAudio = FALSE;
	return;
    }
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = DEV_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = DEV_CHANS;
    want.samples = 1024;
    want.callback = mixCB;
    sdlDev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (!sdlDev) {
	fprintf(stderr, "Sound: open failed: %s\n", SDL_GetError());
	hasAudio = FALSE;
	return;
    }
    memset(chans, 0, sizeof chans);
    memset(sounds, 0, sizeof sounds);
    SDL_PauseAudioDevice(sdlDev, 0);
    hasAudio = TRUE;
}

static void
startEffect(int effect, SbBool loop, long stopAfter)
{
    PCM *p = loadEffect(effect);
    if (!p) return;
    SDL_LockAudioDevice(sdlDev);
    int c = freeChan();
    if (c >= 0) {
	Chan *ch = &chans[c];
	ch->pcm = p;
	ch->pos = 0;
	ch->step = (double) p->rate / DEV_RATE;
	ch->loop = loop;
	ch->stopAfter = stopAfter;
	ch->playing = TRUE;
    }
    SDL_UnlockAudioDevice(sdlDev);
}

void
Sound::playOnce(Effect effect)
{
    if (!hasAudio || !doAudio) return;
    startEffect(effect, FALSE, -1);
}

void
Sound::playForDuration(Effect effect, float duration)
{
    if (!hasAudio || !doAudio) return;
    startEffect(effect, TRUE, (long)(duration * DEV_RATE));
}
