////////////////////////////////////////////////////////////////////////////
//
// Class: Panel
//
// Static class that displays status panel:
//
//	Energy usage
//	Current speed
//	Distance along tunnel (and to end of current section)
//	Distance from center of tunnel section
//	Elapsed time within current tunnel
//	Score
//
////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbTime.h>

extern class Meter;
extern class SoNode;
extern class SoSwitch;
extern class SoText2;

class Panel {
  public:
    // Initializes class
    static void		init();

    // Cleans up
    static void		cleanUp();

    // Returns scene graph that displays status
    static SoNode *	getGraph()		{ return root; }

    // Resets all displays to initial values
    static void		reset();

    // Sets/returns current level
    static void		setLevel(int level);
    static int		getLevel()		{ return level; }

    // Sets/adds to/returns elapsed time
    static void		setElapsedTime(const SbTime &time);
    static void		addElapsedTime(const SbTime &timeDiff);
    static SbTime 	getElapsedTime();

    // Sets/adds to/returns current score
    static void		setScore(int score);
    static void		addScore(int scoreDiff);
    static int		getScore()		{ return score; }

    // Sets multiplier for scores
    static void		setScoreMult(float mult) { scoreMult = mult; }

    // Sets/returns metered quantities
    static void		setSpeed(float speed);
    static void		setDistance(float distance);
    static void		setCentering(float centering);
    static void		setEnergy(float energy);
    static float	getSpeed();
    static float	getDistance();
    static float	getCentering();
    static float	getEnergy();

    // Begins a crash, ring, or explode effect
    static void		startCrash()		{ startEffect(&crash); }
    static void		startRingApply()	{ startEffect(&ring); }
    static void		startExplode()		{ startEffect(&explode); }

    // Activates/deactivates display of main panel
    static void		display(SbBool flag);

    // Activates/deactivates between-level display
    static void		displayEndLevel(SbBool flag);

    // Activates display of ending graphics
    static void		displayEnd();

    // Turns off all panel display
    static void		displayNothing();

  private:
    // This structure manages the display of effects that last for a
    // certain amount of time: crashing, a ring effect, exploding
    struct Effect {
	SbBool		isDisplayed;	// TRUE if currently displayed
	float		duration;	// Duration of effect in seconds
	float		displayTime;	// Time since display started
	SoSwitch	*displaySwitch;	// Activates display of effect
    };
    static int		level;
    static SbTime	elapsedTime;
    static int		score;
    static float	scoreMult;
    static Effect	crash;
    static Effect	ring;
    static Effect	explode;
    static Meter	*energyMeter;
    static Meter	*distanceMeter;
    static Meter	*speedMeter;
    static Meter	*centerMeter;
    static SoNode	*root;		// Root of scene graph
    static SoText2	*levelText;
    static SoText2	*scoreText;
    static SoText2	*timeText;

    // Builds scene graph
    static void		buildGraph();

    // Begins display of an effect
    static void		startEffect(Effect *effect);

    // Updates display of an effect for given change in time, possibly
    // ending display of effect
    static void		updateEffect(Effect *effect, float numSeconds);
};
