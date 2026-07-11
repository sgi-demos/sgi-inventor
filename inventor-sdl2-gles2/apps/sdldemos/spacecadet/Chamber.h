////////////////////////////////////////////////////////////////////////////
//
// Class: Chamber
//
// Static class that is used to show the between-levels chamber.
//
////////////////////////////////////////////////////////////////////////////

extern class SoAction;
extern class SoNode;
extern class SoSeparator;
extern class SoSwitch;

class Chamber {
  public:
    // Initializes class
    static void		init(int startLevel);

    // Cleans up
    static void		cleanUp();

    // Returns scene graph that displays chamber
    static SoNode *	getGraph()		{ return root; }

    // Returns duration (in seconds) of main chamber animation and
    // door animation
    static float	getMainAnimationTime();
    static float	getDoorAnimationTime();

    // Begins animation that brings ship through chamber to tunnel for
    // given level
    static void		animateMain(int level);

    // Begins animation that opens door to tunnel
    static void		animateDoor();

  private:
    static SoNode	*root;
    static SoSeparator	*doors;		// Root of tunnel doors graph
    static SoSwitch	*doorSwitch;	// Switch over current door

    // Builds scene graph
    static void		buildGraph(int startLevel);

    // Callback to enable or disable GL depth test. The userData
    // pointer is actually the Boolean flag.
    static void	depthCB(void *userData, SoAction *action);
};
