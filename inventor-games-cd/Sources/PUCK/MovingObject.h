//  -*- C++ -*-

/*
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.4 $
 |
 |   Description:
 |      Defines the moving object classes for shots and pucks.
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _PUCK_MOVING_OBJECT_
#define _PUCK_MOVING_OBJECT_

#include <Inventor/SbTime.h>
#include <Inventor/SbLinear.h>
#include "msg.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Collision
//
//////////////////////////////////////////////////////////////////////////////

typedef struct {
    SbVec2f where;
    SbTime  when;
    SbVec2f normal;
} Collision;


//////////////////////////////////////////////////////////////////////////////
//
//  Class: MovingObjectData
//
//////////////////////////////////////////////////////////////////////////////

class MovingObjectData {
  public:
    // Initial location, direction, and time of moving object
    MovingObjectData(const SbVec2f &l, const SbVec2f &d, const SbTime &t);
    
    // Loc and Dir of the moving object at Time-Zero
    const SbVec2f & loc0() const	{ return _loc0; }
    const SbVec2f & dir0() const	{ return _dir0; }
    const SbTime &  time0() const	{ return _time0; }

    // Set a new time zero - this will recompute constants
    void	    setNewTimeZero(const SbTime &t0)
			{ update(t0); setTimeZeroData(loc(), dir0(), t0); }
    
    // Update the location of the moving object to reflect the passed time
    void	    update(const SbTime &t);

    // Current location of the moving object
    const SbVec2f & loc() const		{ return _loc; }
    
    float	    dirDotDir() const	    { return _dirDotDir; }
    float	    locDotLoc() const	    { return _locDotLoc; }
    float	    locDotDirX2() const	    { return _locDotDirX2; }

    // Next intersection with rink. Moving object just stores this info.
    void	    setRinkCollision(
			const SbVec2f &where,
			const SbTime &when, 
			const SbVec2f &normal);
    
    // This is the time at which the object will hit the rink    
    const SbTime &  hitTime() const	{ return collision.when; }

    // This is where it will hit
    const SbVec2f & hitLoc() const	{ return collision.where; }
    
    // This is the normal of the rink where the object hits
    const SbVec2f & hitNormal() const	{ return collision.normal; }
    
    
  protected:  
    // Loc and Dir of the moving object at Time-Zero
    void	    setTimeZeroData(
			const SbVec2f &loc,
			const SbVec2f &dir,
			const SbTime &t0);

  private:  
    SbVec2f _loc0;
    SbVec2f _dir0;
    SbTime  _time0;
    
    SbVec2f _loc;	    // current location
    
    // Precomputed constants for use in intersection equations
    float   _dirDotDir;	    // (dir.x * dir.x) + (dir.y * dir.y)
    float   _locDotLoc;	    // (loc.x * loc.x) + (loc.y * loc.y)
    float   _locDotDirX2;   // 2 * ((loc.x * dir.x) + (loc.y * dir.y))

    Collision	collision;
};


//////////////////////////////////////////////////////////////////////////////
//
//  Class: ShotData
//
//////////////////////////////////////////////////////////////////////////////

class ShotData : public MovingObjectData {
  public:
    ShotData(const SbVec2f &l, const SbVec2f &d, const SbTime &t);
    
    void    setForce(float f)	{ _force = f; }
    float   force() const	{ return _force; }
    
  private:
    float	    _force;
};


//////////////////////////////////////////////////////////////////////////////
//
//  Class: PuckData
//
//////////////////////////////////////////////////////////////////////////////

class PuckData : public MovingObjectData {
  public:
    PuckData(const SbVec2f &l, const SbVec2f &d, const SbTime &t);
    
    void    setMass(float m)	    { _mass = m; }
    float   mass() const	    { return _mass; }
    
    void    setRadius(int r);
    int	    radius() const	    { return _radius; }
    int	    radiusXradius() const   { return _radiusXradius; }
    
    void    setNewDirection(const SbVec2f &newDir, const SbTime &t0)
		{ setTimeZeroData(loc(), newDir, t0); }

    void	    setCollisionType(CollisionType t)	{ collisionType = t; }
    CollisionType   getCollisionType() const		{ return collisionType; }
        
  private:
    float	_mass;
    int		_radius;
    int	    	_radiusXradius;	    // radius * radius
    CollisionType collisionType;    // puck hit wall? hit by shot? score goal?
};

#endif
