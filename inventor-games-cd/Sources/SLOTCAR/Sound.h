////////////////////////////////////////////////////////////////////////////
//
// Class: Sound
//
// Static class that plays sound effects. A lot of this was stolen
// from Kevin Goldsmith's PlayClass class and Howard Look's Noise
// class.
//
////////////////////////////////////////////////////////////////////////////

#include <ulocks.h>
#include <Inventor/SbBasic.h>

extern struct SoundData;

class Sound {
  public:
    // Initializes sound processing. If audio does not exist on this
    // machine, future calls to play sounds have no effect.
    static void		init();

    // Enables or disables sound processing
    static void		enable(SbBool flag)	{ doAudio = flag; }
    static SbBool	isEnabled()		{ return doAudio; }

    // Plays a sound once. If waitTime is non-zero, waits that long
    // (in seconds) before playing the sound.
    static void		playOnce(const char *soundName, float waitTime = 0.0);

    // Plays a sound repeatedly for the given duration (in seconds)
    static void		playForDuration(const char *soundName, float duration);

    // Initializes and begins a sound that plays continuously. Returns
    // an id that is used to refer to the sound later. A negative id
    // indicates an error. The initial relative pitch is passed in (1
    // = as is).
    static int		initContinuous(const char *soundName, float pitch);

    // Changes the pitch of a continuous sound
    static void		setContinuousPitch(int soundId, float newPitch);

    // Sets whether to play or pause a continuous sound
    static void		setPlayContinuous(int soundId, SbBool playFlag,
                                float newPitch = 1.0);

    // Ends use of a continuous sound
    static void		endContinuous(int soundId);

  private:
    static SbBool	hasAudio;	// TRUE if machine can do audio
    static SbBool	doAudio;	// TRUE if audio is enabled
    static usptr_t	*arena;		// Shared memory arena
    static usema_t	*semaphore;	// Semaphore for interprocess locking
    static int		numContinuous;	// Number of current continuous sounds
    static SoundData	**continuousSounds;	// Continuous sounds

    // Initiates playing of a sound
    static SbBool	initiate(SoundData *data);

    // Sets up to play the sound. This is the entry point for the
    // sproc'ed audio thread
    static void		setUpToPlay(void *data);

    // Actually plays the sound
    static SbBool	play(SoundData *data);

    // Sets the sample rate based on the input file
    static SbBool	setRate(SoundData *data);

    // Sets the sample word size
    static SbBool	setSampleWordSize(SoundData *data);

    // Plays the samples for the sound. Non-continuous and continuous versions.
    static void		playSamples(SoundData *data);
    static void		playContinuousSamples(SoundData *data);

    // Exit handler for sproc processes
    static void 	killHandler(int);

};
