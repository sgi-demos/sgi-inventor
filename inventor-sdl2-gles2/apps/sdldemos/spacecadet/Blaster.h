////////////////////////////////////////////////////////////////////////////
//
// Class: Blaster
//
// Fires blasts from ship.
//
////////////////////////////////////////////////////////////////////////////

extern struct Blast;

extern class SoSeparator;
extern class SoNode;
extern class Tunnel;

#include "Motion.h"

class Blaster {
  public:
    // Constructor and destructor
    Blaster();
    ~Blaster();

    // Returns scene graph representing blasts
    SoNode *		getGraph();

    // Sets tunnel to shoot into
    void		setTunnel(Tunnel *tunnel);

    // Sets whether currently firing
    void		setFiring(SbBool flag);

    // Updates all current blasts for given ship motion interval.
    // Fires a new blast if appropriate.
    void		update(Motion *shipMotion);

  private:
    Tunnel		*tunnel;	// Tunnel blasts are fired in
    SbBool		firing;		// Whether currently firing
    SbBool		needToFire;	// If firing enabled before update
    int			numBlasts;	// Number of active blasts
    Blast		*blasts;	// Active blasts
    float		sinceLastBlast;	// Time since last blast fired
    SoNode		*root;		// Root of scene graph
    SoNode		*blastObj;	// Blast display object
    SoSeparator		*blastSep;	// Root separator over blasts
    Motion		motion;		// To send blasts into the tunnel

    // Builds graph representing blasts
    void		buildGraph();

    // Adds a new active blast
    void		addBlast(const SbVec3f &position,
				 const SbVec3f &direction);

    // Removes a blast that is no longer active
    void		removeBlast(int index);
};
