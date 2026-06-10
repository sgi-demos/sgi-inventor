/*
 * Copyright 1994, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to restrictions
 * as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
 * and Computer Software clause at DFARS 252.227-7013, and/or in similar or
 * successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
 * rights reserved under the Copyright Laws of the United States.
 */
/*
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.28 $
 |
 |   Description:
 |      Simulation code.
 | 
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>
#include <limits.h>
#include "Simulation.h"

//////////////////////////////////////////////////////////////////////////////
//
// This code computes the intersection point and time of collision
// between a moving point (a shot) and a moving circle (a puck).
// We have to solve 3 equations with 3 unknowns:
//
// Unknowns: x,y,t
//	x,y - location of point
//	t   - time of intersection
//
// Equations:
//
//	(Moving point)
//	    x = shot.x + dirShot.x * t
//	    y = shot.y + dirShot.y * t
//
//	(Moving circle)
//	    [x - (puck.x + dirPuck.x * t)]^2 +
//	    [y - (puck.y + dirPuck.y * t)]^2 + r^2 = 0
//
// Multiplying out the moving-circle equation, then substituting in the
// moving point equations gives us one equation with one unknown, t.
// Solve this quadratic using (-b +- sqrt(b^2 - 4ac)) / 2a
// to find values for t. Then substitute the t value(s) into the 
// moving point equations to find x and y.
//
// This is implemented as SimulationState::willCollide().
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
SimulationState::SimulationState()
//
//////////////////////////////////////////////////////////////////////////////
{
    reset();
    collisionCB = NULL;
    collisionState = new CollisionState;
};

//////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
SimulationState::~SimulationState()
//
//////////////////////////////////////////////////////////////////////////////
{
    delete collisionState;
};

//////////////////////////////////////////////////////////////////////////////
//
// Puck hit wall!
// Change the puck direction and recompute the simulation state.
//
void
SimulationState::puckWallCollision()
//
//////////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    printf("puck hit the wall!\n");
#endif

    // Update the puck for the collision time
    puck->update(nextCollisionTime);

    // Compute new direction of puck - 
    // P = puckdir.dot.rinknormal (D.dot.N)
    // R = reflection = puckdir + 2*P

    SbVec2f D(puck->dir0());
    SbVec2f P = puck->hitNormal() * -(D.dot(puck->hitNormal()));
    SbVec2f R = D + 2 * P;
    
    // Slow the puck down a bit when it hits the rink.
    // The value .75 was emperically chosen.
    R = R * .75;

    // Set that direction!
    puck->setNewDirection(R, nextCollisionTime);

    // Compute when the puck will hit the rink.
    rink.setNextCollision(puck);       

    // Compute when shots will hit the puck
    computeShotPuckCollisions(nextCollisionTime);
    
    // What is our next collision?
    markNextCollision();
}

//////////////////////////////////////////////////////////////////////////////
//
// Shot hit puck!
// Change the puck direction and recompute the simulation state.
//
void
SimulationState::shotPuckCollision()
//
//////////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    fprintf(stderr, "shot puck collision: %f!\n", shotPuckCollisionTime.getValue());
#endif
    
    // Update the puck and shot for the collision time
    ShotData *shot = shotPuckCollisionShot;
    puck->update(shotPuckCollisionTime);
    shot->update(shotPuckCollisionTime);
    
    // Compute new direction of puck
    SbVec2f newDir;
    computeNewPuckDir(puck, shot, newDir);
    puck->setNewDirection(newDir, shotPuckCollisionTime);
        
    // Compute when the puck will hit the rink.
    rink.setNextCollision(puck);       

    // Remove the colliding shot from the shot list
    removeShot(shotPuckCollisionId, shotPuckCollisionShot);
    delete shotPuckCollisionShot;

    // Compute when shots will hit the puck
    computeShotPuckCollisions(shotPuckCollisionTime);
    
    // Mark next collision - shot/puck, puck/wall, or nothing
    markNextCollision();
}

//////////////////////////////////////////////////////////////////////////////
//
// Compute a new direction for the puck, based on what
// angle the shot hits the puck.
//
void
SimulationState::computeNewPuckDir(
    PuckData *puck, ShotData *shot, SbVec2f &newDir)
//
//////////////////////////////////////////////////////////////////////////////
{
#define SHOT_PUCK_FORCE_CHANGE 60
    // See how much affect the shot will have on the puck
    // Take the center of puck minus the intersection point to find
    // a force vector.
    SbVec2f F = puck->loc() - shotPuckCollisionLoc;
    F.normalize();
    
    // A direct blow has more impact, and a glancing blow has no impact
    SbVec2f S = shot->dir0();
    float shotspeed = S.normalize();
    
    // The force takes into the direction of the shot,
    // where the shot hits the puck, and jumbles the speed of
    // the shot and the mass of the puck into one constant which
    // we empirically call SHOT_PUCK_FORCE_CHANGE.
    float force = F.dot(S) * SHOT_PUCK_FORCE_CHANGE;

    // New direction is the old direction plus the change resulting
    // from this collision.
    newDir = puck->dir0() + F * force;   
}

//////////////////////////////////////////////////////////////////////////////
//
// Update the state of the simulation for the passed time t.
// Return TRUE if anything changed.
//
SbBool
SimulationState::simulate(const SbTime &t)
//
//////////////////////////////////////////////////////////////////////////////
{
    SbBool somethingChanged = FALSE;

    // Time for a collision?
    int n = 0;
    while (t >= nextCollisionTime) {
	// Remove any pillars that are down before next collision
	for (int p = 0; p < NUM_PILLARS; p++) {
	    if (rink.isPillarUp(p) && 
		(rink.getPillarDownTime(p) < nextCollisionTime))
		rink.setPillarDown(p);
	}
	
	// Deal with the collision
	if (nextCollision == COLLISION_SHOT_HIT_PUCK) {    

	    // Log this collision
	    collisionState->type = nextCollision;
	    collisionState->loc = shotPuckCollisionLoc;
	    if (collisionCB != NULL)
		(*collisionCB)(collisionUserData, collisionState);

	    shotPuckCollision();
	    somethingChanged = TRUE;
	}
	else if ((nextCollision == COLLISION_PUCK_HIT_WALL) || 
		 (nextCollision == COLLISION_PUCK_HIT_GOALPOST) ||
		 (nextCollision == COLLISION_PUCK_HIT_PILLAR)) {

	    // Log this collision
	    collisionState->type = nextCollision;
	    collisionState->loc  = puck->hitLoc();
	    collisionState->norm = puck->hitNormal();
	    if (collisionCB != NULL)
		(*collisionCB)(collisionUserData, collisionState);

	    puckWallCollision();
	    somethingChanged = TRUE;
	}
	else if (nextCollision == COLLISION_PUCK_SCORES_GOAL) {
	    
	    // Log this goal
	    collisionState->type = nextCollision;
	    collisionState->loc = puck->hitLoc();
	    if (collisionCB != NULL)
		(*collisionCB)(collisionUserData, collisionState);

	    // Drop out of this simulation
	    nextCollisionTime = SbTime::max();
	    return FALSE;
	}
    }

    // Remove any pillars that are down by now
    for (int p = 0; p < NUM_PILLARS; p++) {
	if (rink.isPillarUp(p) && 
	    (rink.getPillarDownTime(p) < t))
	    rink.setPillarDown(p);
    }
    
    // Update the puck location
    if (puck->dir0()[0] != 0.0 || puck->dir0()[1] != 0.0)
	somethingChanged = TRUE;
    puck->update(t);
    
    // Update the shot locations for both players
    for (int id = 0; id < 2; id++) {
	if (shotData[id].getLength() > 0) somethingChanged = TRUE;
	// Loop from end to start, since we may remove shots along the way
	for (int s = shotData[id].getLength() - 1; s >= 0; s--) {
	    ShotData *shot = getShot(id, s);
	    
	    // If this shot is out of the rink, remove it
	    // Else, update the shot location to this time slice
	    if (shot->hitTime() < t) {
		removeShot(id, s);
		delete shot; // delete it here!
	    }
	    else shot->update(t);
	}
    }
    
    return somethingChanged;    
}

//////////////////////////////////////////////////////////////////////////////
//
// Add a shot to the list of shots.
//
void
SimulationState::addShot(int id, ShotData *shot)
//
//////////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    SbTime timenow = SbTime::getTimeOfDay();
    if (timenow < shot->time0()) {
	printf("BUG - shot time zero is in the future! bizarre!!!\n");
	return;
    }
#endif
    
    SbBool calculateCollision = TRUE;
    
    // Was shot fired before our last state was computed?
    // If so, get it in sync with our latest state.
    if (shot->time0() < puck->time0()) 
	shot->setNewTimeZero(puck->time0());
   
    // Else, shot was fired after our last state.
    // Was shot fired before the next collision?
    // If so, we can safely update the pucks position to match the
    // time of the shot. We need the puck and shot time zero to
    // match in order for collision calculations to be correct.
    else if (shot->time0() < nextCollisionTime)
	puck->setNewTimeZero(shot->time0());
    
    // Else, shot was fired after our next collision. It cannot possibly be the
    // next thing to collide with the puck, so no need to perform
    // shot/puck collision calculations.
    else calculateCollision = FALSE;
    	
    // See when the shot will hit the rink
    rink.setNextCollision(shot);
    
    // Already gone? If not, add to list of shots
    if (shot->hitTime() > shot->time0()) {
	int whichShot = shotData[id].getLength();
	shotData[id].append(shot);
	
	// See if this shot will hit the puck
	if (calculateCollision) {
	    computeShotPuckCollision(id, whichShot);
	    markNextCollision();
	}
    }
#ifdef DEBUG
    else {
	printf("shot data arrived late - out of rink already\n");
	printf("hit time with rink: %f\n", shot->hitTime().getValue());
	printf("time zero         : %f\n", shot->time0().getValue());
    }
#endif
}

//////////////////////////////////////////////////////////////////////////////
//
// Put a pillar up or down.
//
void
SimulationState::setPillarUp(
    int which,
    const SbTime &timeNow,
    const SbTime &downTime)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Put the pillar up.
    rink.setPillarUp(which, downTime);

    // See if the puck will hit this pillar. We have to update
    // the puck time-zero so our collision tests are in sync,
    // but only do this if the time now is before our next collision.
    if (timeNow < nextCollisionTime) {
	puck->setNewTimeZero(timeNow);
	rink.setNextCollision(puck, which);       
	markNextCollision();
    }

    //??? see if any shots will hit this pillar?
}

//////////////////////////////////////////////////////////////////////////////
//
// Update the passed render state to reflect the current simulation state.
//
void
SimulationState::updateRenderState(RenderState &rstate, const SbTime &timestamp)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Update the shot locations for both players
    for (int id = 0; id < 2; id++) {
	int numShots = shotData[id].getLength();
	rstate.numShots[id] = numShots;
	for (int s = 0; s < numShots; s++) {
	    ShotData *shot = getShot(id, s);
	    rstate.shotLoc[id][s] = shot->loc();
	    rstate.shotDir[id][s] = shot->dir0();
	}
    }
    
    // Update the puck location
    if (puck != NULL)
	rstate.puckLoc = puck->loc();
    
    // Stamp that time
    rstate.timestamp = timestamp;
}

//////////////////////////////////////////////////////////////////////////////
//
// Returns whether the shot will hit the puck, and if so,
// where and when this will occur. Note we have precomputed
// as much as possible.
//
// public, static
//
SbBool
SimulationState::willCollide(
    const ShotData &s,
    const PuckData &p,
    SbVec2f &where, 
    SbTime &when)
//
//////////////////////////////////////////////////////////////////////////////
{
    float A, B, C;

//    
//    A = (sDir.x * sDir.x) + (sDir.y * sDir.y) +
//	  (pDir.x * pDir.x) + (pDir.y * pDir.y) +
//	  (-2 * pDir.x * sDir.x) + (-2 * pDir.y * sDir.y);
//
    A = s.dirDotDir() + p.dirDotDir() +
	(-2 * p.dir0().dot(s.dir0()));

//
//    B = ( 2 * sLoc.x * sDir.x) + ( 2 * sLoc.y * sDir.y) +
//	  (-2 * pLoc.x * sDir.x) + (-2 * pLoc.y * sDir.y) +
//	  ( 2 * pLoc.x * pDir.x) + ( 2 * pLoc.y * pDir.y) +
//	  (-2 * sLoc.x * pDir.x) + (-2 * sLoc.y * pDir.y);
//
    B = s.locDotDirX2() +
	(-2 * p.loc0().dot(s.dir0())) +
	p.locDotDirX2() +
	(-2 * s.loc0().dot(p.dir0()));

//    	
//    C = (sLoc.x * sLoc.x) + (sLoc.y * sLoc.y) +
//	  (-2 * pLoc.x * sLoc.x) + (-2 * pLoc.y * sLoc.y) +
//	  (pLoc.x * pLoc.x) + (pLoc.y * pLoc.y) - (r * r);
//
    C = s.locDotLoc() +
	(-2 * p.loc0().dot(s.loc0())) +
	p.locDotLoc() - p.radiusXradius();
	
    
    float desc = B * B - 4 * A * C;
    
    SbBool doesCollide = FALSE;
    
    if (desc < 0.0) {
	// No intersections, nothing to do
    }
    else if (desc == 0.0) {
	// One intersection
	float t1 = (-B)/(2 * A);
	
	if (t1 > 0) {
	    where = s.loc0() + s.dir0() * t1;
	    when = t1 + s.time0(); 
	    doesCollide = TRUE;
	}
    }
    else {
	float sq = fsqrt(desc);
	float t1 = (-B - sq)/(2 * A);
	float t2 = (-B + sq)/(2 * A);
	
	// Is t1 always smaller than t2???
	if (t1 > 0) {
	    where = s.loc0() + s.dir0() * t1;
	    when = t1 + s.time0();  
	    doesCollide = TRUE;
	}
	else if (t2 > 0) {
	    where = s.loc0() + s.dir0() * t2;
	    when = t2 + s.time0(); 
	    doesCollide = TRUE;
	}
    }
    
    return doesCollide;
}

//////////////////////////////////////////////////////////////////////////////
//
// Compute collisions for all shots.
//
void
SimulationState::computeShotPuckCollisions(const SbTime &t)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Since the puck changed direction and/or speed,
    // we have to recompute collision times for all the shots.
    // Note we do not have to recompute when shots will hit
    // the rink, since that time will be unchanged.
    shotPuckCollisionTime = SbTime::max();	// we will recompute this now
    for (int id = 0; id < 2; id++) {
	// Loop from end to start, since we may remove shots along the way
	for (int s = shotData[id].getLength() - 1; s >= 0 ; s--) {
	    ShotData * shot = (ShotData *) shotData[id][s];
	    
	    // Remove any shot that has already hit the rink
	    if (shot->hitTime() < t) {
		removeShot(id, s);
		delete shot; // delete it here!
	    }
	    // Else compute when/if the shot will hit the puck
	    else {
		shot->setNewTimeZero(t);
		computeShotPuckCollision(id, s);
	    }
	}
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Compute collisions for the named shot.
//
void
SimulationState::computeShotPuckCollision(int id, int s)
//
//////////////////////////////////////////////////////////////////////////////
{
    SbVec2f where;
    SbTime  when;
    ShotData *shot = (ShotData *) shotData[id][s];
    
    // Will there be a collision?
    if (SimulationState::willCollide(*shot, *puck, where, when)) {
	// Will it happen sooner than the currently scheduled collision?
	// And will it happen before the shot leaves the rink?
	if (when < shotPuckCollisionTime &&
	    when < shot->hitTime()) {
	    shotPuckCollisionTime = when;
	    shotPuckCollisionLoc = where;
	    shotPuckCollisionId = id;
	    shotPuckCollisionShot = shot;
	}
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Mark the next collision - shot/puck, puck/wall, or nothing
//
void
SimulationState::markNextCollision()
//
//////////////////////////////////////////////////////////////////////////////
{
    // Assume nothing
    nextCollision = COLLISION_NONE;
    nextCollisionTime = SbTime::max();

    // Will the next collision be shot/puck or puck/wall?
    if (shotPuckCollisionTime < puck->hitTime()) {
	nextCollision = COLLISION_SHOT_HIT_PUCK;
	nextCollisionTime = shotPuckCollisionTime;
    } 
    else if (puck->hitTime() < SbTime::max()) {
	nextCollision = puck->getCollisionType();
	nextCollisionTime = puck->hitTime();
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Reset simulation state, so we can start a new game
//
void
SimulationState::reset()
//
//////////////////////////////////////////////////////////////////////////////
{
    shotData[0].truncate(0);    
    shotData[1].truncate(0);    
    puck = NULL;    

    shotPuckCollisionTime = SbTime::max();
    shotPuckCollisionId   = -1;
    shotPuckCollisionShot = NULL;
    nextCollision = COLLISION_NONE;
    nextCollisionTime = SbTime::max();
}

//////////////////////////////////////////////////////////////////////////////
//
// Reset simulation state, so we can start a new game
//
void
SimulationState::setPuck(PuckData *p)
//
//////////////////////////////////////////////////////////////////////////////
{
    puck = p;
    
    // If puck is moving, compute when the puck will hit the rink.
    if (puck->dir0() != SbVec2f(0, 0)) {
	rink.setNextCollision(puck); 
    }      
}
