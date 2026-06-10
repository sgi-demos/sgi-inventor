/*
 * Copyright 1994, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to restrictions
 * as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
 * and Computer Software clause at DFARS 252.227-7013, and/or in similar or
 * successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
 * rights reserved under the Copyright Laws of the United States.
 */
#include <audio.h>
#include <audiofile.h>
#include <invent.h>
#include <signal.h>
#include <sys/prctl.h>
#include <Inventor/SbString.h>
#include "Sound.h"

SbBool		Sound::hasAudio = FALSE;	// TRUE if machine can do audio
SbBool		Sound::doAudio = TRUE;		// TRUE if audio is enabled
usptr_t		*Sound::arena;			// Shared memory arena
usema_t		*Sound::semaphore;		// For interprocess locking
int		Sound::numContinuous;		// # of cur continuous sounds
SoundData	**Sound::continuousSounds;	// Continuous sounds

// Maximum number of simultaneous continuous sounds
#define MAX_CONTINUOUS	10

////////////////////////////////////////////////////////////////////////////
//
// This structure is used to pass data to sproc'ed audio threads
//
////////////////////////////////////////////////////////////////////////////

struct SoundData {
    // These indicate what to do:
    SbString		soundName;
    SbString		fileName;
    SbBool		isContinuous;
    int			id;		// Id of sound if continuous
    float		pitch;		// If continuous
    float		prevPitch;	// If continuous
    float		duration;	// 0 if play once (not continuous)
    float		waitTime;	// If not continuous

    // These are computed:
    AFfilehandle	file;
    ALport		port;
    ALconfig		config;
    long		sampleRate;
    long		sampleWordSize;
    long		bytesPerSample;
    long		samplesPerFrame;
    long		framesPerBuf;
};

////////////////////////////////////////////////////////////////////////////
//
// Initializes sound processing. If audio does not exist on this
// machine, future calls to play sounds have no effect.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::init()
{
    inventory_t* inventory;

    setinvent();

    while ((inventory = getinvent()) != 0) {
	if (inventory->inv_class == INV_AUDIO) {
	    hasAudio = TRUE;
	    break;
	}
    }

    endinvent();

    // sproc() sometimes leaves zombie processes lying around. If
    // enough of these accumulate, we can't sproc any more processes.
    // This call prevents that from happening.
    if (hasAudio)
	signal(SIGCHLD, SIG_IGN);

    // Set up a shared memory arena for semaphore - this is used in
    // communicating with the process that plays continuous sounds.
    usconfig(CONF_ARENATYPE, US_SHAREDONLY);
    char *arenaFile = "/tmp/SoundXXXXXX";
    mktemp(arenaFile);
    arena = usinit(arenaFile);
    semaphore = usnewsema(arena, 1);

    numContinuous = 0;
    continuousSounds = new SoundData * [MAX_CONTINUOUS];
}

////////////////////////////////////////////////////////////////////////////
//
// Plays a sound once. If waitTime is non-zero, waits that long (in
// seconds) before playing the sound.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playOnce(const char *soundName, float waitTime)
{
    // Make sure machine can handle audio and it's enabled
    if (! hasAudio || ! doAudio)
	return;

    // Set up data to play the effect
    SoundData *data = new SoundData;
    data->soundName = soundName;
    data->fileName  = "sounds/";
    data->fileName += soundName;
    data->fileName += ".aifc";
    data->isContinuous = FALSE;
    data->duration = 0.0;
    data->waitTime = waitTime;

    if (! initiate(data))
	delete data;
}

////////////////////////////////////////////////////////////////////////////
//
// Plays a sound repeatedly for the given duration.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playForDuration(const char *soundName, float duration)
{
    // Make sure machine can handle audio and it's enabled
    if (! hasAudio || ! doAudio)
	return;

    // Set up data to play the effect
    SoundData *data = new SoundData;
    data->soundName = soundName;
    data->fileName  = "sounds/";
    data->fileName += soundName;
    data->fileName += ".aifc";
    data->isContinuous = FALSE;
    data->duration = duration;
    data->waitTime = 0.0;

    if (! initiate(data))
	delete data;
}

////////////////////////////////////////////////////////////////////////////
//
// Initializes and begins a sound that plays continuously. Returns
// an id that is used to refer to the sound later. A negative id
// indicates an error. The initial relative pitch is passed in (1
// = as is).
//
////////////////////////////////////////////////////////////////////////////

int
Sound::initContinuous(const char *soundName, float pitch)
{
    // Make sure machine can handle audio and it's enabled
    if (! hasAudio || ! doAudio)
	return -1;

    // Make sure there's room for another continuous sound
    if (numContinuous == MAX_CONTINUOUS)
	return -1;

    // Allocate a sound
    SoundData *data = new SoundData;
    data->soundName = soundName;
    data->fileName  = "sounds/";
    data->fileName += soundName;
    data->fileName += ".aifc";
    data->isContinuous = TRUE;
    data->waitTime = 0.0;
    data->id = numContinuous;
    data->pitch = data->prevPitch = pitch;

    // Store in array
    continuousSounds[numContinuous++] = data;

    if (! initiate(data)) {
	delete data;
	return -1;
    }

    return data->id;
}

////////////////////////////////////////////////////////////////////////////
//
// Changes the pitch of a continuous sound.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::setContinuousPitch(int soundId, float newPitch)
{
    // Make sure id is valid
    if (soundId < 0 || continuousSounds[soundId] == NULL)
	return;

    // Don't allow a negative pitch to be sent - this is a signal to
    // stop the sound
    if (newPitch < 0.0)
	return;

    // Make sure the sound-playing process doesn't try to read while
    // we're writing into the pitch variable
    uspsema(semaphore);
    continuousSounds[soundId]->pitch = newPitch;
    usvsema(semaphore);
}

////////////////////////////////////////////////////////////////////////////
//
// Ends use of a continuous sound.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::endContinuous(int soundId)
{
    if (soundId < 0 || continuousSounds[soundId] == NULL)
	return;

    // Make the pitch negative. This is a signal to the other process
    // that the sound should be stopped
    uspsema(semaphore);
    continuousSounds[soundId]->pitch = -1.0;
    usvsema(semaphore);
}

////////////////////////////////////////////////////////////////////////////
//
// Initiates playing of a sound.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Sound::initiate(SoundData *data)
{
    // Do a very quick check to see if there is an audio port
    // available, before going through the process of opening it for real.
    ALconfig config = ALnewconfig();
    ALport   port   = ALopenport("noname", "w", config);

    // If no audio ports, can't play sound
    if (port == NULL) {
	ALfreeconfig(config);
	return FALSE;
    }

    ALfreeconfig(config);
    ALcloseport(port);

    // Use sproc(2) to create an audio thread. The thread begins
    // execution at setUpToPlay. The PR_SADDR argument allows the two
    // processes to share address spaces.
    if (sproc(&Sound::setUpToPlay, PR_SADDR, data) < 0) {
//	fprintf(stderr, "Sound::initiate(): can't sproc thread.\n");
	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets up to play the sound. This is the entry point for the sproc'ed
// audio thread.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::setUpToPlay(void *dataPtr)
{
    SoundData *data = (SoundData *) dataPtr;

    // Open the audio file to read from
    int fd = open(data->fileName.getString(), O_RDONLY);

    // Make sure the file is valid to read from and is an AIFF file
    SbBool ok = (fd >= 0 &&
		 AFidentifyfd(fd) >= 0 &&
		 ((data->file = AFopenfd(fd, "r", AF_NULL_FILESETUP)) !=
		  AF_NULL_FILEHANDLE));

    if (ok) {
	data->samplesPerFrame = AFgetchannels(data->file, AF_DEFAULT_TRACK);

	ok = ((data->samplesPerFrame == 1 || data->samplesPerFrame == 2) &&
	      setRate(data) &&
	      setSampleWordSize(data) &&
	      play(data));
    }

//     if (! ok)
// 	fprintf(stderr, "Sound::play: problem with file \"%s\".\n",
// 		data->fileName.getString());

    // Free up the data
    delete data;

    // Exit from the new process
    exit(0);
}

////////////////////////////////////////////////////////////////////////////
//
// Actually plays the sound.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Sound::play(SoundData *data)
{
    // Make the buffer large enough to hold 1/4 sec of audio frames
    data->framesPerBuf = (data->sampleRate + 15) / 20;

    // Configure and open audio port
    data->config = ALnewconfig();
    ALsetwidth(data->config, data->sampleWordSize);
    ALsetchannels(data->config, data->samplesPerFrame);

    // Make the ring buffer large enough to hold 1 sec of audio samples
    ALsetqueuesize(data->config,
		   2 * data->framesPerBuf * data->samplesPerFrame);
    data->port = ALopenport(data->soundName.getString(), "w", data->config);

    if (data->port == NULL)
	return FALSE;

    // Wait, if we're supposed to
    if (data->waitTime > 0.0)
	sginap((long) (data->waitTime * CLK_TCK));

    // Play the samples
    if (data->isContinuous)
	playContinuousSamples(data);
    else
	playSamples(data);

    AFclosefile(data->file);
    ALcloseport(data->port);
    ALfreeconfig(data->config);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets the sample rate based on the input file.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Sound::setRate(SoundData *data)
{
    switch ((long) AFgetrate(data->file, AF_DEFAULT_TRACK)) {
      case  8000:
	data->sampleRate = AL_RATE_8000;
	break;
      case 11025:
	data->sampleRate = AL_RATE_11025;
	break;
      case 16000:
	data->sampleRate = AL_RATE_16000;
	break;
      case 22050:
	data->sampleRate = AL_RATE_22050;
	break;
      case 32000:
	data->sampleRate = AL_RATE_32000;
	break;
      case 44100:
	data->sampleRate = AL_RATE_44100;
	break;
      case 48000:
	data->sampleRate = AL_RATE_48000;
	break;
      default:
// 	fprintf(stderr, "Sound::getRate: unknown frame rate in \"%s\".\n",
// 		data->fileName.getString());
	return FALSE;
    }

    long pvbuf[2];
    pvbuf[0] = AL_OUTPUT_RATE;
    pvbuf[1] = data->sampleRate;
    ALsetparams(AL_DEFAULT_DEVICE, pvbuf, 2);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets the sample word size.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Sound::setSampleWordSize(SoundData *data)
{
    long sampleType;
    long bitsPerSample;

    AFgetsampfmt(data->file, AF_DEFAULT_TRACK,
		 &sampleType, &bitsPerSample);

    if (bitsPerSample <= 8) {
	data->bytesPerSample = 1;
	data->sampleWordSize = AL_SAMPLE_8;
    }

    else if (bitsPerSample <= 16) {
	data->bytesPerSample = 2;
	data->sampleWordSize = AL_SAMPLE_16;
    }

    else if (bitsPerSample <= 24) {
	data->bytesPerSample = 4;
	data->sampleWordSize = AL_SAMPLE_24;
    }

    else {
// 	fprintf(stderr, "Sound::setSampleWordSize: can't play %d-bit samples "
// 		"for \"%s\".\n",
// 		bitsPerSample, data->fileName.getString());
	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Plays the sound (non-continuous).
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playSamples(SoundData *data)
{
    long totalFrames = AFgetframecnt(data->file, AF_DEFAULT_TRACK);

    float totalTime = 0.0;
    float secondsPerFrame = 1.0 / (double) data->sampleRate;

    // Allocate a buffer to hold samples
    long samplesPerBuf = data->framesPerBuf * data->samplesPerFrame;
    char *sampleBuf = new char [samplesPerBuf * data->bytesPerSample];

    // Repeat this until we have played the entire duration. If we're
    // supposed to play only once, duration is 0, so the outer loop
    // will be executed once.
    do {

	// Reset to the beginning of the sample
	AFseekframe(data->file, AF_DEFAULT_TRACK, 0);

	long framesWritten = 0;

	// Keep going until all frames have been read and output
	while (framesWritten < totalFrames) {

	    // Fill the buffer with frames
	    long framesRead = AFreadframes(data->file, AF_DEFAULT_TRACK, 
					   sampleBuf, data->framesPerBuf);

	    // Output the frames
	    ALwritesamps(data->port, sampleBuf,
			 framesRead * data->samplesPerFrame);

	    framesWritten += framesRead;

	    // Stop if we have exceeded the duration
	    totalTime += framesRead * secondsPerFrame;
	    if (data->duration > 0.0 && totalTime >= data->duration)
		break;
	}

    } while (totalTime < data->duration);

    delete [] sampleBuf;
}

////////////////////////////////////////////////////////////////////////////
//
// Plays the sound continuously. Stops when pitch is negative.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playContinuousSamples(SoundData *data)
{
    long sampleSize = data->samplesPerFrame * data->bytesPerSample;

    // Allocate a buffer to hold samples for entire sound
    long totalFrames = AFgetframecnt(data->file, AF_DEFAULT_TRACK);
    char *sampleBuf = new char [totalFrames * sampleSize];

    // Read sound into buffer
    if (AFreadframes(data->file, AF_DEFAULT_TRACK, sampleBuf, totalFrames) !=
	totalFrames) {
// 	fprintf(stderr, "Can't read sound in Sound::playContinuousSamples\n");
	return;
    }

    // Allocate another sample buffer - this will hold the samples
    // that have been scaled by the current pitch. We will operate on
    // 1/10th of a second at a time.
    long framesPerBuf = data->sampleRate / 10;
    char *pitchBuf    = new char [framesPerBuf * sampleSize];

    // This indicates how far along in the input sample frame buffer
    // we are. It is a floating point number so we can scale correctly.
    float curFrame = 0.0;
    long  frameIndex;

    float pitch = 0.0;

    while (TRUE) {

	// Make sure the main process doesn't try to write while we're
	// reading from the pitch variable. If we can't read it, just
	// use the previous pitch.
	if (uscpsema(semaphore)) {
	    data->prevPitch = pitch;
	    pitch = data->pitch;
	    usvsema(semaphore);

	    // Stop when pitch becomes negative
	    if (pitch < 0.0)
		break;
	}

	// Don't play anything if sound has become disabled
	if (! doAudio) {
	    sginap(10);
	    continue;
	}

	float step	= pitch;
	float deltaStep	= (pitch - data->prevPitch) / framesPerBuf;

	// Copy and scale the samples into the work buffer. Do one
	// bufferful and then check for a pitch change
	for (int i = 0; i < framesPerBuf; i++) {

	    // Round off current sample index
	    frameIndex = (long) (curFrame + 0.5);

	    // If it's off the end, wrap around to start
	    while (frameIndex >= totalFrames) {
		curFrame   -= totalFrames;
		frameIndex -= totalFrames;
	    }

	    // Store the current sample in the buffer
	    for (int j = 0; j < sampleSize; j++)
		pitchBuf[i * sampleSize + j] =
		    sampleBuf[frameIndex * sampleSize + j];

	    curFrame += step;
	    step += deltaStep;
	}

	// Output the buffer
	ALwritesamps(data->port, pitchBuf,
		     framesPerBuf * data->samplesPerFrame);
    }

    delete [] pitchBuf;
}
