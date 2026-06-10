////////////////////////////////////////////////////////////////////////////
//
// Classes: Ring, EnergizerRing, StealthRing, WarpRing
//
// Ring that applies some effect when the ship flies through it. Each
// subclass has a different effect. Rings are activated when a blast
// hits the activator button and are applied when the ship flies
// through them.
//
////////////////////////////////////////////////////////////////////////////

extern struct Motion;

extern class SoNode;
extern class SoSwitch;

#include <Inventor/SbLinear.h>

class Ring {
  public:
    // Destructor
    virtual ~Ring();

    // Returns scene graph representing ring
    SoNode *		getGraph()		{ return root; }

    // Updates ring based on ship motion
    void		updateShip(Motion *shipMotion);

    // Updates ring based on blast motion
    void		updateBlast(Motion *blastMotion);

  protected:
    // Constructor and destructor. The constructor is passed the
    // center, orientation, and radius of the ring.
    Ring(const SbVec3f &center, const SbRotation &orientation, 
	 float radius);

    // Builds graph representing ring
    void		buildGraph();

  private:
    SbVec3f		center;		// Center of ring
    SbRotation		orientation;	// Orientation of ring wrt -z axis
    SbPlane		plane;		// Plane containing ring
    float		radius;		// Radius of ring (scaled from 1)
    SbVec3f		buttonOffset;	// Offset of activ. button wrt center
    SbVec3f		buttonCenter;	// Sphere used to detect button hits 
    SbBool		isActive;	// TRUE if ring was activated
    SbBool		wasApplied;	// TRUE after applied once

    static SoNode	*sceneRoot;	// Root of ring scene graph
					// read from file
    SoNode		*root;		// Root of scene graph
    SoSwitch		*ringSwitch;	// Switch node for changing appearance

    // Returns color of ring
    virtual SbColor		getColor() const = 0;

    // Returns string that is label for ring
    virtual const char *	getLabel() const = 0;

    // Activates or deactivates ring
    void			activate(SbBool on);

    // Applies whatever affect the ring has to a ship
    virtual void		apply(Motion *shipMotion) = 0;

    // Computes intersection of motion with plane of ring. Returns
    // FALSE if the motion does not intersect within its interval.
    // Otherwise, fills in intersection and returns TRUE.
    SbBool			intersect(Motion *motion,
					  SbVec3f &intersection);
};

class EnergizerRing : public Ring {
  public:
    EnergizerRing(const SbVec3f &center, const SbRotation &orientation,
		  float radius, float energy);

  private:
    float			energy;
    virtual SbColor		getColor() const;
    virtual const char *	getLabel() const;
    virtual void		apply(Motion *shipMotion);
};

class StealthRing : public Ring {
  public:
    StealthRing(const SbVec3f &center, const SbRotation &orientation,
		float radius, float stealthTime);

  private:
    float			stealthTime;
    virtual SbColor		getColor() const;
    virtual const char *	getLabel() const;
    virtual void		apply(Motion *shipMotion);
};

class WarpRing : public Ring {
  public:
    WarpRing(const SbVec3f &center, const SbRotation &orientation,
	     float radius, float warpDistance);

  private:
    float			warpDistance;
    virtual SbColor		getColor() const;
    virtual const char *	getLabel() const;
    virtual void		apply(Motion *shipMotion);
};
