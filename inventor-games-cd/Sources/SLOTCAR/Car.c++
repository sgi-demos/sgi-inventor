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
 |   $Revision: 1.43 $
 |
 |   Classes:
 |	Car
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#include "Packet.h"
#include "Car.h"
#include "Sound.h"

#ifndef DEBUG
#define NDEBUG
#endif
#include <assert.h>

#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/nodes/SoTransform.h>

extern "C" {
get_myaddress( struct sockaddr_in *addr );
}

#define CRASH_FRICTION 0.8
#define SPIN_FRICTION 0.90
#define CRASH_SPIN_SPEED 7.0
#define CRASH_DURATION 6.0
#define WORLD_SCALE 100.0
#define LANE_SWITCH_SPEED 3.0
#define CRASH_SOUND_DURATION 2.95
#define ACCEL_FACTOR 0.2
#define LOST_CAR_TIME 2.0

// Internet ID of this host.
u_long CarPlayer::hostAddr = 0;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//

CarPlayer::CarPlayer(u_long internet_host, unsigned char ID)
//
////////////////////////////////////////////////////////////////////////
{
    if (hostAddr == 0) {
	struct sockaddr_in addr;
	get_myaddress(&addr);
	hostAddr = addr.sin_addr.s_addr;
    }

    // Local player
    if (internet_host == 0) {
	netAddr = hostAddr;
	carId = ID;
    }
    // Car from over the network:
    else {
	netAddr = internet_host;
	carId = ID;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//

Car::Car(const CarPlayer &player, const SbString &myName,
	 SbBool _controlled)
//
////////////////////////////////////////////////////////////////////////
{
    id = new CarPlayer(player);

    carName = myName;
    controlled = _controlled;

    currentTime = reportedTime = finishTime = SbTime::zero();
    isInit = FALSE;
    hasCrashed = FALSE;
    isCrashing = FALSE;
    needToGetReady = FALSE;
    spinSpeed  = CRASH_SPIN_SPEED;
    targetLane = 0;
    playSounds   = FALSE;
    revvingID    = -1;
    screechID    = -1;
    isRevving    = FALSE;
    isScreeching = FALSE;
    targetSpeed  = 0.0;
    velocity.across = velocity.along = 0.0;
    raceStarting = FALSE;
    startingPosition.along = startingPosition.across = -1;
    reportedPosition.along = currentPosition.along = -1;
    reportedPosition.across = currentPosition.across = 1.0;
    numPoints = numGames = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//

Car::~Car()

//
////////////////////////////////////////////////////////////////////////
{
    delete id;
    if (revvingID >= 0)
        Sound::endContinuous(revvingID);
    if (screechID >= 0)
        Sound::endContinuous(screechID);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the audio used by the car.
//

void
Car::initializeSounds()

//
////////////////////////////////////////////////////////////////////////
{
    revvingID = Sound::initContinuous("./motorLoop.aiff", 1.5);		
    screechID = Sound::initContinuous("./screech.aiff", 1.0);
    playSounds = TRUE;
}

/////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turn the car revving on and off.
//

void
Car::setRevving(SbBool onoff)

//
////////////////////////////////////////////////////////////////////////
{
    if ((revvingID >= 0) && isControlled()) {
        Sound::setPlayContinuous(revvingID, onoff, 1.5);
    }
    isRevving = onoff;		
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//    Turn the car tire screeching on and off.
//

void
Car::setScreech(SbBool onoff)

//
////////////////////////////////////////////////////////////////////////
{
    if ((screechID >= 0) && isControlled()) {
        Sound::setPlayContinuous(screechID, onoff);
    }
    isScreeching = onoff;		
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set this car's target lane.
//

void
Car::setTargetLane(int lane)
//
////////////////////////////////////////////////////////////////////////
{
    // If we're easing up to start line:
    if (raceStarting && (startingPosition.along != -1))
	return;

    targetLane = lane;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set this car's speed.  May only be called on the controlled car.
//

void
Car::setSpeed(float s)
//
////////////////////////////////////////////////////////////////////////
{
    assert(isControlled());

    if (!isCrashing) {
	// If the car is on the shoulder, make it go slower.
	if ((targetLane == -1) || (targetLane == NUM_LANES))
	    targetSpeed = s / 4.0;
	else
	    targetSpeed = s;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return whether the car has just crashed or not.
//

SbBool
Car::getHasCrashed()

//
////////////////////////////////////////////////////////////////////////
{
    if (hasCrashed) {
        hasCrashed = FALSE;
        return TRUE;
    }
    else
        return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Update car's position, given a absolute position/velocity/etc
//

void
Car::packetUpdate(const SbTime &timeStamp,
		  float _along, float _across, float _velocity,
		  float _velAcross, signed char _targetLane, float _screech)

//
////////////////////////////////////////////////////////////////////////
{
    // Ignore out-of-date info:
    if (timeStamp < reportedTime) {
//        fprintf(stderr, "ignoring out of date info\n");
        return;
    }
    
    reportedTime = timeStamp;
    
    // If the car is crashing, don't update it's info
    if (isCrashing) {
        reportedPosition.across = _across;
        reportedPosition.along = _along;
        velocity.along = _velocity;
        velocity.across = _velAcross;

        targetLane = _targetLane;
        return;
    }

    reportedPosition.across = _across;
    reportedPosition.along = _along;

    velocity.along = _velocity;
    screechAmount  = _screech;
    targetLane     = _targetLane;

    if (targetLane < reportedPosition.across)
	velocity.across = -LANE_SWITCH_SPEED;
    else if (targetLane > reportedPosition.across)
	velocity.across = LANE_SWITCH_SPEED;
    else velocity.across = 0;

    // If the car has not been initialized yet, set it's current position
    // to be the reported position.
    if (!isInit) {
        currentPosition = reportedPosition;
        isInit = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Update the car's position, given the new simulation time and a
//    pointer to the track.  Returns FALSE if have not heard from the
//    car in awhile.
//

SbBool
Car::updatePosition(const SbTime &newTime, Track *track)
//
////////////////////////////////////////////////////////////////////////
{
    float trackGrip;

    assert(newTime > currentTime);

    // If have not heard from the car in awhile, assume it has left the
    // game and you have not received an exit packet.
    if (!isSimulated() && ((newTime-reportedTime).getValue() > LOST_CAR_TIME))
        return FALSE;

    // If the car has crashed, animate it through its crash animation
    if (isCrashing) {
        animateCrash(newTime - currentTime, track);

        // After a certain amount of time, place the car back on the track
        // with 0 velocity.  Choose the new track location by putting
        // in a random factor to reduce the chance that two crashed cars will
        // reappear in the same place only to crash again.
        if ((currentTime - crashStart).getValue() > CRASH_DURATION) {
            velocity.along = 0.0;
            velocity.across = 0.0;
            targetSpeed = 0.0;
            isCrashing = FALSE;
            resetScreech();
            if (isSimulated()) {
                currentPosition.across = (drand48() > 0.5) ? NUM_LANES : -1;
                currentPosition.along  = crashPosition.along -
                    3.0 * drand48();
                targetLane = (int)(currentPosition.across);
                setRevving(TRUE);
                hasCrashed = FALSE;
            }
            
            // if a race is starting while the car had been crashing,
            // set it up for the race now.
            if (needToGetReady) {
                needToGetReady = FALSE;
                getReadyToRace(track);
            }
        }

        currentTime = newTime;

        // If this is the car owned by this simulation process...
        if (isSimulated()) {

	    reportedPosition = currentPosition;
	    reportedTime = currentTime;
        } 
        return TRUE;      
    }

    // Adjust across velocity based on current lane/target lane:
    if (currentPosition.across > targetLane) {
	velocity.across = -LANE_SWITCH_SPEED;
    }
    else if (currentPosition.across < targetLane) {
	velocity.across = LANE_SWITCH_SPEED;
    }

    // Adjust along velocity based on the target velocity set by the
    // mouse and the current velocity.  Ramp the speed.
    if (isSimulated()) {
        float diffVelocity = targetSpeed - velocity.along;
        if (diffVelocity != 0.0) {
            velocity.along += diffVelocity * ACCEL_FACTOR;
            if ((revvingID >= 0) && isControlled()) {
                float newNoise = velocity.along * 0.5 + 1.5;
                Sound::setContinuousPitch(revvingID, newNoise);
            }
        }
    }

    // If we're getting close to our starting position, we may need to
    // increase or decrease our velocity:
    if (raceStarting && (startingPosition.along != -1)) {

	// If we're at, or somehow managed to go past our starting
	// position, STOP!
	if (currentPosition.along >= startingPosition.along) {
	    velocity.along = 0.0;
	} else {
	    float maxVelocity = 
		(startingPosition.along - currentPosition.along)*3.0;
	    if (velocity.along > maxVelocity)
		velocity.along = maxVelocity;

	    float minVelocity = maxVelocity/6.0;
	    if (minVelocity < 0) minVelocity = 0;
	    if (velocity.along < minVelocity)
		velocity.along = minVelocity;
	}
    }

    track->getNewPosition(reportedPosition, velocity, newTime -
			  reportedTime, currentPosition, trackGrip);

    // If we went past targetLane:
    if (velocity.across < 0 && currentPosition.across < targetLane) {
	currentPosition.across = targetLane;
	velocity.across = 0;
    }
    else if (velocity.across > 0 && currentPosition.across > targetLane) {
	currentPosition.across = targetLane;
	velocity.across = 0;
    }

    // If this is the player's car, determine whether it has crashed or not.
    // If the car has not crashed, see if the tires should screech or not.
    if (isSimulated() && !raceStarting) {
#ifdef DEBUG
//    fprintf(stderr, "DEBUG:  velocity and grip:  %f %f\n",
//velocity.along * GRIP_FACTOR, trackGrip);
#endif
        float gripDiff = velocity.along * GRIP_FACTOR - trackGrip;
        if (gripDiff > 0.0) {

            // THIS CAR HAS CRASHED!!!
            setScreechAmount(0.0, track->getCurvature(currentPosition));
            beginCrash(newTime, currentPosition, track, FALSE);
        }
        else if (gripDiff > SCREECH_CUTOFF) {
            setScreech(TRUE);
            setScreechAmount(gripDiff,track->getCurvature(currentPosition));
        }
        else {
            setScreech(FALSE);
            setScreechAmount(-trackGrip, 0.0);
        }
    }
    else if (!isSimulated())
        setScreechAmount(screechAmount,track->getCurvature(currentPosition));

    currentTime = newTime;
    
    // Update scene graph:
    WorldPosition p;
    track->getWorldPosition(currentPosition, p);
    positionCar(p);

    // If this is the car owned by this simulation process...
    if (isSimulated()) {
	reportedPosition = currentPosition;
	reportedTime = currentTime;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get the car ready to race.  This affects updatePosition and
//    setSpeed.
//

void
Car::getReadyToRace(Track *track)
//
////////////////////////////////////////////////////////////////////////
{
    // If the car is currently crashing, hold off until it is done with
    // its crash animation.
    if (isCrashing) {
        needToGetReady = TRUE;
        return;
    }

    raceStarting = TRUE;
    startingPosition.along = -1;

    // Adjust current, reported positions:
    float length = track->getLength();
    currentPosition.along = fmod(currentPosition.along, length) +
            length*STARTING_LINE_LAP;
    reportedPosition.along = fmod(reportedPosition.along, length) +
            length*STARTING_LINE_LAP;

    finishTime = SbTime::zero();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    When we hit the starting track, we'll ask the track for our
//    starting position (if we haven't already):
//

void
Car::getStartingPosition(Track *track)
//
////////////////////////////////////////////////////////////////////////
{
    // Bail if already have starting position
    if (startingPosition.along != -1) return;
    
    if (track->atFinishLine(currentPosition)) {
	track->assignStartingPosition(currentPosition, startingPosition);

	// Head for starting lane:
	targetLane = (signed char)startingPosition.across;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Start racing.  Undoes the effect of getReadyToRace.
//

void
Car::startRacing()
//
////////////////////////////////////////////////////////////////////////
{
    raceStarting = FALSE;
    startingPosition.along = -1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Make the lap position of this car to be between 0 and 1 laps.
//

void
Car::syncRacePosition(float lapLength)
//
////////////////////////////////////////////////////////////////////////
{
    raceStarting = FALSE;
    reportedPosition.along = fmod(reportedPosition.along, lapLength) +
            lapLength*STARTING_LINE_LAP;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the car into the crash animation.
//

void
Car::beginCrash(
	const SbTime &time,
	const TrackPosition &position,
	Track *track,
        SbBool isCollision,
	float acrossSpeed)
//
////////////////////////////////////////////////////////////////////////
{
    // If the car is in its starting position, ignore crashes.
    if (startingPosition.along != -1.0)
        return;

    isCrashing    = TRUE;
    crashPosition = position;
    crashTime     = time;
    crashStart    = time;
    spinSpeed     = CRASH_SPIN_SPEED;
    reportedTime  = time;
    reportedPosition = position;

    float curvature = track->getCurvature(crashPosition);
    if (!isCollision)
        setScreechAmount(0.0, curvature);
    else if (acrossSpeed > 0.0)
        setScreechAmount(0.0, 1.0);
    else
        setScreechAmount(0.0, -1.0);

    if (isSimulated()) {
        hasCrashed = TRUE;
	setRevving(FALSE);
	setScreech(FALSE);
	if (isControlled() && playSounds) {
	    Sound::playOnce("./crash.aiff");
	}
    }

    // The car will crash by animating to the far lane and spinning in
    // the shoulder.  Set the target lane to be the farthest lane in the
    // direction the car is moving.  Give it some across velocity.
    velocity.across = 2.0 * acrossSpeed;
    if (velocity.across > 0.0) {
        setTargetLane(NUM_LANES+1);
    }
    else if (velocity.across < 0.0) {
            setTargetLane(-1);
    }
    else {
        if (curvature > 0.0) {
            setTargetLane(NUM_LANES+1);
            velocity.across = NUM_LANES+1;
        }
        else {
            setTargetLane(-1);
            velocity.across = -NUM_LANES;
        }
    }

    track->getWorldPosition(crashPosition, crashWorldPosition);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Animate the car crash.
//

void
Car::animateCrash(const SbTime &deltaTime, Track *track)
//
////////////////////////////////////////////////////////////////////////
{
    float trackGrip;

    track->getNewPosition(reportedPosition, velocity, deltaTime,
            currentPosition, trackGrip);

    // Update scene graph:
    WorldPosition p;

    SbRotation deltaRot(SbVec3f(0, 1, 0),
            deltaTime.getValue() * spinSpeed);
    track->getWorldPosition(currentPosition, p);
    crashWorldPosition.translation = p.translation;
    crashWorldPosition.rotation *= deltaRot;
    positionCar(crashWorldPosition);

    // Reduce the velocity by some amount
    velocity.along  *= CRASH_FRICTION;    
    velocity.across *= CRASH_FRICTION;    
    spinSpeed       *= SPIN_FRICTION;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Send the car's position information to the network.
//

void
Car::broadcastPosition(Network *network) const
//
////////////////////////////////////////////////////////////////////////
{
    // Create a CarInfo packet and fill it with the car's data
    CarInfo cinfo(id->getCarNum(), targetLane, currentPosition, velocity,
		  screechAmount, currentTime);

    // Send the packet off to the network
    network->send(cinfo, sizeof(CarInfo));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out if we have finished the race.  If we have, remember
//    our finish time.
//

SbBool
Car::crossedFinishLine(Track *track)
//
////////////////////////////////////////////////////////////////////////
{
    if (finishTime != SbTime::zero()) return TRUE;
    
    // See if we've crossed finish line:

    float finishLine = track->getLength()*(VICTORY_LAP);

    if (currentPosition.along >	finishLine) {
	// Figure out our finishing time
	// We know currentPosition, currentTime, velocity, and where
	// the finish line is.  To make it easier to calculate, we'll
	// assume the track after the finish line is a unit-length
	// straight track-- that track-space isn't being stretched by
	// it.  So, our finish time is just the distance we're over
	// the finish line times our speed, subtracted from the
	// current time.
	float distanceOverLine = currentPosition.along - finishLine;
	float timeOverLine = distanceOverLine*velocity.along;
	finishTime = currentTime - SbTime(timeOverLine);

	return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return TRUE if the car has finished the race.
//

SbBool
Car::getIsFinished() const
//
////////////////////////////////////////////////////////////////////////
{
    if (finishTime == SbTime::zero()) return FALSE;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Send the car's finish time to the network.
//

void
Car::broadcastFinishTime(Network *network) const
//
////////////////////////////////////////////////////////////////////////
{
    assert(finishTime != SbTime::zero());
    
#ifdef DEBUG
    fprintf(stderr, "DEBUG:  I crossed the finish line!\n");
#endif
 
    // Create a finish packet and fill it with the time the car crossed
    // the finish line.
    CarFinish finish(id->getCarNum(), finishTime);

    // Send the packet off to the network
    network->send(finish, sizeof(CarFinish));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return TRUE if this car is ahead of the given car
//

SbBool
Car::isAheadOf(const Car *otherCar) const
//
////////////////////////////////////////////////////////////////////////
{
    if (getIsFinished()) {
	if (!otherCar->getIsFinished()) return TRUE;
	else if (finishTime < otherCar->finishTime) return TRUE;
    } else {
	if (otherCar->getIsFinished()) return FALSE;
	
	else if (currentPosition.along > otherCar->currentPosition.along)
	    return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the finish time of the car.  This method is used when packets
//    arrive from other cars with their finish times.
//

void
Car::justFinished(const SbTime &t)
//
////////////////////////////////////////////////////////////////////////
{
    finishTime = t;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets the screeching.
//

void
Car::resetScreech()
//
////////////////////////////////////////////////////////////////////////
{
    screechAmount = SCREECH_CUTOFF;
    screechSign   = 1.0;
    screechDiff   = 1.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Compute the current transformation matrices.
//

void
Car::computeCurrentMatrices()
//
////////////////////////////////////////////////////////////////////////
{
    getTransform()->getTranslationSpaceMatrix(currentMatrix,
					      currentInverse);
}
