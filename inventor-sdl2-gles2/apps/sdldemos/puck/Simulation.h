//  -*- C++ -*-

/*
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.16 $
 |
 |   Description:
 |      Defines the simulation state class.
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _PUCK_SIMULATION_
#define _PUCK_SIMULATION_

#include <Inventor/SbTime.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbPList.h>
#include "MovingObject.h"
#include "Rink.h"
#include "msg.h"

typedef void SimulationCollisionCB(void *userData, CollisionState *state);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SimulationState
//
//////////////////////////////////////////////////////////////////////////////

class SimulationState {
  public:
    SimulationState();
    ~SimulationState();
    
    // Add/remove shots and pucks
    void	    addShot(int id, ShotData *s);
    inline ShotData *getShot(int id, int whichShot) const;
    inline int	    getNumShots(int id) const;
    
    void	    setPuck(PuckData *p);
    inline PuckData *getPuck() const	    { return puck; }
    
    // Move pillars up and down in the rink
    void	    setPillarUp(
			int which,
			const SbTime &timeNow, 
			const SbTime &downTime);
    SbBool	    isPillarUp(int which) { return rink.isPillarUp(which); }
	
    // Returns whether the shot will hit the puck, and if so,
    // where and when this will occur.
    static SbBool   willCollide(
			const ShotData &s,
			const PuckData &p,
			SbVec2f &where, 
			SbTime &when);
    
    // Update the state of the simulation for the passed time t.
    // Returns TRUE if something changed.
    SbBool	    simulate(const SbTime &t);
    
    // Update the passed render state to reflect the current simulation state
    void	    updateRenderState(RenderState &rstate, const SbTime &timestamp);
    
    // Reset simulation, so we can start a new game
    void	    reset();
    
    // Set a callback to find out when there are collisions
    void	    setCollisionCallback(
			SimulationCollisionCB *f,
			void *userData = NULL)
		    { collisionCB = f, collisionUserData = userData; }

  protected:
    // Compute collisions for all shots
    void	    computeShotPuckCollisions(const SbTime &t);

    // Compute collisions for the named shot
    void	    computeShotPuckCollision(int id, int shot);

    // Shot hit puck!
    // Change the puck direction and recompute the simulation state.
    void	    shotPuckCollision();
    
    // Puck hit wall!  
    // Change the puck direction and recompute the simulation state.
    void	    puckWallCollision();
    
    // Mark the next collision - shot/puck, puck/wall, or nothing
    void	    markNextCollision();
    
    // Compute a new puck direction
    void	    computeNewPuckDir(
			PuckData *puck, ShotData *shot, SbVec2f &newDir);
			
    // List removal
    inline void	    removeShot(int id, int whichShot);
    inline void	    removeShot(int id, ShotData *shot);

  private:
    // Shots, pucks, and the rink used in the simulation
    SbPList shotData[2];    // list of (ShotData *) for each player
    PuckData *puck;	    // the puck
    Rink    rink;	    // the rink
    
    // Shot/puck collision data
    SbVec2f	shotPuckCollisionLoc;
    SbTime	shotPuckCollisionTime;
    int		shotPuckCollisionId;
    ShotData	*shotPuckCollisionShot;
    
    // Next collision
    CollisionType nextCollision;
    SbTime	  nextCollisionTime;
    
    // Callback to tell when collisions occur
    SimulationCollisionCB   *collisionCB;
    void		    *collisionUserData;
    CollisionState	    *collisionState;
};


// Inline methods
void SimulationState::removeShot(int id, int whichShot)
{ shotData[id].remove(whichShot); }

void SimulationState::removeShot(int id, ShotData *shot)
{ shotData[id].remove(shotData[id].find(shot)); }

ShotData *SimulationState::getShot(int id, int whichShot) const
{ return (ShotData *) shotData[id][whichShot]; }

int SimulationState::getNumShots(int id) const
{ return shotData[id].getLength(); }

#endif
