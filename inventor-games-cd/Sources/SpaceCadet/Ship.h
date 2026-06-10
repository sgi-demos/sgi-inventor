////////////////////////////////////////////////////////////////////////////
//
// Class: Ship
//
// Ship that flies through a tunnel.
//
////////////////////////////////////////////////////////////////////////////

#include "Motion.h"

extern class Blaster;
extern class SoDirectionalLight;
extern class SoNode;
extern class SoPerspectiveCamera;
extern class Tunnel;

class Ship {
  public:
    enum State {
	ACCELERATE,
	DECELERATE,
	COAST,
    };

    // Initializes class
    static void		init();

    // Cleans up
    static void		cleanUp();

    // Returns scene graph that represents camera, lights, and other
    // stuff attached to ship
    static SoNode *	getGraph()			{ return root; }

    // Returns ships' blaster
    static Blaster	*getBlaster()			{ return blaster; }

    // Sets airplane mode - if TRUE, moving the mouse down tilts the
    // ship up
    static void		setAirplaneMode(SbBool flag)	{ airplaneMode = flag;}

    // Sets dark mode - if TRUE, lights are out in tunnels
    static void		setDarkMode(SbBool flag);

    // Sets base cursor position to given one
    static void		setBaseCursor(const SbVec2s &pos);

    // Sets current tunnel to ship through. Resets to start of tunnel.
    static void		setTunnel(Tunnel *tunnel);

    // Changes/returns state of motion
    static void		setState(State newState)	{ state = newState; }
    static State	getState()			{ return state; }

    // Sets whether ship's blaster is currently firing
    static void		setFiring(SbBool flag);

    // Returns maximum speed
    static float	getMaxSpeed()			{ return MAX_SPEED; }

    // Sets new cursor position to use to update ship location and direction
    static void		setCursor(const SbVec2s &newCursor);

    // Starts/stops the ship (mostly the sound)
    static void		start();
    static void		stop();

    // Moves over given time interval (in seconds). Returns TRUE if
    // ship has exited tunnel.
    static SbBool	update(float numSeconds);

  private:
    static Tunnel	*tunnel;	// Tunnel shiping through
    static Blaster	*blaster;	// Blaster to shoot things
    static State	state;		// Current motion state
    static SbBool	airplaneMode;	// Whether to use airplane mode
    static SbBool	darkMode;	// Whether to use dark mode
    static float	speed;		// Current speed
    static int		shipSound;	// Id of continuous sound
    static SbVec3f	position;	// Current 3D position
    static SbRotation	orientation;	// Orientation wrt default orientation
    static SbVec2s	initCursor;	// Initial cursor position
    static SbVec2s	curCursor;	// Current cursor position
    static SbVec3f	headLightDir;	// Initial direction of headlight
    static Motion	motion;		// Motion parameters

    static SoNode		*root;		// Root of graph
    static SoPerspectiveCamera	*camera;	// Camera node
    static SoDirectionalLight	*headLight;	// Headlight node

    // These keep track of crashing and allows us to display the crash
    // indicator for a particular length of time
    static SbBool	crashDisplayed;		// TRUE if displayed
    static float	crashDisplayTime;	// Time since displayed

    // Builds scene graph
    static void		buildGraph();

    // Sets camera and lights based on current position and orientation
    static void		setView();
};
