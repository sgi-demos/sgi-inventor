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
#include "Sound.h"

const char *Sound::fileNames[] = {
    "sounds/click2.aiff",
    "sounds/click3.aiff",
    "sounds/click2.aiff",
    "sounds/click3.aiff",
    "sounds/bell.aiff",
};

const char *Sound::names[] = {
    "readyToConnect",
    "connected",
    "readyToDisconnect",
    "disconnected",
    "bell",
};

SbBool	Sound::doAudio = TRUE;		// TRUE if audio is enabled
SbBool	Sound::hasAudio = FALSE;	// TRUE if machine can do audio

////////////////////////////////////////////////////////////////////////////
//
// This structure is used to pass data to sproc'ed audio threads
//
////////////////////////////////////////////////////////////////////////////

struct SoundData {
    Sound::Effect	effect;
    const char *	fileName;
    AFfilehandle	file;
    ALport		port;
    ALconfig		config;
    float		duration; // 0 if play once
    float		pitch; // 1.0 if same as in file.
    long		sampleRate;
    long		sampleWordSize;
    long		bytesPerSample;
    long		samplesPerFrame;
    long		framesPerBuf;
    long		samplesPerBuf;
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

    if (hasAudio)
	signal(SIGCHLD, SIG_IGN);
    
}

////////////////////////////////////////////////////////////////////////////
//
// Plays a sound once.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playOnce(Effect effect)
{
    initiate(effect, 0.0);
}

////////////////////////////////////////////////////////////////////////////
//
// Plays a sound once. Uses given filename instead of the standard
// one for that effect
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playOnce(Effect effect, char *fileName)
{
    initiate(effect, 0.0, fileName);
}

////////////////////////////////////////////////////////////////////////////
//
// Plays a sound once at a given pitch (relative to the one in the file).
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playOnce(Effect effect, char *fileName, float pitch)
{
    initiate(effect, 0.0, fileName, pitch);
}

////////////////////////////////////////////////////////////////////////////
//
// Plays a sound repeatedly for the given duration.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playForDuration(Effect effect, float duration)
{
    initiate(effect, duration);
}

////////////////////////////////////////////////////////////////////////////
//
// Initiates playing of a sound.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::initiate(Effect effect, float duration)
{
    initiate( effect, duration, fileNames[effect], 1.0 );
}

void
Sound::initiate(Effect effect, float duration, const char *filename )
{
    initiate( effect, duration, filename, 1.0 );
}

void
Sound::initiate(Effect effect, float duration, const char *filename, 
		float pitch )
{
    // Make sure machine can handle audio and it's enabled
    if (! hasAudio || ! doAudio)
	return;

    // Do a very quick check to see if there is an audio port
    // available, before going through the process of opening it for real.
    ALconfig config = ALnewconfig();
    ALport   port   = ALopenport("noname", "w", config);

    // If no audio ports, can't play sound
    if (port == NULL)
	return;

    ALcloseport(port);

    // Set up data to play the effect
    SoundData *data = new SoundData;
    data->effect = effect;
    data->duration = duration;
    if (pitch == 0.0) 
	data->pitch = 1.0;
    else
	data->pitch = pitch;
    data->fileName = filename;

    // Use sproc(2) to create an audio thread. The thread begins
    // execution at setUpToPlay. The PR_SALL argument allows the two
    // processes to share all attributes.

    if (sproc(&Sound::setUpToPlay, PR_SALL, data) < 0) {
	fprintf(stderr, "Sound::play: can't sproc thread.\n");
	return;
    }
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
    int fd = open(data->fileName, O_RDONLY);

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

    if (! ok)
	fprintf(stderr, "Sound::play: problem with file \"%s\".\n",
		data->fileName);

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
    data->framesPerBuf   = (data->sampleRate + 15) / 20;
    data->samplesPerBuf = data->framesPerBuf * data->samplesPerFrame;

    // Configure and open audio port
    data->config = ALnewconfig();
    ALsetwidth(data->config, data->sampleWordSize);
    ALsetchannels(data->config, data->samplesPerFrame);

    // Make the ring buffer large enough to hold 1 sec of audio samples
    ALsetqueuesize(data->config, 2 * data->samplesPerBuf);
    data->port = ALopenport(names[data->effect], "w", data->config);

    if (data->port == NULL)
	return FALSE;

    playSamples(data);

    AFclosefile(data->file);
    ALfreeconfig(data->config);
    ALcloseport(data->port);

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
	fprintf(stderr, "Sound::getRate: unknown frame rate in \"%s\".\n",
		data->fileName);
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
	fprintf(stderr, "Sound::setSampleWordSize: can't play %d-bit samples "
		"for \"%s\".\n",
		bitsPerSample, data->fileName);
	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Actually plays the sound.
//
////////////////////////////////////////////////////////////////////////////

void
Sound::playSamples(SoundData *data)
{
    char *sampleBuf = new char [data->samplesPerBuf * data->bytesPerSample];

    long samplesPerOutputBuf = (long) floor((1.0/data->pitch) * data->samplesPerBuf);
    char *outputBuf = new char [samplesPerOutputBuf * data->bytesPerSample];

    long totalFrames = AFgetframecnt(data->file, AF_DEFAULT_TRACK);

    float totalTime = 0.0;
    float secondsPerFrame = 1.0 / (double) data->sampleRate;

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

	    if ( data->pitch == 1.0) {
		// Output the frames
		ALwritesamps(data->port, sampleBuf,
			     framesRead * data->samplesPerFrame);
	    }
	    else {

		for (long outSmp = 0; outSmp < samplesPerOutputBuf; outSmp++ ) {
		    long inSmp = (long) floor(outSmp * data->pitch);
		    long outStart = outSmp * data->bytesPerSample;
		    long inStart = inSmp * data->bytesPerSample;
		    for (int j = 0; j < data->bytesPerSample;j++)
		        outputBuf[outStart+j] = sampleBuf[inStart+j];
		}
		// Output the frames
		ALwritesamps(data->port, outputBuf,
			     (long) floor((1.0/data->pitch) * framesRead * data->samplesPerFrame));
	    }

	    framesWritten += framesRead;

	    // Stop if we have exceeded the duration
	    totalTime += framesRead * secondsPerFrame;
	    if (data->duration > 0.0 && totalTime >= data->duration)
		break;
	}

    } while (totalTime < data->duration);

    delete [] sampleBuf;
    delete [] outputBuf;
}

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <getopt.h>
#include <CC/osfcn.h>
#include <signal.h>

