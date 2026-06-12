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
 |   $Revision: 1.22 $
 |
 |   Routines:
 |      mouseInputCB()
 |
 |   Author(s)  : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/sensors/SoSensor.h>

#include "RaceCar.h"
#include "State.h"

#ifndef DEBUG
#define NDEBUG
#endif
#include <assert.h>

void
mouseInputCB(void *_gameState, SoEventCallback *eventCBnode)
{
    const SoEvent *event = eventCBnode->getEvent();
    
    GameState *gameState = (GameState *)_gameState;

    Car *car = gameState->getControlledCar();

    // No matter what, keep track of how fast the user wants the car
    // to go:
    if (event->isOfType(SoLocation2Event::getClassTypeId())) {

        // If still in the startScreen, car hasn't been created yet.
        // Just return.
        if (car == NULL) {
	    eventCBnode->setHandled();
            return;
        }

	// Get normalized location:
	const SbViewportRegion &vpReg =
	    eventCBnode->getAction()->getViewportRegion();

	const SbVec2f &position = 
	    event->getNormalizedPosition(vpReg);
	
	// Velocity ranges from 0.0 to 5.0:
	float newSpeed = position[1]*6.0 - 1.0;
	if (newSpeed < 0.0)
	    newSpeed = 0.0;
        if (newSpeed > 5.0)
            newSpeed = 5.0;
	car->setSpeed(newSpeed);
    }

    // If the 'A' key is pressed, toggle on/off the annotation above each
    // car.
    else if (SO_KEY_PRESS_EVENT(event, A)) {

        int i;
        if (gameState->showAnnotation) {
            gameState->showAnnotation = FALSE;
            for (i = 0; i < gameState->getNumCars(); i++) {
                Car *car = gameState->getCar(i);
                if (!car->isSimulated())
                    car->removeAnnotation();
            }
        }
        else {
            gameState->showAnnotation = TRUE;
            for (i = 0; i < gameState->getNumCars(); i++) {
                Car *car = gameState->getCar(i);
                if (!car->isSimulated())
                    car->addAnnotation();
            }
        }
    }
    else if (SO_KEY_PRESS_EVENT(event, UP_ARROW)) {
        if (gameState->cameraUpDown < 20) {
	    ++gameState->cameraUpDown;
	}
    }
    else if (SO_KEY_PRESS_EVENT(event, DOWN_ARROW)) {
        if (gameState->cameraUpDown > 0) {
	    --gameState->cameraUpDown;
	}
    }
    else if (SO_KEY_PRESS_EVENT(event, RIGHT_ARROW)) {
        if (gameState->cameraBackForth < 20) {
	    ++gameState->cameraBackForth;
	}
    }
    else if (SO_KEY_PRESS_EVENT(event, LEFT_ARROW)) {
        if (gameState->cameraBackForth > 0) {
	    --gameState->cameraBackForth;
	}
    }

    switch (gameState->mode->getValue()) {
      case GameState::START_SCREEN:
	if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)) {
	    const SoPickedPoint *pp = eventCBnode->getPickedPoint();
	    if (pp == NULL) break;
	    const SoPath *pickPath = pp->getPath();
	    if (pickPath == NULL) break;
	    const SoNode *pickNode = pickPath->getTail();
	    
	    if (pickNode->getName() == "StartButton") {

                if (gameState->myCarNumber == -1) {

                    // Choose the car for this player.  Pick a unique (if
                    // possible) car number by checking to see which car numbers
                    // have already been chosen.  If 8 or more cars are playing
                    // then cycle through the numbers again.
                    int minEncounters = gameState->getNumCars() / 8;
                    int proposedNumber = (int)(drand48() * 8);
                    for (int i=0; i<8; i++, proposedNumber++) {
                        proposedNumber = proposedNumber%8;

                        // Check the proposed number against the list of cars
                        int numEncounters = 0;
                        int j;	// used after the loop (pre-ANSI scoping)
                        for (j=0; j<gameState->getNumCars(); j++) {
                            if (gameState->getCar(j)->getCarNum() ==
                                    (proposedNumber+1)) {
                                ++numEncounters;
                                if (numEncounters > minEncounters)
                                    break;
                            }
                         }
                        if (j == gameState->getNumCars())
                            break;
                    }
                    gameState->myCarNumber = proposedNumber + 1;
                }

                // Create the car for this player and add it to the scene.
                // Set its initial position on the track using the
                // Car::packetUpdate() method and placing the car somewhere
                // after the finish line.  On the shoulder?
#ifdef DEBUG
fprintf(stderr, "DEBUG:  picking car #%d\n", gameState->myCarNumber);
#endif
                RaceCar *rcar = new RaceCar(CarPlayer(0,
                        gameState->myCarNumber), gameState->playerName, TRUE);
                gameState->addCar(rcar, TRUE);

                // If playing quietly, don't initialize the sounds
                if (!gameState->playQuietly)
		    rcar->initializeSounds();
                rcar->packetUpdate(SbTime::zero(),
                        gameState->track->getLength()*(VICTORY_LAP
                        + 1.1 + drand48()*0.3),
                        0, 0.0, 0.0, -1, SCREECH_CUTOFF);
                // old: SoSeparator *carRoot = rcar->getCarRoot();    
                // old: SoSeparator *trackGeom = gameState->track->getTrack();
                // old: trackGeom->addChild(carRoot);
                gameState->track->addCar( rcar );

                // Set the game state to PRACTICING and start playing!
		gameState->mode->setValue(GameState::PRACTICING);
		gameState->simulationSensor->schedule();

                // Update the positions of any cars that might have joined
                // the race already.
                SbTime newTime;
                SbTime tod = SbTime::getTimeOfDay();
                newTime = tod - gameState->startTime;
                for (int i = 0; i < gameState->getNumCars(); i++) {
                    Car *car = gameState->getCar(i);
                    car->updatePosition(newTime, gameState->track);
                }

                rcar->setRevving(TRUE);
	    }
	}
	break;
      case GameState::PRACTICING:
      case GameState::RACING:
      case GameState::AFTER_RACE:
      case GameState::RACE_OVER:
	{
            // If still in the startScreen, car hasn't been created yet.
            // Just return.
            if (car == NULL) {
	        eventCBnode->setHandled();
                return;
            }

	    if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)) {
		int currentLane = car->getTargetLane();
		if (currentLane > 0) 
		    car->setTargetLane(currentLane-1);
	    }
	    else if (SO_MOUSE_PRESS_EVENT(event, BUTTON3)) {
		int currentLane = car->getTargetLane();
		if (currentLane < (NUM_LANES-1)) 
		    car->setTargetLane(currentLane+1);
	    }
	    eventCBnode->setHandled();
	}
	break;
      case GameState::RACE_STARTING:
	{
            // If still in the startScreen, car hasn't been created yet.
            // Just return.
            if (car == NULL) {
	        eventCBnode->setHandled();
                return;
            }

            // If the race is starting, and the car is already at the
            // start/finish line, don't allow it to change lanes.
            if (gameState->track->atFinishLine(car->getCurrentPosition()))
                break;

	    if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)) {
		int currentLane = car->getTargetLane();
		if (currentLane > 0) 
		    car->setTargetLane(currentLane-1);
	    }
	    else if (SO_MOUSE_PRESS_EVENT(event, BUTTON3)) {
		int currentLane = car->getTargetLane();
		if (currentLane < (NUM_LANES-1)) 
		    car->setTargetLane(currentLane+1);
	    }
	    eventCBnode->setHandled();
	}
	break;
    }
}
