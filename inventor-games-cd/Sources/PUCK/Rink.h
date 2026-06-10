//  -*- C++ -*-

/*
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.7 $
 |
 |   Description:
 |      Defines the Rink class.
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _PUCK_RINK_
#define _PUCK_RINK_

#include <Inventor/SbTime.h>
#include <Inventor/SbLinear.h>
#include "MovingObject.h"

// Dimensions of the rink - these must match values in rink data file!
#define RINK_X	     60
#define RINK_Y	     90
#define RINK_WIDTH   (2*RINK_X)
#define RINK_LENGTH  (2*RINK_Y)

// There are 11 pillars -
//     8 - 4 on each side X 2 sides
//     2 - 1 in front of each goal X 2 goals
//     1 - at center ice
#define NUM_PILLARS  11
#define CENTER_ICE_PILLAR 10
#define PLAYER_0_PILLAR 0
#define PLAYER_1_PILLAR 1

// from arena.iv
#define PILLAR_RADIUS 2.0

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Rink
//
//////////////////////////////////////////////////////////////////////////////

class Rink {
  public:
    Rink();
    
    // Compute when a puck will collide with the rink walls or pillars.
    void	setNextCollision(PuckData *puck);
    
    // Compute when a puck will collide with the specified pillar.
    void	setNextCollision(PuckData *puck, int whichPillar);

    // Compute when a shot will collide with the rink walls or obstacles.
    void	setNextCollision(ShotData *shot);

    // Set/get whether a pillar is up or not
    void	setPillarUp(int which, const SbTime &downTime)
		    { pillarUp[which] = TRUE, pillarDownTime[which] = downTime; }
    void	setPillarDown(int which)	 { pillarUp[which] = FALSE; }
    SbBool	isPillarUp(int which) const	 { return pillarUp[which]; }
    const SbTime &getPillarDownTime(int which) const 
		    { return pillarDownTime[which]; }
    
    // The red circles in the rink can become pillars.
    // Call this to find out the location of each pillar.
    static const SbVec2f &getPillarLoc(int whichPillar)
		{ return pillarLocs[whichPillar]; }

  protected:
    // Compute shot/wall collisions
    SbBool	objWallCollision(
		    MovingObjectData *obj,  // shot or puck
		    float radius,	    // for shots, this will be zero
		    SbVec2f &where,	    // where obj will be...
		    SbTime &when,	    // and when obj will hit wall
		    SbVec2f &normal);	    // normal of the rink at hit loc

    SbBool	objPillarCollision(
		    MovingObjectData *obj,  // shot or puck
		    float radius,	    // for shots, this will be zero
		    SbVec2f &where,	    // where obj will be...
		    SbTime &when,	    // and when obj will hit wall
		    SbVec2f &normal, 	    // normal of the rink at hit loc
		    int which);		    // which pillar to check

  private:
    float	pyGoalWidth;	// goal width at positive y end of rink
    float	nyGoalWidth;	// goal width at negative y end of rink
    SbBool	pillarUp[NUM_PILLARS];
    SbTime	pillarDownTime[NUM_PILLARS];
    static SbVec2f pillarLocs[NUM_PILLARS];
    static float pillarDotProducts[NUM_PILLARS];
};

#endif
