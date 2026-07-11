//
// SoundSDL.c++ — SDL2 audio implementation of the slotcar Sound class.
//
// The 1994 original (Sound.c++) drove IRIX dmedia from a forked sproc,
// playing AIFF files with live pitch control on the engine loop. This
// implementation reproduces that on a plain SDL audio device with a
// small software mixer: each channel plays a cached PCM buffer at a
// variable rate (linear interpolation), which is exactly the pitch
// behavior the game needs (engine pitch follows speed: v*0.5 + 1.5).
//
// Sounds ship as WAV conversions of the original AIFFs (SDL_LoadWAV
// has no AIFF parser); the .aiff originals remain in data/ for
// preservation. Name mapping: "./motorLoop.aiff" -> "motorLoop.wav".
//
// Works identically native and under Emscripten (WebAudio via SDL;
// browsers resume the audio context on the first user gesture, which
// the Start-button click provides).
//
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Inventor/SbString.h>
#include "Sound.h"

SbBool Sound::doAudio = FALSE;

#define DEV_RATE    44100
#define DEV_CHANS   2
#define NUM_CHANS   16          // mixer channels (4 cars x 2 + one-shots)
#define MAX_SOUNDS  8           // distinct loaded files

struct PCM {
    char        name[64];
    Sint16     *data;           // interleaved stereo
    int         frames;
    int         rate;
};

struct Chan {
    PCM        *pcm;
    double      pos;            // source frame position
    double      step;           // source frames per output frame
    SbBool      playing;
    SbBool      loop;
    SbBool      persistent;     // continuous channel (keeps its slot)
    long        delayFrames;    // playOnce waitTime
    long        stopAfter;      // playForDuration limit (-1 = none)
};

static SDL_AudioDeviceID sdlDev = 0;
static PCM  sounds[MAX_SOUNDS];
static int  numSounds = 0;
static Chan chans[NUM_CHANS];

// Mix callback: accumulate every playing channel with linear-interp
// variable-rate playback.
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
	    if (ch->delayFrames > 0) { ch->delayFrames--; continue; }
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

// "./motorLoop.aiff" -> cached PCM (loaded from motorLoop.wav).
static PCM *
loadSound(const char *soundName)
{
    // Normalize the name: strip path, strip extension.
    const char *base = strrchr(soundName, '/');
    base = base ? base + 1 : soundName;
    char stem[64];
    snprintf(stem, sizeof stem, "%s", base);
    char *dot = strrchr(stem, '.');
    if (dot) *dot = '\0';

    for (int i = 0; i < numSounds; i++)
	if (strcmp(sounds[i].name, stem) == 0)
	    return &sounds[i];
    if (numSounds >= MAX_SOUNDS)
	return NULL;

    char path[128];
    snprintf(path, sizeof path, "%s.wav", stem);

    SDL_AudioSpec spec;
    Uint8 *buf; Uint32 bytes;
    if (!SDL_LoadWAV(path, &spec, &buf, &bytes)) {
	fprintf(stderr, "Sound: cannot load %s: %s\n", path, SDL_GetError());
	return NULL;
    }
    // Convert to S16 stereo at the file's own rate (rate conversion
    // happens in the mixer via the channel step).
    SDL_AudioCVT cvt;
    SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
		      AUDIO_S16SYS, DEV_CHANS, spec.freq);
    PCM *p = &sounds[numSounds];
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
    snprintf(p->name, sizeof p->name, "%s", stem);
    p->rate = spec.freq;
    numSounds++;
    return p;
}

static int
freeChan(SbBool persistent)
{
    for (int i = 0; i < NUM_CHANS; i++)
	if (!chans[i].persistent && !chans[i].playing)
	    { chans[i].persistent = persistent; return i; }
    return -1;
}

static double
stepFor(PCM *p, float pitch)
{
    return (double) pitch * p->rate / DEV_RATE;
}

void
Sound::init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
	fprintf(stderr, "Sound: no audio: %s\n", SDL_GetError());
	doAudio = FALSE;
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
	doAudio = FALSE;
	return;
    }
    memset(chans, 0, sizeof chans);
    SDL_PauseAudioDevice(sdlDev, 0);
    doAudio = TRUE;
}

void
Sound::playOnce(const char *soundName, float waitTime)
{
    if (!doAudio) return;
    PCM *p = loadSound(soundName);
    if (!p) return;
    SDL_LockAudioDevice(sdlDev);
    int c = freeChan(FALSE);
    if (c >= 0) {
	Chan *ch = &chans[c];
	ch->pcm = p; ch->pos = 0; ch->step = stepFor(p, 1.0f);
	ch->loop = FALSE; ch->stopAfter = -1;
	ch->delayFrames = (long)(waitTime * DEV_RATE);
	ch->playing = TRUE;
    }
    SDL_UnlockAudioDevice(sdlDev);
}

void
Sound::playForDuration(const char *soundName, float duration)
{
    if (!doAudio) return;
    PCM *p = loadSound(soundName);
    if (!p) return;
    SDL_LockAudioDevice(sdlDev);
    int c = freeChan(FALSE);
    if (c >= 0) {
	Chan *ch = &chans[c];
	ch->pcm = p; ch->pos = 0; ch->step = stepFor(p, 1.0f);
	ch->loop = TRUE; ch->delayFrames = 0;
	ch->stopAfter = (long)(duration * DEV_RATE);
	ch->playing = TRUE;
    }
    SDL_UnlockAudioDevice(sdlDev);
}

int
Sound::initContinuous(const char *soundName, float pitch)
{
    if (!doAudio) return -1;
    PCM *p = loadSound(soundName);
    if (!p) return -1;
    SDL_LockAudioDevice(sdlDev);
    int c = freeChan(TRUE);
    if (c >= 0) {
	Chan *ch = &chans[c];
	ch->pcm = p; ch->pos = 0; ch->step = stepFor(p, pitch);
	ch->loop = TRUE; ch->delayFrames = 0; ch->stopAfter = -1;
	ch->playing = FALSE;	// game toggles via setPlayContinuous
    }
    SDL_UnlockAudioDevice(sdlDev);
    return c;
}

void
Sound::setContinuousPitch(int soundId, float newPitch)
{
    if (!doAudio || soundId < 0 || soundId >= NUM_CHANS) return;
    SDL_LockAudioDevice(sdlDev);
    if (chans[soundId].pcm)
	chans[soundId].step = stepFor(chans[soundId].pcm, newPitch);
    SDL_UnlockAudioDevice(sdlDev);
}

void
Sound::setPlayContinuous(int soundId, SbBool playFlag, float newPitch)
{
    if (!doAudio || soundId < 0 || soundId >= NUM_CHANS) return;
    SDL_LockAudioDevice(sdlDev);
    Chan *ch = &chans[soundId];
    if (ch->pcm) {
	if (newPitch != 1.0f)
	    ch->step = stepFor(ch->pcm, newPitch);
	ch->playing = playFlag;
    }
    SDL_UnlockAudioDevice(sdlDev);
}

void
Sound::endContinuous(int soundId)
{
    if (!doAudio || soundId < 0 || soundId >= NUM_CHANS) return;
    SDL_LockAudioDevice(sdlDev);
    chans[soundId].playing = FALSE;
    chans[soundId].persistent = FALSE;
    chans[soundId].pcm = NULL;
    SDL_UnlockAudioDevice(sdlDev);
}
