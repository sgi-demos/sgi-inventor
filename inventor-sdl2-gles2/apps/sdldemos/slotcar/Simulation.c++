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
 |   $Revision: 1.38 $
 |
 |   Classes:
 |      runSimulation()
 |
 |   Author(s)  : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/fields/SoMFLong.h>
#include <Inventor/fields/SoSFLong.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/sensors/SoSensor.h>
#include "Car.h"
#include "Packet.h"
#include "State.h"
#include "Track.h"
#include "RobotCar.h"

void repositionCamera(GameState *);
SbBool checkCollision(Car *, GameState *, int &, float &);
void assignStartingPositions(GameState *state);

// This defines the minimum time between when packets can be sent to
// the net.  This is to prevent slower machines from choking on too
// may packets.
#define SIMULATION_DELTA 0.06
#define COLLISION_SPEED  2.0

// This defines how long to wait between a car stopping near the
// start/finish line and starting the race.
#define START_DELAY 15.0

#define CAR_TRACK_LENGTH .05
#define CAR_TRACK_WIDTH  .85

// How quickly the camera can accelerate:
#define CAMERA_ACCEL_FACTOR 0.1

// Camera parameters; up/down and for/back.  They all have two linear
// scales used to adjust the camera; the arrow keys are used to choose
// between 21 different settings (0 to 20).

// CONSTANT is a constant distance behind car the camera should be:
#define CONSTANT_0	0.0
#define CONSTANT_10	1.0
#define CONSTANT_20	4.0

// STRETCH is a multiplier for car's speed; defines how closely the
// camera tracks the car when it is going fast:
#define STRETCH_0	10.0
#define STRETCH_10	2.5
#define STRETCH_20	.5

// BEHIND is the height behind the camera:
#define BEHIND_0	10.0
#define BEHIND_10	40.0
#define BEHIND_20	80.0

// INFRONT is the height to look in front of car:
#define INFRONT_0	0.0
#define INFRONT_10	15.0
#define INFRONT_20	30.0

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine is called to figure out when cars we're simulating
//    cross the finish line.  When they do, we broadcast their
//    finishing positions.
//

static void
finishCars(GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < state->getNumCars(); i++) {

	Car *car = state->getCar(i);
		
	// If we're simulating this car and it just crossed the finish
	// line:
	if (car->isSimulated() && !car->getIsFinished() && 
	    car->crossedFinishLine(state->track)) {

	    car->broadcastFinishTime(state->network);
	    ++state->numCarsFinished;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine is the idle sensor callback for performing the
//    game simulation. It is called when all pending packets have
//    been read and the game is ready to be updated.  This routine
//    updates the car positions and the game state based on the
//    current game state and the current time.  Then, based on the
//    results of the simulation, packets are sent out and the scene
//    graph is updated.
//

void
runSimulation( void *data, SoSensor *s )

//
////////////////////////////////////////////////////////////////////////
{
    int i;

    GameState *state = (GameState *)data;

    // Calculate a new current game time.
    SbTime newTime;
    SbTime tod = SbTime::getTimeOfDay();
    newTime = tod - state->startTime;

    // Only send out info packets every once in a while, to avoid
    // flooding the net.
    SbBool broadcastPositions;
    if ((tod - state->packetTime).getValue() > SIMULATION_DELTA) {
	broadcastPositions = TRUE;
	state->packetTime = tod;
    } else {
	broadcastPositions = FALSE;
    }
    
    // Simulate robot cars (if any)
    RobotCar::SimulateRobots(state, newTime);

    // Preprocess car information before updating simulation
    for (i = 0; i < state->getNumCars(); i++)
        state->getCar(i)->computeCurrentMatrices();
    
    // Let each car update its position:
    for (i = 0; i < state->getNumCars(); i++) {
        Car *car = state->getCar(i);

	if (!car->updatePosition(newTime, state->track)) {

            // This car has disappeared from the game.  Remove it and
            // redisplay the heads up display.
#ifdef DEBUG
fprintf(stderr, "DEBUG:  %s has left the game.\n",
	    car->getCarName().getString());
#endif
            state->removeCar(car);
            //old: state->track->getTrack()->removeChild(car->getCarRoot());
            state->track->removeCar( car );
            delete car;
	    state->displayPlaces();
            state->displayPointTotals();
	    continue;
        }
    
	// If we're simulating this car, send info about it over net.
	if (car->isSimulated()) {
	    // Only send a packet after a certain amount of time.  If
	    // too many packets are sent too frequently, other
	    // instances of the game may be choked.

	    // Check to see if this car has gone off the track or 
	    // collided with another car.  Collisions and crashes
	    // are broadcasted immediately.
	    int       carIndex;
	    float     collisionSpeed;
	    if (car->getHasCrashed()) {

		// Create a crash packet and send it.
		CarCrash crashPacket(car->getCarNum(), newTime,
				     car->getLastReportedPosition());
		state->network->send(crashPacket, sizeof(CarCrash));
		state->packetTime = tod;
	    }

	    // Check to see if the car has collided with another car
	    // If so, crash the car and send a packet to the other car
	    // telling it that it has crashed.  If my car is not already
	    // crashing from some other crash or collision, crash my car also.
	    if (checkCollision(car, state, carIndex, collisionSpeed)) {

		// Crash the car if it is not already crashing and
		// send a packet saying that it was crashed
		Car *        hitCar = state->getCar(carIndex);
		if (!hitCar->getIsCrashing()) {

		    CarCollision collisionPacket(
			hitCar->getId(), newTime,
			hitCar->getLastReportedPosition(),
			-collisionSpeed);

		    hitCar->beginCrash(
			newTime, hitCar->getLastReportedPosition(),
			state->track, TRUE, -collisionSpeed);
		    state->network->send(
			collisionPacket, sizeof(CarCollision));
		}

		// If my car needs crashing, crash it and send a packet
		if (!car->getIsCrashing()) {
		    CarCollision collisionPacket(
			car->getId(), newTime,
			car->getLastReportedPosition(),
			collisionSpeed);

		    car->beginCrash(newTime, car->getLastReportedPosition(),
				    state->track, TRUE, collisionSpeed);
		    state->network->send(collisionPacket,
					 sizeof(CarCollision));
		}
		state->packetTime = tod;
	    }
	    // Everything is fine.  Send out a packet every so often
	    // reporting the car's current position to the network
	    else if (broadcastPositions) {
		car->broadcastPosition(state->network);
	    }
	}
    }

    // Do mode-specific actions:
    switch(state->mode->getValue()) {
      case GameState::PRACTICING:
      case GameState::RACE_OVER:
	{
	    // Display results from previous race (if any)
	    state->displayPlaces();

	    Car *myCar = state->getControlledCar();
	    
	    // See if the race should start:
	    if (myCar->getSpeed() < 0.1 && 
		state->track->atFinishLine(
		    myCar->getLastReportedPosition())) {

		// Switch into RACE_STARTING mode, and broadcast start
		// packet:
		state->mode->setValue(GameState::RACE_STARTING);
		state->track->clearStartingPositions();
		state->raceStartTime = newTime + SbTime(state->startDelay);
		state->myCarStartedRace = TRUE;
		
		RaceStartInfo startPacket(state->raceStartTime);
		state->network->send(startPacket, sizeof(RaceStartInfo));

		// Get all the cars ready to start:
		for (i = 0; i < state->getNumCars(); i++) {
		    Car *car = state->getCar(i);
		    car->getReadyToRace(state->track);
		}
	    }
	}
	break;

      case GameState::AFTER_RACE:
	{
	    // Let any cars we're still simulating finish:
	    finishCars(state);

	    // Display results from previous race (if any)
	    state->displayPlaces();

            // Check to see if the race is over
            if ((state->numCarsFinished > 2) ||
                (state->numCarsFinished == state->getNumCars())) {
		state->mode->setValue(GameState::RACE_OVER);
                state->numCarsFinished = 0;

                Car *car;
		for (i = 0; i < state->getNumCars(); i++) {
		    car = state->getCar(i);
                    car->setNumGames(car->getNumGames()+1);
                }

                // Award points to the first 3 places;
                car = state->getCar(0);
                car->setPointTotal(car->getPointTotal() + 5);
                car->setNumGames(car->getNumGames() + 1);
                if (state->getNumCars() > 1) {
                    car = state->getCar(1);
                    car->setPointTotal(car->getPointTotal() + 3);
                    car->setNumGames(car->getNumGames() + 1);
                }
                if (state->getNumCars() > 2) {
                    car = state->getCar(2);
                    car->setPointTotal(car->getPointTotal() + 1);
                    car->setNumGames(car->getNumGames() + 1);
                }
                state->displayPointTotals();
            }
        }
        break;

      case GameState::RACE_STARTING:

	// See if the race should begin:
	if (newTime >= state->raceStartTime) {
	    state->mode->setValue(GameState::RACING); // And they're off!
            state->numCarsFinished = 0;
	    for (i = 0; i < state->getNumCars(); i++) {
		Car *car = state->getCar(i);
		car->startRacing();
	    }
	}

	else {
	    // Set the StartTime global field:
	    SoSFLong *countDown =
		    (SoSFLong *)SoDB::getGlobalField("SecsToStart");
	    int newVal = (int)((state->raceStartTime -
			       state->gameTime).getValue()+1.0);
	    
	    if (countDown->getValue() != newVal)
		countDown->setValue(newVal);

	    // And assign starting positions to any cars that are at
	    // the start/finish line and don't already have starting
	    // positions:
	    assignStartingPositions(state);
	}
	break;
      case GameState::RACING:
	{

	    // Figure out who's in first, etc...
	    state->displayPlaces();

	    Car *car = state->getControlledCar();

	    // Update lap counter for controlled car
	    SoSFLong *Lap = (SoSFLong *)SoDB::getGlobalField("Lap");
	    int newLap = (int)(car->getCurrentPosition().along /
			       state->track->getLength()) - 
			       STARTING_LINE_LAP;
	    if (newLap != Lap->getValue())
		Lap->setValue(newLap);

	    // Make cars cross finish line, if they've finished:
	    finishCars(state);

	    if (car->getIsFinished())
		state->mode->setValue(GameState::AFTER_RACE);
	}
	break;
    }

    // Update the camera's position
    repositionCamera(state);

    // Update the state's game time
    state->gameTime = newTime;

    // Reschedule the sensor
    s->schedule();
}

// Helper routine:
static float
cameraInterp(float pos0, float pos10, float pos20, int i)
{
    if (i < 10) return (pos0 + ((pos10-pos0)*i)/10.0);
    
    return (pos10 + ((pos20-pos10)*(i-10))/10.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine mucks around with the camera, making it follow the
//    car.
//

void
repositionCamera(GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    // Where the camera wants to be / wants to look:
    SbVec3f targetEye, targetLook;

    float trackGrip;  // This isn't used.

    // Always track controlled car
    // (someday we might want to add a 'shadow' mode and allow the
    // user to track different cars, but not today!)
    Car *myCar = state->getControlledCar();

    const TrackPosition &carPosition = myCar->getCurrentPosition();

    // Adding to the car's velocity is equivalent to making the camera
    // a constant distance behind the car:
    float behindVelocity;
    float k = cameraInterp(CONSTANT_0, CONSTANT_10, CONSTANT_20,
			   state->cameraBackForth);
    float stretch = cameraInterp(STRETCH_0, STRETCH_10, STRETCH_20,
				 state->cameraBackForth);
    behindVelocity = -(k + myCar->getSpeed()/stretch);

    // Distance (in terms of time) to be behind car:
    SbTime behindTime;
    behindTime.setValue(.8);

    // Calculate a new TargetEye position:
    TrackPosition behindV, behind;
    behindV.along = behindVelocity;
    behindV.across = 0.0;
    state->track->getNewPosition(carPosition, behindV,
				 behindTime, behind, trackGrip);

    WorldPosition behindW;
    state->track->getWorldPosition(behind, behindW);

    float backH =  cameraInterp(BEHIND_0, BEHIND_10, BEHIND_20,
				state->cameraUpDown);
    targetEye = behindW.translation + SbVec3f(0, backH, 0);
    
    // And calculate a new place to look:
    float inFrontVelocity = myCar->getSpeed() + 0.3;
    
    // Distance (in terms of time) to be in front of car:
    SbTime inFrontTime;
    inFrontTime.setValue(.2);

    // Calculate a new targetLook position:
    TrackPosition inFrontV, inFront;
    inFrontV.along = inFrontVelocity;
    inFrontV.across = 0.0;
    state->track->getNewPosition(carPosition, inFrontV,
				 inFrontTime, inFront, trackGrip);

    WorldPosition inFrontW;
    state->track->getWorldPosition(inFront, inFrontW);

    float frontH =  cameraInterp(INFRONT_0, INFRONT_10, INFRONT_20,
				state->cameraUpDown);
    targetLook = inFrontW.translation + SbVec3f(0, frontH, 0);

    // Ok, have a look from and a look to point, the camera will be at
    // the look from point, and will be oriented towards the look to
    // point:

    state->camera->position.setValue(targetEye);

    SbVec3f fromToVec = targetLook - targetEye;
    if (fromToVec.length() > 0.01) {
	(void)fromToVec.normalize();
	
	float angle;
	angle = -atan2f(fromToVec[0],-fromToVec[2]);
	SbRotation r(SbVec3f(0,1,0), angle);

	angle = -asinf(-fromToVec[1]);
	r = SbRotation(SbVec3f(1,0,0), angle) * r;
    
	state->camera->orientation = r;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine checks to see if this player's car is in collision
//    with any other car on the track.  If so, a collision speed is
//    calculated.
//

SbBool
checkBBox(SbVec3f pts[8])

//
////////////////////////////////////////////////////////////////////////
{
    for (int i=0; i<8; i++) {

        if ((pts[i][0] >= CAR_X_MIN) &&
            (pts[i][1] >= CAR_Y_MIN) &&
            (pts[i][2] >= CAR_Z_MIN) &&
            (pts[i][0] <= CAR_X_MAX) &&
            (pts[i][1] <= CAR_Y_MAX) &&
            (pts[i][2] <= CAR_Z_MAX))
            return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine makes a trivial rejection test of one point with
//    the 3D box of the car.  If this test returns TRUE, a collision
//    can be trivially rejected
//

SbBool
trivialReject(SbVec3f &testPt)

//
////////////////////////////////////////////////////////////////////////
{
    if ((fabs(testPt[0]) > 24.0) ||
        (fabs(testPt[1]) > 25.0) ||
        (fabs(testPt[2]) > 36.0))
        return TRUE;
 
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine checks to see if this player's car is in collision
//    with any other car on the track.  If so, a collision speed is
//    calculated.
//

SbBool
checkCollision(Car *myCar, GameState *state, int &carIndex,
 	float &speed)

//
////////////////////////////////////////////////////////////////////////
{
    const TrackPosition & myPosition = myCar->getLastReportedPosition();
    const SbMatrix &      myMat      = myCar->getCurrentInverse();

    // For each other car, get its transformation and combine it with
    // the transformation for my car.  Transform the standard car bounding
    // box by this combined transformation, and test it for intersection.
    for (int i = 0; i < state->getNumCars(); i++) {
        Car *car = state->getCar(i);
	if (car == myCar) continue;

        // Generate the transformed bounding box.  Perform a trivial reject
        // test by looking at the distance of the first point.  If it is
        // Greater than a certain distance, a collision is not possible.
        SbMatrix xform = car->getCurrentMatrix() * myMat;
        SbVec3f  pts[8];
        xform.multVecMatrix(state->carBox[0], pts[0]);
        if (trivialReject(pts[0]))
            continue;

        for (int j=1; j<8; j++)
            xform.multVecMatrix(state->carBox[j], pts[j]);

        // Test for intersection
        if (checkBBox(pts)) {

            // COLLISION!!!  Fill in the player that was hit and the
            // lateral speed the cars should move after the collision
            const TrackPosition &pos = car->getLastReportedPosition();
            carIndex = i;
            if (myPosition.across > pos.across)
                speed = COLLISION_SPEED;
            else
                speed = -COLLISION_SPEED;

            return TRUE;
        }
    }

    // No collision
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This assigns starting positions to cars.  Cars further ahead get
//    first crack at good positions.
//

void
assignStartingPositions(GameState *state)
//
////////////////////////////////////////////////////////////////////////
{
    state->sortCars();

    for (int i = 0; i < state->getNumCars(); i++) {
	Car *car = state->getCar(i);
	car->getStartingPosition(state->track);
    }
}

