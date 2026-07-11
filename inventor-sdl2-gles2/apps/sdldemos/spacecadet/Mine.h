////////////////////////////////////////////////////////////////////////////
//
// Class: Mine
//
// Exploding mine.
//
////////////////////////////////////////////////////////////////////////////

extern struct Motion;

extern class SoNode;
extern class SoSwitch;

#include <Inventor/SbLinear.h>

class Mine {
  public:
    // Constructor and destructor
    Mine(const SbVec3f &position, float radius, float delay);
    ~Mine();

    // Returns scene graph representing mine
    SoNode *		getGraph()		{ return root; }

    // Updates mine based on ship motion
    void		updateShip(Motion *shipMotion);

    // Updates mine based on blast motion
    void		updateBlast(Motion *blastMotion);

  private:
    // Mine states. The values correspond to the appropriate switch
    // settings to display the mine.
    enum State {
	DORMANT		= 0,	// Not yet active
	ACTIVE		= 1,	// Active, ticking
	EXPLODING1	= 2,	// Beginning explosion
	EXPLODING2	= 3,	// Next stage in explosion
	DEACTIVATING1	= 4,	// Beginning deactivation
	DEACTIVATING2	= 5,	// Next stage in deactivation
	INACTIVE	= 6,	// Deactivated or exploded
    }			state;

    SbVec3f		position;	// Center of mine
    float		radius;		// Radius of mine (scaled from 1)
    float		delay;		// How long until explosion
    SbSphere		mineSphere;	// Sphere used to detect blast hits 
    float		changeTime;	// Time left to show deactivation
					// or explosion change

    static SoNode	*sceneRoot;	// Root of mine scene graph
					// read from file

    SoNode		*root;		// Root of scene graph
    SoSwitch		*mineSwitch;	// Switch node for changing appearance

    // Builds graph representing mine
    void		buildGraph();

    // Returns TRUE if given motion (ship or blast) hits mine
    SbBool		isHitBy(Motion *motion);

    // Activates or deactivates mine
    void		activate();
    void		deactivate();

    // Makes mine explode
    void		explode(Motion *shipMotion);
};
