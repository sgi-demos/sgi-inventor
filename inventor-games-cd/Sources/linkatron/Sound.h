////////////////////////////////////////////////////////////////////////////
//
// Class: Sound
//
// Static class that plays sound effects. 
//
////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbBasic.h>

extern struct SoundData;

class Sound {
  public:
    enum Effect {
	READY_TO_CONNECT,	// These 4 occur when Axle is being dragged.
	CONNECTED,		// 
	READY_TO_DISCONNECT,	// 
	DISCONNECTED,		// 
	BELL,		        // Played when a bell rings.
    };

    // Initializes sound processing. If audio does not exist on this
    // machine, future calls to play sounds have no effect.
    static void		init();

    // Enables or disables sound processing
    static void		enable(SbBool flag)	{ doAudio = flag; }
    static SbBool	isEnabled()		{ return doAudio; }

    // Plays a sound once
    static void		playOnce(Effect effect);
    static void		playOnce(Effect effect, char *fileName);
    static void		playOnce(Effect effect, char *fileName, float pitch);

    // Plays a sound repeatedly for the given duration (in seconds)
    static void		playForDuration(Effect effect, float duration);

  private:
    static const char	*fileNames[];	// File names for each effect
    static const char	*names[];	// Names for each effect
    static SbBool	hasAudio;	// TRUE if machine can do audio
    static SbBool	doAudio;	// TRUE if audio is enabled

    // Initiates playing of a sound
    static void		initiate(Effect effect, float duration);
    static void		initiate(Effect effect, float duration, const char *filename);
    static void		initiate(Effect effect, float duration, const char *filename, float pitch);

    // Sets up to play the sound. This is the entry point for the
    // sproc'ed audio thread
    static void		setUpToPlay(void *data);

    // Actually plays the sound
    static SbBool	play(SoundData *data);

    // Sets the sample rate based on the input file
    static SbBool	setRate(SoundData *data);

    // Sets the sample word size
    static SbBool	setSampleWordSize(SoundData *data);

    // Plays the samples for the sound
    static void		playSamples(SoundData *data);
};
