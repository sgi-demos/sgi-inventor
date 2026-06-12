//
// SoundStub.c++ — NO_AUDIO implementation of the slotcar Sound class.
//
// The original Sound.c++ drives IRIX dmedia (audio.h/audiofile.h) from a
// forked sproc; none of that exists off IRIX. This stub satisfies the
// same interface (Sound.h) with silence. Engine/screech/crash audio is a
// candidate for an SDL_mixer reimplementation later.
//
#include <Inventor/SbString.h>
#include "Sound.h"

SbBool Sound::doAudio = FALSE;

void	Sound::init() { doAudio = FALSE; }
void	Sound::playOnce(const char *, float) {}
void	Sound::playForDuration(const char *, float) {}
int	Sound::initContinuous(const char *, float) { return -1; }
void	Sound::setContinuousPitch(int, float) {}
void	Sound::setPlayContinuous(int, SbBool, float) {}
void	Sound::endContinuous(int) {}
