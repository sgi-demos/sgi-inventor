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
 * Copyright (C) 1990,91,92,93,94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.7 $
 |
 |   Classes:
 |	RobotCar
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

#include <math.h>

#include "RobotCar.h"
#include "State.h"
#include "Track.h"

static const float INITIAL_ROBOT_SKILL = 0.6;
static const float INITIAL_ROBOT_MAXV = 5.0;

#define SCREECH_PER_FRAME 5.0

// This makes sure robots look far enough ahead to avoid crashes even
// if they are going very slowly (or are stopped).
#define VELOCITY_SLOP 0.3

// Geometry shared between all the cars
SoSeparator *RobotCar::sharedGeom;

// Static list of all robot cars, used by simulation...
SbPList RobotCar::roboList;

// Counter used to give robot cars unique numbers.
// Robot cars' numbers start at ROBOT_CAR_NUM.
int RobotCar::robotCarNum = ROBOT_CAR_NUM;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor for a local robocar.  Robot cars control their own
//    numbering, are always named 'Robot', and are never controlled by
//    the mouse.
//

RobotCar::RobotCar() : Car(CarPlayer(0, robotCarNum), "Robot", FALSE)
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize.  The 1000.0 starting position ensures that robot
    // cars start well after the race has finished.
    reportedPosition.along = currentPosition.along = 
	1000.0+robotCarNum*2.0;
    reportedPosition.across = currentPosition.across = 0;
    velocity.along = 1.0;

    ++robotCarNum;
    buildCar();

    // Add some personality to this robot:
    skill = INITIAL_ROBOT_SKILL - drand48()*0.2;
    maxV = INITIAL_ROBOT_MAXV - drand48()*0.8;

    roboList.append((void *)this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor for a robot from over the net.
//

RobotCar::RobotCar(const CarPlayer &_player) : Car(_player, "Robot", FALSE)
//
////////////////////////////////////////////////////////////////////////
{
    buildCar();

    skill = maxV = 0.0;  // These will not be used in this case, but
			 // we'll initialize them anyway to make bugs
			 // easier to find.
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

RobotCar::~RobotCar()
//
////////////////////////////////////////////////////////////////////////
{
    if (isSimulated())
	roboList.remove(roboList.find((void *)this));

    carGraph->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adjust the robot cars' paramaters (targetSpeed and
//    targetLane), based on each car's place on track:
//

void
RobotCar::SimulateRobots(GameState *state, const SbTime &deltaT)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < roboList.getLength(); i++) {
	// We'll be really, really, really dumb for now:
	RobotCar *car = (RobotCar *)roboList[i];

	car->simulate(state, deltaT);
    }
}

static float
min(float a, float b)
{
    return (a < b ? a : b);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Simulate this robot car
//

void
RobotCar::simulate(GameState *state, const SbTime &)
//
////////////////////////////////////////////////////////////////////////
{
    // Figure out how fast we want to go:
    TrackPosition nextP;	// Where we'll be in a bit:
    float desiredV;		// How fast we'll want to be going then
    float trackGrip;		// Grip reported by track

    SbTime deltaT;
    deltaT.setValue(1.0);	// Allow 1 second of space between cars

    // Figure out what section of track we'll be at in deltaT, and
    // figure out what the minimum grip is along the way:
    state->track->getNewPosition(currentPosition, velocity,
				 deltaT, nextP, trackGrip);

    desiredV = trackGrip/GRIP_FACTOR * skill;

    if (desiredV > maxV*skill)
	desiredV = maxV*skill;
    
    targetSpeed = desiredV;

    // See if we want to change lanes; we do if we are on a
    // shoulder, OR there is a car ahead of us we might run into
    // (look ahead down the track a ways):
    if (targetLane == -1) {
	changeLanesSafely(state, deltaT, +1);
    }
    else if (targetLane == NUM_LANES) {
	changeLanesSafely(state, deltaT, -1);
    }
    // Change lanes if there is a car ahead of us, AND change lanes
    // randomly every once in a while for fun:
    else if (carAhead(state, deltaT, currentPosition, targetSpeed)
	     || drand48() < 0.02) {

	// randomly choose left/right:
	int laneChange = 1;
	if (drand48() < .5) laneChange = -1;

	if (!changeLanesSafely(state, deltaT, laneChange)) {
	    if (!changeLanesSafely(state, deltaT, -laneChange)) {
		// If we can't change lanes, better slow down!
		targetSpeed = targetSpeed/4;
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Try to safely change lanes, checking blind spot, etc.  If we
//    can't safely change lanes in the desired direction, returns
//    FALSE.
//

SbBool
RobotCar::changeLanesSafely(GameState *state, const SbTime &deltaT,
			    int deltaLane)
//
////////////////////////////////////////////////////////////////////////
{
    int desiredLane = targetLane+deltaLane;

    // Don't drive onto shoulders:
    if (desiredLane == -1 || desiredLane == NUM_LANES) return FALSE;
    
    // And don't change lanes when we're starting race:
    if (startingPosition.along != -1.0) return FALSE;

    // Get a position behind us:
    TrackPosition negativeV;
    negativeV.along = -targetSpeed;
    negativeV.across = 0.0;
    TrackPosition behindP;
    float junk;	// Don't care about TrackGrip
    state->track->getNewPosition(currentPosition, negativeV, deltaT,
				 behindP, junk);

    // Make sure desiredLane is clear from deltaT behind us to deltaT in
    // front of us:
    behindP.across = desiredLane;
    if (carAhead(state, deltaT*2, behindP, targetSpeed))
	return FALSE;
    
    targetLane = desiredLane;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a state, a time to look ahead, a spot to start from, and a
//    velocity, see if there are any other cars in that interval.
//

SbBool
RobotCar::carAhead(GameState *state, const SbTime &deltaT,
		   const TrackPosition &start, float speed)
//
////////////////////////////////////////////////////////////////////////
{
    TrackPosition v;
    v.along = speed;
    v.across = 0.0;
    TrackPosition end;
    float junk;	// Don't care about TrackGrip
    state->track->getNewPosition(start, v, deltaT, end, junk);
    
    float trackLen = state->track->getLength();

    SbBool result = FALSE;

    // Now, see if any of the other cars have current positions
    // between start and end:
    for (int i = 0; i < state->getNumCars(); i++) {
	Car *car = state->getCar(i);
	if (car == this) continue;
	
	const TrackPosition &p = car->getCurrentPosition();

	// If across positions are close enough:
	if (fabsf(start.across-p.across) < 1.9) {

	    // And along positions match:
	    float p0, p1, mid;
	    p0 = fmod(start.along, trackLen);
	    p1 = fmod(end.along, trackLen);
	    mid = fmod(p.along, trackLen);

	    // Check for special case-- start/end crosses start/finish
	    // line, so need to adjust intervals:
	    if (p1 < p0) {
		p1 += trackLen;
		if (mid < p0) mid += trackLen;
	    }
	    
	    if ((mid > p0) && (mid < p1)) {
		result = TRUE;
		break;
	    }
	}
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return scene graph representing car
//

SoSeparator *
RobotCar::getCarRoot()
//
////////////////////////////////////////////////////////////////////////
{
    return carGraph;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Build a scene graph representing the car.
//

void
RobotCar::buildCar()

//
////////////////////////////////////////////////////////////////////////
{
    if (sharedGeom == NULL) {
	SoInput reader;

	reader.openFile("robotCar.iv");
	sharedGeom = SoDB::readAll(&reader);
	reader.closeFile();
	if (sharedGeom == NULL) {
	    fprintf(stderr, "Couldn't read robot car geometry\n");
	    exit(1);
	}
	sharedGeom->ref();
    }

    // Create the scene graph for the car
    carGraph   = new SoSeparator;
    transform  = new SoTransform;
    screechTransform = new SoTransform;
    carGraph->addChild(transform);
    carGraph->addChild(screechTransform);
    carGraph->addChild(sharedGeom);
    carGraph->ref();

    screechAmount = SCREECH_CUTOFF;
    screechDiff   = 0.0;
    screechSign   = 1.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add the annotation text above the car.
//

void
RobotCar::addAnnotation()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove the annotation text above the car.
//

void
RobotCar::removeAnnotation()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Update the car's position, given the new simulation time and a
//    pointer to the track.
//

void
RobotCar::positionCar(const WorldPosition &position)
//
////////////////////////////////////////////////////////////////////////
{
    transform->translation = position.translation;
    transform->rotation = position.rotation;

    // Tilt the car over based on the screech amount.  To keep the
    // tilt from happening instantaneously, move it gradually.

    if (fabs(screechDiff) < SCREECH_PER_FRAME) {
        screechAmount += screechDiff;
        screechDiff = 0.0;
    }
    else {
        if (screechDiff > 0.0) {
            screechDiff -= SCREECH_PER_FRAME;
            screechAmount += SCREECH_PER_FRAME;
        }
        else {
            screechDiff += SCREECH_PER_FRAME;
            screechAmount -= SCREECH_PER_FRAME;
        }
    }

    float screechAngle;
    if (!getIsCrashing()) 
        screechAngle = -screechSign *
                  (screechAmount-SCREECH_CUTOFF) * M_PI_2/SCREECH_CUTOFF;
    else
        screechAngle = screechSign * M_PI_2;
    screechTransform->rotation.setValue(SbVec3f(0, 0, 1), screechAngle);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Screech the car by the given amount.
//

void
RobotCar::setScreechAmount(float _screech, float curvature)
//
////////////////////////////////////////////////////////////////////////
{
    float newScreech = (_screech < SCREECH_CUTOFF) ? SCREECH_CUTOFF : _screech;
    if (!getIsCrashing())
        newScreech -= (newScreech-SCREECH_CUTOFF) * 0.7;
    if (newScreech == screechAmount)
        return;

    screechDiff   = newScreech - screechAmount;
    screechSign   = (curvature > 0.0) ? -1.0 : 1.0;

    // Depending on the curvature, set the rotation center to be on one
    // side of the car or another.
    if (curvature < 0.0) {
        screechTransform->center.setValue(-CAR_WIDTH*0.5, 0.0, 0.0);
    }
    else if (curvature > 0.0) {
        screechTransform->center.setValue(CAR_WIDTH*0.5, 0.0, 0.0);
    }
}
