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
 |   $Revision: 1.14 $
 |
 |   Description:
 |      Definition of Rink class
 | 
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "Rink.h"

// static members
SbVec2f Rink::pillarLocs[NUM_PILLARS];
float Rink::pillarDotProducts[NUM_PILLARS];

//////////////////////////////////////////////////////////////////////////////
//
// Constructor for the Rink class.
//
Rink::Rink()
//
//////////////////////////////////////////////////////////////////////////////
{
    pyGoalWidth = nyGoalWidth = 9.6; // from data/arena.iv
    
    // Init the pillar location array. 
    // It doesn't matter what order the pillars are stored here.
    // Values taken from data/arena.iv
    
    // Player 0 is at -Y in our coordinates,
    // but at +Z in the coordinates of the arena.
    // (So if data file says (30, 0, 65), we use (30, -65)!!!)
    pillarLocs[0].setValue(   0, -80);	// in front of player 0 goal
    pillarLocs[1].setValue(   0,  80);	// in front of player 1 goal
    
    pillarLocs[2].setValue( -30, -65);	// down the sides...
    pillarLocs[3].setValue(  30, -65);
    pillarLocs[4].setValue(  30,  65);
    pillarLocs[5].setValue( -30,  65);
    pillarLocs[6].setValue( -30, -25);
    pillarLocs[7].setValue(  30, -25);
    pillarLocs[8].setValue(  30,  25);
    pillarLocs[9].setValue( -30,  25);
    
    pillarLocs[10].setValue(0, 0);	// center ice
    
    // Precompute dot products
    for (int i = 0; i < NUM_PILLARS; i++) {
	const SbVec2f &v = getPillarLoc(i);
	pillarDotProducts[i] = v.dot(v);
    }
    
    // Put all pillars down.
    for (i = 0; i < NUM_PILLARS; i++)
	pillarUp[i] = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//
// Figure out when and where this puck will hit the rink,
// storing that information on the passed puck. 
//
void
Rink::setNextCollision(PuckData *puck)
//
//////////////////////////////////////////////////////////////////////////////
{
    SbVec2f where;
    SbTime  when;
    SbVec2f normal;
    
    // Will puck hit wall?
    if (objWallCollision(puck, puck->radius(), where, when, normal)) {
	puck->setRinkCollision(where, when, normal);
	puck->setCollisionType(COLLISION_PUCK_HIT_WALL);
	
	// If it is hitting the Y wall, it might score a goal.
	float goalWidth = 0.0;
	if (where[1] == RINK_Y)
	    goalWidth = pyGoalWidth;
	else if (where[1] == -RINK_Y)
	    goalWidth = nyGoalWidth;
	
	if (goalWidth != 0) {  
	    // Does puck pass through the goal?
	    float puckX = fabs(where[0]);
	    if (puckX < (goalWidth - puck->radius())) {
		// Goooooooooooooooaaaaaaaaaaaaaallllllllllllll!
		puck->setCollisionType(COLLISION_PUCK_SCORES_GOAL);
	    }
	    else if (puckX < goalWidth) {
		// Puck hits the post! 
		puck->setCollisionType(COLLISION_PUCK_HIT_GOALPOST);
		
		// Set the intersection point to be the post.
		SbVec2f postLoc;
		if (where[0] > 0.0)
		     postLoc.setValue( goalWidth, where[1]);
		else postLoc.setValue(-goalWidth, where[1]);

		puck->setRinkCollision(postLoc, when, normal);
	    }
	}
    }
    
    // Will puck hit one of the pillars?
    for (int p = 0; p < NUM_PILLARS; p++)
	setNextCollision(puck, p);
}

//////////////////////////////////////////////////////////////////////////////
//
// Compute when a puck will collide with the specified pillar.
//
void
Rink::setNextCollision(PuckData *puck, int which)
//
//////////////////////////////////////////////////////////////////////////////
{
    SbVec2f where;
    SbTime  when;
    SbVec2f normal;
    
    if (objPillarCollision(puck, puck->radius(), where, when, normal, which)) {
	// Hit this before puck hits something else?
	if (when < puck->hitTime()) {
	    puck->setRinkCollision(where, when, normal);
	    puck->setCollisionType(COLLISION_PUCK_HIT_PILLAR);
	}
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Figure out when and where this shot will hit the rink,
// storing that information on the passed shot. 
//
void
Rink::setNextCollision(ShotData *shot)
//
//////////////////////////////////////////////////////////////////////////////
{
    SbVec2f where;
    SbTime  when;
    SbVec2f normal;
    
    // Will shot hit wall or obstacle first?
    if (objWallCollision(shot, 0.0, where, when, normal))
	shot->setRinkCollision(where, when, normal);
    
    // Will shot hit pillar?
    for (int p = 0; p < NUM_PILLARS; p++) {
	if (objPillarCollision(shot, 0.0, where, when, normal, p)) {
	    // Hit this before shot hits something else?
	    if (when < shot->hitTime())
		shot->setRinkCollision(where, when, normal);
	}
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Compute the next collision of this object with the rink wall.
// Return FALSE if there will not be one (i.e. object is not moving).
// This ignores obstacles!
//
// This does take into account the 4 walls, and the corners of the rink.
// It does not consider whether a goal would be scored.
//
// To see if object hits the wall, consider:
//
//	x = Mx + Dx*t
//	y = My + Dy*t
//
//	x = +- RINK_X  (two walls)
//	y = +- RINK_Y  (two walls)
//
// and object never gets closer to wall than its radius.
// So, we have four equations (one for each wall) to solve for t:
//
//	 RINK_X - r = Mx + Dx*t    =>    t = ( RINK_X - Mx - r)/Dx
//	-RINK_X + r = Mx + Dx*t    =>    t = (-RINK_X - Mx + r)/Dx
//	 RINK_Y - r = My + Dy*t    =>    t = ( RINK_Y - My + r)/Dy
//	-RINK_Y + r = My + Dy*t    =>    t = (-RINK_Y - My + r)/Dy
//
// To see if object hits the corners, we need equations for the lines
// which comprise the corners:
//
//	I = y-intercept of the line (i.e. RINK_X + RINK_Y - 4)
//
// Take into account the radius of the object. Since the corner cut makes
// a 45 degree angle, we know that the object will never be closer in the
// x or y dimension than r/sqrt(2)
//
//	K = I - r/sqrt(2)
//
//	y =  x + K => My + Dy*t =  Mx + Dx*t + K => t = (Mx - My + K)/(Dy - Dx)
//      y =  x - K => My + Dy*t =  Mx + Dx*t - K => t = (Mx - My - K)/(Dy - Dx)
//	y = -x + K => My + Dy*t = -Mx - Dx*t + K => t = (-Mx - My + K)/(Dy + Dx)
//	y = -x - K => My + Dy*t = -Mx - Dx*t - K => t = (-Mx - My - K)/(Dy + Dx)
//
SbBool
Rink::objWallCollision(
    MovingObjectData *obj,
    float radius, 
    SbVec2f &where,
    SbTime &when, 
    SbVec2f &normal)
//
//////////////////////////////////////////////////////////////////////////////
{
    SbBool willHit = TRUE;
    
    // x = loc0.x + dir0.x * t --> t = (x - loc0.x)/dir0.x
    // y = loc0.y + dir0.y * t --> t = (y - loc0.y)/dir0.y
    
    SbVec2f xnormal, ynormal;
    float time = FLT_MAX, t;
    
    float locx = obj->loc0()[0];
    float locy = obj->loc0()[1];
    float dirx = obj->dir0()[0];
    float diry = obj->dir0()[1];
    
    // Will it hit either x wall?
    if (dirx > 0.0) {
	time = (RINK_X - locx - radius)/dirx;
	where.setValue(RINK_X, locy + diry * time);
	when = obj->time0() + time;
	normal.setValue(-1, 0);
    }
    else if (dirx < 0.0) {
	time = (-RINK_X - locx + radius)/dirx;
	where.setValue(-RINK_X, locy + diry * time);
	when = obj->time0() + time;
	normal.setValue( 1, 0);
    }
    
    // Will it hit either y wall?
    if (diry > 0.0) {
	t = (RINK_Y - locy - radius)/diry;
	if (t < time) {
	    time = t;
	    where.setValue(locx + dirx * time, RINK_Y);
	    when = obj->time0() + time;
	    normal.setValue(0, -1);
	}
    }
    else if (diry < 0.0) {
	t = (-RINK_Y - locy + radius)/diry;
	if (t < time) {
	    time = t;
	    where.setValue(locx + dirx * time, -RINK_Y);
	    when = obj->time0() + time;
	    normal.setValue(0,  1);
	}
    }
    
    // Will it hit a corner? Check against the four corners.
    // The 4 value here is because the corner cuts in 4 units from the wall.
    // This value is from arena.iv.
#define CORNER_Y_INTERCEPT (RINK_X + RINK_Y - 4)
    float K = CORNER_Y_INTERCEPT - radius*M_SQRT2;

    // It will only hit one corner before a wall, if it hits any at all.
    // So we can use 'else if' through here.
    float b;
    SbVec2f objloc;
    if ((b = (diry - dirx)) != 0.0 &&
        (t = (locx - locy + K)/b) < time &&
	 t > 0.0) {
	time = t;
	objloc = obj->loc0() + obj->dir0() * time;
	when = obj->time0() + time;
	normal.setValue(1, -1);
	where = objloc - normal * radius;
	//printf("will hit -x, +y corner at %f, %f\n", where[0], where[1]);
    }
    else if ((b = (diry - dirx)) != 0.0 &&
             (t = (locx - locy - K)/b) < time &&
	     t > 0.0) {
	time = t;
	objloc = obj->loc0() + obj->dir0() * time;
	when = obj->time0() + time;
	normal.setValue(-1, 1);
	where = objloc - normal * radius;
	//printf("will hit +x, -y corner at %f, %f\n", where[0], where[1]);
    }
    else if ((b = (diry + dirx)) != 0.0 &&
             (t = (-locx - locy + K)/b) < time &&
	     t > 0.0) {
	time = t;
	objloc = obj->loc0() + obj->dir0() * time;
	when = obj->time0() + time;
	normal.setValue(-1, -1);
	where = objloc - normal * radius;
	//printf("will hit +x, +y corner at %f, %f\n", where[0], where[1]);
    }
    else if ((b = (diry + dirx)) != 0.0 &&
             (t = (-locx - locy - K)/b) < time &&
	     t > 0.0) {
	time = t;
	objloc = obj->loc0() + obj->dir0() * time;
	when = obj->time0() + time;
	normal.setValue(1, 1);
	where = objloc - normal * radius;
	//printf("will hit -x, -y corner at %f, %f\n", where[0], where[1]);
    }
    
    // If it won't hit, set a bogus time.
    if (time == FLT_MAX) {
	when = SbTime::max();
	willHit = FALSE;
    }
    
    return willHit;
}

//////////////////////////////////////////////////////////////////////////////
//
// This code computes the intersection point and time of collision
// between a stationary circle (a pillar) and a moving circle (a puck/shot).
// We have to solve 3 equations with 3 unknowns:
//
// Unknowns: x,y,t
//	x,y - intersection point
//	t   - time of intersection
//
// Equations:
//
//	S = stationary circle center (S.x,S.y)
//	M = moving circle with center (M.x,M.y)
//	D = direction vector of moving circle (D.x,D.y) 
//	s = stationary circle radius
//	m = moving circle radius
//
//	(Moving circle)
//	    [x - (M.x + D.x * t)]^2 +
//	    [y - (M.y + D.y * t)]^2 + m^2 = 0
//
//	(Collision - circles never get closer than the sum of their radii)
//	    ||S - (M + D*t)|| = s + m
//
//	(Intersection point - lies on line segment between M and S centers)
//	    I = (M + D*t) + m*(S - (M + D*t))
//
// Solution:
//
// Substitute the Moving Circle equation into the Collision equation:
//
//	sqrt([(M.x + D.x * t) - S.x]^2 + [(M.y + D.y * t) - S.y]^2) = s + m
//
// This gives us the quadratic:
//
//	(D.dot.D)*t^2 +
//	(2*D.dot.M - 2*D.dot.S)*t +
//	(M.dot.M + S.dot.S - 2*(M.dot.S) - m^2 - s^2 - 2*m*s) = 0
//
// Which we can solve for t.
//
// Now, substitute t into the Intersection point equation and we are done!
//
//	P - position of moving circle at time t
//	P = M + D*t
//
//	I = P + m*(S - P)
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
// Compute the next collision of this object with the pillar described by which.
// Return FALSE if there will not be one.
//
SbBool
Rink::objPillarCollision(
    MovingObjectData *obj,
    float radius, 
    SbVec2f &where,
    SbTime &when, 
    SbVec2f &normal, 
    int	which)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (! isPillarUp(which)) return FALSE;
    
    // Pillar is up - find that collision!
    const SbVec2f &S = getPillarLoc(which);	// stationary circle center
    SbVec2f M(obj->loc0());			// moving circle center
    SbVec2f D(obj->dir0());			// moving circle direction
    float s = PILLAR_RADIUS;			// stationary circle radius
    float m = radius;				// moving circle radius

    // A*t^2 + B*t + C = 0, solve for t,
    // A,B,C are described in the big fat comment above.
    float A, B, C;
    A = obj->dirDotDir();
    B = obj->locDotDirX2() - 2 * D.dot(S);
    C = obj->locDotLoc() + 
	pillarDotProducts[which] - 
	2 * M.dot(S) -
	m*m - s*s - 2*m*s;

    // t = (-B +- sqrt(B^2 - 4AC))/2A
    float desc = B * B - 4 * A * C;

    SbBool doesCollide = FALSE;
    
    if (desc < 0.0) {
	// No intersections, nothing to do
    }
    else {
	// Does collide! Find out where.
	float t = -1;	// time of collision
	SbVec2f P;	// position of moving object at collision time
	
	if (desc == 0.0) {
	    // One intersection
	    float t1 = (-B)/(2 * A);
	    
	    if (t1 > 0)
		t = t1;
	}
	else {
	    float sq = fsqrt(desc);
	    float t1 = (-B - sq)/(2 * A);
	    float t2 = (-B + sq)/(2 * A);
	    
	    // Choose t
	    if (t1 > 0)
		t = t1;
	    else if (t2 > 0)
		t = t2;
	}

	// Will object collide with pillar in future?
	// Will pillar still be up?
	SbTime hitTime = t + obj->time0(); // t is a delta. hitTime is absolute.
	if ((t > 0.0) && (hitTime < getPillarDownTime(which))) {
	    // Puck location at collision time
	    P = M + D*t;
	    
	    // The intersection point is on the vector between S and P,
	    // a distance away from S equal to S's radius.
	    SbVec2f dir = P - S;
	    dir.normalize();
	    where = S + s * dir;

	    when = hitTime; 
	    normal = where - S;
	    normal.normalize();
	    
	    doesCollide = TRUE;
	}
    }
    
    return doesCollide;
}

