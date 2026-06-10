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
	CHANT,		// Crowd - game starts
	GUN_SHOT,	// Gun is fired
	SHOT_PUCK,	// Shot hits puck
	PUCK_WALL,	// Puck hits wall
	PUCK_PILLAR,	// Puck hits pillar
	PUCK_POST,	// Puck hits goalpost
	AHH,		// Crowd - my shot hit goalpost
	CHEER,		// Crowd - my team scored a goal
	BOO,		// Crowd - other team scored a goal
	BELL,		// Bell when puck drops
	VICTORY,	// Victory song for the winner
	LOSER,		// Sound when "you lost" drops to the ground
    };

    // Initializes sound processing. If audio does not exist on this
    // machine, future calls to play sounds have no effect.
    static void		init();

    // Enables or disables sound processing
    static void		enable(SbBool flag)	{ doAudio = flag; }
    static SbBool	isEnabled()		{ return doAudio; }

    // Plays a sound once
    static void		playOnce(Effect effect);

    // Plays a sound repeatedly for the given duration (in seconds)
    static void		playForDuration(Effect effect, float duration);

  private:
    static const char	*fileNames[];	// File names for each effect
    static const char	*names[];	// Names for each effect
    static SbBool	hasAudio;	// TRUE if machine can do audio
    static SbBool	doAudio;	// TRUE if audio is enabled

    // Initiates playing of a sound
    static void		initiate(Effect effect, float duration);

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
