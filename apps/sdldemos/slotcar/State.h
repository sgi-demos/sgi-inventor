//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.24 $
 |
 |   Description:
 |      This file contains the definition of the GameState class.
 |
 |   Classes:
 |      GameState
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _GAME_STATE_
#define  _GAME_STATE_

#include <Inventor/SbPList.h>
#include <Inventor/SbTime.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoLists.h>
#include "Network.h"
#include "Car.h"

class SoCamera;
class SoSensor;
class SoSeparator;
class SoSwitch;
#include <Inventor/fields/SoSFLong.h>  // typedef of SoSFInt32; cannot forward-declare

//////////////////////////////////////////////////////////////////////////////
//
//  Class: GameState
//
//  This class contains all state information for the slotcar game.
//
//////////////////////////////////////////////////////////////////////////////

class GameState {
 public:

    SbTime		startTime;	// Time session was started
    SbTime		gameTime;	// Elapsed game time
    SbTime		packetTime;	// Time since last packet was sent
    SbTime		raceStartTime;  // Time race will be/was started
    float		startDelay;	// Time to wait before starting race
    int			myCarNumber;	// Number of my car
    SbBool		myCarStartedRace;// TRUE if my car started race
    Network *		network;	// The network class
    SoSeparator	*	scene;		// Scene to be rendered.
    Track *		track;		// The track
    SoCamera *		camera;		// The view
    SbVec3f		carBox[8];  	// The standard 3D extents of a car
    int			numCarsFinished;// Number of cars that have finished
    SbBool              playQuietly;	// Play audio or not
    SbBool              showAnnotation;	// Display name over cars or not
    SbString            playerName;	// This player's name
    Car *		controlledCar;	// Car controlled by mouse
    int			cameraUpDown;	// 0...20, controls camera
    int			cameraBackForth;// 0...20, controls camera

    enum Mode {
	START_SCREEN = 0,
	PRACTICING = 1,
	RACE_STARTING = 2,
	RACING = 3,
	AFTER_RACE = 4,
	RACE_OVER = 5,
    };
    SoSFLong		*mode;		// Modes global field

    SoSensor *		simulationSensor;

    GameState();
    Car *		findCar(const CarPlayer &player) const;
    Car *		getCar(int which) const { return (Car *)cars[which]; }
    Car *		getControlledCar() const { return controlledCar; }
    int			getNumCars() const { return cars.getLength(); }
    void		addCar(Car *, SbBool mine = FALSE);
    void		removeCar(Car *);
    void		sortCars();
    void		displayPlaces();
    void		displayPointTotals();

  private:
    SbPList 		cars;		// List of cars on the whole track
};

#endif /* _GAME_STATE_ */
