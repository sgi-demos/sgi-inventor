////////////////////////////////////////////////////////////////////////////
//
// Class: Setup
//
// Static class that displays setup panel:
//
//	Sound on/off switch
//	Airplane mode on/off switch
//	Dark mode on/off switch
//	Starting level
//	Help display button
//	Start button
//
////////////////////////////////////////////////////////////////////////////

extern class SoEventCallback;
extern class SoNode;
extern class SoPickedPoint;

// Type of callback registered with setStartCallback()
typedef void SetupStartCB(void);

class Setup {
  public:
    // Initializes class
    static void		init();

    // Cleans up
    static void		cleanUp();

    // These set initial values for each of the setup options
    static void		setPlayerName(const char *name)	{ playerName = name; }
    static void		setSound(SbBool flag)		{ soundOn = flag; }
    static void		setAirplaneMode(SbBool flag)	{ airplaneMode = flag;}
    static void		setDarkMode(SbBool flag)	{ darkMode = flag;}
    static void		setStartLevel(int level)	{ startLevel =level; }

    // These return the setup options
    static const char *	getPlayerName()			{ return playerName; }
    static SbBool	isSound()			{ return soundOn; }
    static SbBool	isAirplaneMode()		{ return airplaneMode;}
    static SbBool	isDarkMode()			{ return darkMode;}
    static int		getStartLevel()			{ return startLevel; }

    // Registers a callback to call when the "start" button is pressed
    // and the game is to begin or resume
    static void		setStartCallback(SetupStartCB *cb) { startCB = cb; }

    // Sets/returns whether texturing is fast enough on this machine
    // to be enabled
    static void		setTextureEnabled(SbBool flag)	{ texEnabled = flag; }
    static SbBool	isTextureEnabled()		{ return texEnabled; }

    // Returns scene graph that displays setup panel
    static SoNode *	getGraph();

    // Activates display of high score list. If the player hits the
    // "Play Again" button in the list, the setup panel is displayed
    // again, which can call startCB again.
    static void		displayHighScores();

  private:

    // Enum that indicates what was picked on the panel
    enum Hit {
	NONE,			// No hit
	SOUND,			// Sound toggle
	AIRPLANE_MODE,		// Airplane mode toggle
	DARK_MODE,		// Dark mode toggle
	LEVEL_UP_1,		// Increment level by 1
	LEVEL_UP_5,		// Increment level by 5
	LEVEL_DOWN_1,		// Decrement level by 1
	LEVEL_DOWN_5,		// Decrement level by 5
	HELP,			// Display help
	START,			// Begin game
	QUIT,			// Quit game
	RESTART,		// Restart game (from high score panel)
    };

    static const char	*playerName;
    static SbBool	soundOn;
    static SbBool	airplaneMode;
    static SbBool	darkMode;
    static int		startLevel;
    static SetupStartCB	*startCB;
    static SbBool	texEnabled;
    static SoNode	*root;		// Root of scene graph
    static Hit		saveHit;	// What was hit when mouse went down

    // Builds scene graph
    static void		buildGraph();

    // Callback for keyboard button press or release
    static void		keyboardCB(void *userData, SoEventCallback *ecb);

    // Callback for mouse button press or release
    static void		buttonCB(void *userData, SoEventCallback *ecb);

    // Restarts after game has completed
    static void		restart();

    // Finds what was hit, returning code
    static Hit		getHit(const SoPickedPoint *pp);

    // Processes a hit
    static void		processHit(Hit hit);

    // Updates scene graph based on current settings
    static void		updateGraph();

    // Displays help screen
    static void		displayHelp();
};
