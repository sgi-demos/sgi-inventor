//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.31 $
 |
 |   Description:
 |      This file contains the definition of the Car and Position classes.
 |
 |   Classes:
 |      Car, Position
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _CAR_
#define  _CAR_

#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbTime.h>

#include "Track.h"

// Standard dimensions of all cars
#define CAR_LENGTH  16.0
#define CAR_WIDTH    7.0
#define CAR_HEIGHT   4.0

// How grippy a standard car is
// (a car will crash if velocity.along*GRIP_FACTOR-trackGrip is > 0)
// (trackGrip is the value returned from track...)
#define GRIP_FACTOR 40.0

// These are the standard dimensions of all cars which can play the
// game.  These values are used for performing collision detection
#define CAR_X_MIN   -2.8
#define CAR_X_MAX    2.8
#define CAR_Y_MIN    0.0
#define CAR_Y_MAX    6.8
#define CAR_Z_MIN  -16.5
#define CAR_Z_MAX    1.0


#define SCREECH_CUTOFF (-40.0)

class SoSeparator;
class SoTransform;
class Network;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: CarPlayer
//
//  This structure holds the unique identification of a player.
//
//
//////////////////////////////////////////////////////////////////////////////

class CarPlayer {
  public:

    CarPlayer(u_long addr, unsigned char carNum);
    CarPlayer(const CarPlayer &player) { netAddr = player.netAddr;
					 carId = player.carId; }

    unsigned char	getCarNum() const { return carId; }
    SbBool		getIsLocal() const { return netAddr == hostAddr; }

    // Equality comparison operator
    friend int          operator ==(const CarPlayer &p1, const CarPlayer &p2)
        { return ((p1.netAddr == p2.netAddr) && (p1.carId == p2.carId)); }

  private:
    u_long		netAddr;  	// Internet address of player
    unsigned char	carId;		// Which car from that address
    static u_long	hostAddr;	// Internet address of this host
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Car
//
//  This class defines a car used in the slotcar game.
//
//////////////////////////////////////////////////////////////////////////////

class Car {
 public:
    Car(const CarPlayer &player, const SbString &myName,
	SbBool _controlled);
    virtual ~Car();

    // Return scene graph representing car.  The car root includes the
    // world space transformation of the car, and the car geom is in the 
    // object space of the car.
    virtual SoSeparator *getCarRoot() = 0;
    SoTransform *getTransform() { return transform; }

    // Update the car's position when fed a packet from the network
    virtual void	packetUpdate(const SbTime &timeStamp, float along,
				     float across, float velocity,
				     float velAcross, signed char targetLane,
				     float screech);

    // Update the car's position, given new time and pointer to the
    // track.  Calls positionCar() to let subclasses put cars at the
    // right spot on the track.  Returns FALSE if car is no longer valid.
    virtual SbBool	updatePosition(const SbTime &newTime, Track *);
    
    // Prepare to race!  This affects updatePosition and setSpeed so
    // that cars automatically slow down as they approach the
    // start/finish line:
    virtual void	getReadyToRace(Track *);

    // Figure out where I'll start (if I can!)
    virtual void	getStartingPosition(Track *);

    // Race!
    virtual void	startRacing();

    virtual void        setScreechAmount(float amt, float curvature) = 0;
    float        	getScreechAmount() const { return screechAmount; }
    void		resetScreech();

    // Add and remove annotation above the car
    virtual void	addAnnotation() = 0;
    virtual void	removeAnnotation() = 0;

    // Initialize the audio used by the car
    void		initializeSounds();
    void		setRevving(SbBool onoff);
    SbBool		getIsRevving() const { return isRevving; }
    void		setScreech(SbBool onoff);
    SbBool		getIsScreeching() const { return isScreeching; }

    signed char		getTargetLane() const { return targetLane; }
    void		setTargetLane(int lane);
	 
    float		getAcrossSpeed() const { return velocity.across; }
    float		getSpeed() const { return velocity.along; }
    void		setSpeed(float s);  // Only call on controlled car

    const TrackPosition &	getLastReportedPosition() const
			    {	return reportedPosition;    }

    const TrackPosition &	getCurrentPosition() const
			    {	return currentPosition;    }

    // Get this car in sync with an existing race
    void		syncRacePosition(float lapLength);

    // Broadcast this car's most recent position to the network, given 
    // a pointer to network:
    void		broadcastPosition(Network *) const;

    // Figure out if I've finished the race.  Called while racing.
    SbBool		crossedFinishLine(Track *);

    // A car has just crossed the finish line.  Set its finish time.
    void		justFinished(const SbTime &t);

    // Info on whether or not the car has finished the race
    SbBool		getIsFinished() const;
    const SbTime &	getFinishTime() const { return finishTime; }

    // Used to broadcast a car's finishing time.
    void		broadcastFinishTime(Network *) const;

    // Figure out if this car is ahead of the given car
    SbBool		isAheadOf(const Car *) const;

    // Returns identification information about the car
    int			getCarNum() const { return id->getCarNum(); }
    const SbString &	getCarName() const { return carName; }
    const CarPlayer &	getId() const { return *id; }
    SbBool		isSimulated() const { return id->getIsLocal(); }
    SbBool		isControlled() const { return controlled; }

    // Set and get information about the crash state of the car
    void		beginCrash(const SbTime &time,
			        const TrackPosition &position,
			        Track *track, SbBool isCollision,
                                float crashSideSpeed = 0.0);
    void		animateCrash(const SbTime &deltaTime, Track *track);
    SbBool		getHasCrashed();
    SbBool		getIsCrashing() const { return isCrashing; }

    void		setPointTotal(int pts) { numPoints = pts; }
    void		setNumGames(int num) { numGames = num; }
    int			getPointTotal() const { return numPoints; }
    int			getNumGames() const { return numGames; }

    void		computeCurrentMatrices();
    const SbMatrix &	getCurrentMatrix() const { return currentMatrix; }
    const SbMatrix &	getCurrentInverse() const { return currentInverse; }

  protected:

    virtual void	positionCar(const WorldPosition &p) = 0;

    // There are two sets of positions/times; currentPosition/Time are
    // as calculated by this process' simulation.
    // reportedPosition/Time is the last information received over the
    // network (for other cars-- if this is the car we're simulating,
    // the two will be equal).

    SoTransform 	*transform;
    SbTime		currentTime;
    TrackPosition	currentPosition;
    SbTime		reportedTime;
    TrackPosition	reportedPosition;
    TrackPosition	crashPosition;
    WorldPosition	crashWorldPosition;
    SbTime		crashTime;
    SbTime		crashStart;
    SbVec3f		crashDirection;
    SbString		carName;
    SbBool		raceStarting;
    TrackPosition	startingPosition;
    SbTime		finishTime;
    float		screechAmount, screechDiff, screechSign;

    // Velocity, expressed as unit track lengths/second and
    // lanes/second
    TrackPosition	velocity;
    float		spinSpeed;

    // The lane we're aiming for when changing lanes:
    signed char		targetLane;

    // The speed we wanna go:
    float		targetSpeed;

  private:

    CarPlayer		*id;		// ID of the car
    SbBool		controlled;	// TRUE if car mouse is controlling
    SbBool		isInit;		// TRUE if car position initialized
    SbBool		hasCrashed;	// Has the car just crashed
    SbBool		isCrashing;	// Is the car currently animating crash
    SbBool		isRevving;	// Is the car's engine on
    SbBool		isScreeching;	// Are the car's tires screeching
    SbBool		needToGetReady; // Need to get ready for next race
    SbBool		playSounds;	// Whether to play sounds or not
    int			revvingID;
    int			screechID;
    int			numPoints, numGames;

    SbMatrix		currentMatrix, currentInverse;  // used for collisions
};

// Cars with car numbers >= this number are robot cars.
#define ROBOT_CAR_NUM 16

#endif /* _CAR_ */
