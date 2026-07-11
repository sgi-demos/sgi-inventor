//
// SoundSDL.c++ — SDL2 audio implementation of the Linkatron Sound class.
//
// The 1994 original (Sound.c++) drove IRIX dmedia from a forked audio
// process. This reproduces the same static interface — playOnce over an
// Effect enum, with optional per-call sound file and pitch (the music
// box plays its bells at different pitches) — on the shared software
// mixer used by the other game ports. Sounds ship as WAV conversions of
// the original AIFFs under data/sounds/.
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
// table, .wav conversions, under the data dir).
static const char *effectFiles[] = {
    "sounds/click2.wav",    // READY_TO_CONNECT
    "sounds/click3.wav",    // CONNECTED
    "sounds/click2.wav",    // READY_TO_DISCONNECT
    "sounds/click3.wav",    // DISCONNECTED
    "sounds/bell.wav",      // BELL
};

#define DEV_RATE    44100
#define DEV_CHANS   2
#define NUM_CHANS   12
#define MAX_SOUNDS  16

struct PCM {
    char        name[128];
    Sint16     *data;           // interleaved stereo
    int         frames;
    int         rate;
};

struct Chan {
    PCM        *pcm;
    double      pos;
    double      step;
    SbBool      playing;
    SbBool      loop;
    long        stopAfter;      // playForDuration limit (-1 = none)
};

static SDL_AudioDeviceID sdlDev = 0;
static PCM  sounds[MAX_SOUNDS];
static int  numSounds = 0;
static Chan chans[NUM_CHANS];

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

// "sounds/bell.aiff" (or a scene-provided name) -> cached PCM loaded
// from the .wav conversion of the same stem.
static PCM *
loadSound(const char *soundName)
{
    const char *base = strrchr(soundName, '/');
    base = base ? base + 1 : soundName;
    char stem[96];
    snprintf(stem, sizeof stem, "%s", base);
    char *dot = strrchr(stem, '.');
    if (dot) *dot = '\0';

    for (int i = 0; i < numSounds; i++)
	if (strcmp(sounds[i].name, stem) == 0)
	    return sounds[i].data ? &sounds[i] : NULL;
    if (numSounds >= MAX_SOUNDS)
	return NULL;

    PCM *p = &sounds[numSounds++];
    snprintf(p->name, sizeof p->name, "%s", stem);
    p->data = NULL;

    char path[160];
    snprintf(path, sizeof path, "sounds/%s.wav", stem);

    SDL_AudioSpec spec;
    Uint8 *buf; Uint32 bytes;
    if (!SDL_LoadWAV(path, &spec, &buf, &bytes)) {
	fprintf(stderr, "Sound: cannot load %s: %s\n", path, SDL_GetError());
	return NULL;
    }
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
    SDL_PauseAudioDevice(sdlDev, 0);
    hasAudio = TRUE;
}

static void
startSound(PCM *p, float pitch, SbBool loop, long stopAfter)
{
    if (!p) return;
    SDL_LockAudioDevice(sdlDev);
    int c = freeChan();
    if (c >= 0) {
	Chan *ch = &chans[c];
	ch->pcm = p;
	ch->pos = 0;
	ch->step = (double) pitch * p->rate / DEV_RATE;
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
    startSound(loadSound(effectFiles[effect]), 1.0f, FALSE, -1);
}

void
Sound::playOnce(Effect effect, char *fileName)
{
    if (!hasAudio || !doAudio) return;
    PCM *p = fileName ? loadSound(fileName) : loadSound(effectFiles[effect]);
    if (!p) p = loadSound(effectFiles[effect]);
    startSound(p, 1.0f, FALSE, -1);
}

void
Sound::playOnce(Effect effect, char *fileName, float pitch)
{
    if (!hasAudio || !doAudio) return;
    PCM *p = fileName ? loadSound(fileName) : loadSound(effectFiles[effect]);
    if (!p) p = loadSound(effectFiles[effect]);
    startSound(p, pitch > 0 ? pitch : 1.0f, FALSE, -1);
}

void
Sound::playForDuration(Effect effect, float duration)
{
    if (!hasAudio || !doAudio) return;
    startSound(loadSound(effectFiles[effect]), 1.0f, TRUE,
	       (long)(duration * DEV_RATE));
}
