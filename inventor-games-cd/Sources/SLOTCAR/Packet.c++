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
 * Copyright 1994   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.33 $
 |
 |   Classes:
 |      packetCB()
 |
 |   Author(s)  : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <X11/Intrinsic.h>
#include <Inventor/fields/SoSFLong.h>
#include <Inventor/nodes/SoSeparator.h>
#include "Network.h"
#include "Packet.h"
#include "State.h"
#include "RaceCar.h"
#include "RobotCar.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the CARINFO packet.
//

void
processCarInfoPacket(
	const CarInfo *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    // Find the car in the state that sent the message
    // If the car is not in the state, it is a new car.  Create
    // a new car and add it to the state.
    // If the car is in the state, update its information.
    Car *car = state->findCar(packet->getPlayer());

    if (car == NULL) {

        // The car that sent this packet is not in the game.  Send a
        // CarQuery packet to find out about it.
        CarQuery queryPacket(packet->getPlayer());
        state->network->send(queryPacket, sizeof(CarQuery));

        return;
    }

    // Check the time stamp of the packet.  If the time is more recent
    // than this game's time, recalibrate this game's time to coincide
    // with the packet's time stamp
    SbTime tmpTime = SbTime::getTimeOfDay() - state->startTime;
    if (tmpTime < packet->getTimeStamp())
        state->startTime -= (packet->getTimeStamp() - tmpTime);

    const TrackPosition &pos = packet->getTrackPosition();

    // Check the position of the packet to see if a race is starting
    // or in progress.  If so, and the car this process is simulating
    // is in practice mode, change the mode of the car and adjust its
    // track position.
    if (state->mode->getValue() == GameState::PRACTICING) {

        if (pos.along < state->track->getLength()*(VICTORY_LAP)) {

            if (pos.along < state->track->getLength()*
			(STARTING_LINE_LAP+1)) {

                // A race is starting.  Must have missed the packet.  Ask for
                // the packet again.  This is needed to get the starting time.
                StartQuery startPacket;
                state->network->send(startPacket, sizeof(StartQuery));
            }
            else {

                // A race is already in progess.  Put this player in
                // racing mode and start all of its cars racing.  Sync
                // the position of cars with the rest of the race.
                state->numCarsFinished = 0;
	        state->mode->setValue(GameState::RACING);
	        for (int i = 0; i < state->getNumCars(); i++) {
		    Car *car = state->getCar(i);
		    car->startRacing();
		    if (car->isSimulated())
			car->syncRacePosition(state->track->getLength());
                }
            }
        }
        else if (pos.along < state->track->getLength()*(VICTORY_LAP+1)) {

            // This car has already finished the race.  If it is not marked
            // as finished in the state, ask it to resend its finish time.
            if (!car->getIsFinished()) {

                FinishQuery finishPacket(car->getId());
                state->network->send(finishPacket, sizeof(FinishQuery));
            }
        }
    }

    car->packetUpdate(packet->getTimeStamp(), pos.along, pos.across,
		      packet->getVelocity().along,
		      packet->getVelocity().across,
		      packet->getTargetLane(), packet->getScreech()); 
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the CARCRASH packet.
//

void
processCarCrashPacket(
	const CarCrash *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    // A car has crashed.  Find that car and tell it to crash itself.
    Car *car = state->findCar(packet->getPlayer());

    // If the car does not exist yet, ignore the crash
    if (car == NULL)
        return;

#ifdef DEBUG
    fprintf(stderr, "DEBUG:  %s has crashed!\n", car->getCarName().getString());
#endif

    if (!car->getIsCrashing())
        car->beginCrash(packet->getTimeStamp(), packet->getTrackPosition(),
			state->track, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the RACESTARTING packet.
//

void
processRaceStartPacket(
	const RaceStartInfo *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    // If we are still in START_SCREEN mode, we are not ready to
    // handle the RACE_STARTING token.  Just return.
    if (state->mode->getValue() == GameState::START_SCREEN)
        return;

    // If we already think the race is starting...
    if (state->mode->getValue() == GameState::RACE_STARTING) {
	// Ignore new start time if later than the old:
	if (state->raceStartTime < packet->getStartTime()) return;
    }
    state->mode->setValue(GameState::RACE_STARTING);
    state->track->clearStartingPositions();
    state->raceStartTime = packet->getStartTime();
    state->myCarStartedRace = FALSE;

    // Get all the cars ready to start:
    for (int i = 0; i < state->getNumCars(); i++) {
	Car *car = state->getCar(i);
	car->getReadyToRace(state->track);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the CARCOLLISION packet.
//

void
processCarCollisionPacket(
	const CarCollision *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    // A car has collided.  Find that car and tell it to crash itself.
    Car *car = state->findCar(packet->getPlayer());

    // If the car does not exist yet, ignore the collision
    if (car == NULL)
        return;

#ifdef DEBUG
    fprintf(stderr, "DEBUG:  %s has collided!\n", car->getCarName().getString());
#endif

    if (!car->getIsCrashing())
        car->beginCrash(packet->getTimeStamp(), packet->getTrackPosition(),
			state->track, TRUE, packet->getSideSpeed());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the CARJOIN packet.
//

void
processCarJoinPacket(
	const CarJoin *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    // If the player has already joined, ignore the packet
    Car *car = state->findCar(packet->getPlayer());
    if (car != NULL)
        return;

    // Create a new car and add it to the state.
#ifdef DEBUG
    fprintf(stderr, "DEBUG:  %s has joined the game.\n", packet->getName());
#endif

    if (packet->getPlayer().getCarNum() < ROBOT_CAR_NUM) {
	car = new RaceCar(packet->getPlayer(),
			  packet->getName(), FALSE);
        if (state->showAnnotation) {
            car->addAnnotation();
        }

        car->setPointTotal(packet->getPointTotal());
        car->setNumGames(packet->getNumGames());        
    }
    else {
	car = new RobotCar(packet->getPlayer());
    }

    // old: SoSeparator *carRoot = car->getCarRoot();
    // old: state->track->getTrack()->addChild(carRoot);
    state->track->addCar( car );
    state->addCar(car);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the CAREXIT packet.
//

void
processCarExitPacket(
	const CarExit *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    // If the player is not around, just ignore the packet
    Car *car = state->findCar(packet->getPlayer());
    if (car == NULL)
        return;

#ifdef DEBUG
    fprintf(stderr, "DEBUG:  %s is leaving the game.\n",
            car->getCarName().getString());
#endif

    state->removeCar(car);
    state->track->removeCar( car );
    state->displayPointTotals();
    delete car;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the CARQUERY packet.
//

void
processCarQueryPacket(
	const CarQuery *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    Car *car = state->findCar(packet->getPlayer());
    if (car == NULL) return;

    if (car->isSimulated()) {

        // Someone is asking about one of my cars.  Send them a join
        // packet
        CarJoin joinPacket(car->getCarNum(),
			   car->getCarName().getString(),
			   car->getPointTotal(),
			   car->getNumGames());
        state->network->send(joinPacket, sizeof(CarJoin));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the STARTQUERY packet.
//    This packet gets sent by a player who notices that other players
//    are lining up before the starting line, but has not received a
//    RACESTARTING packet yet.  When this packet is received, just send
//    out another RACESTARTING packet.
//

void
processStartQueryPacket(
	const StartQuery *,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    RaceStartInfo startPacket(state->raceStartTime);
    state->network->send(startPacket, sizeof(RaceStartInfo));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the FINISHQUERY packet.
//    This packet gets sent by a player who notices that this player
//    has crossed the finish line, but the finish packet has not been 
//    received yet.  This player should send another finish packet.
//

void
processFinishQueryPacket(
	const FinishQuery *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    Car *car = state->findCar(packet->getPlayer());
    if (car == NULL) return;

    if (car->isSimulated()) {

        // Someone is asking about one of my cars.  Send them a finish
	// packet.
        CarFinish finishPacket(car->getCarNum(), car->getFinishTime());
        state->network->send(finishPacket, sizeof(CarFinish));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine processes information from the CARFINISH packet.
//

void
processCarFinishPacket(
	const CarFinish *packet,
	GameState *state)

//
////////////////////////////////////////////////////////////////////////
{
    // If the player is not around, just ignore the packet
    Car *car = state->findCar(packet->getPlayer());
    if (car == NULL)
        return;

#ifdef DEBUG
    fprintf(stderr, "DEBUG:  %s finished the race.\n",
            car->getCarName().getString());
#endif

    // Set the finish time of the car.
    car->justFinished(packet->getTimeStamp());
    if ((state->mode->getValue() == GameState::RACING) ||
        (state->mode->getValue() == GameState::AFTER_RACE))
        ++state->numCarsFinished;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine is the network callback receiving packets of data
//    from other players.
//

void
packetCB( XtPointer userData, int *, XtInputId * )

//
////////////////////////////////////////////////////////////////////////
{
    const Command   *cmd   = NULL;
    GameState *state = (GameState *)userData;

    // Read the socket until it is drained of data
    SbBool gettingData = TRUE;
    
    while (gettingData) { 

	if ((cmd = (const Command *) state->network->get()) == NULL) {
	    // No data on socket - drop back to Xt main loop
	    gettingData = FALSE;
	}
	else { 

            // Depending on the type of packet received, call a different
            // routine to process the packet.
            switch(cmd->getCommand()) {
              case MSG_CARINFO:
        	processCarInfoPacket((const CarInfo *) cmd, state);
        	break;
              case MSG_CARCRASH:
        	processCarCrashPacket((const CarCrash *) cmd, state);
        	break;
              case MSG_CARCOLLISION:
        	processCarCollisionPacket((const CarCollision *) cmd, state);
        	break;
              case MSG_RACESTARTING:
        	processRaceStartPacket((const RaceStartInfo *) cmd, state);
        	break;
              case MSG_CARFINISH:
        	processCarFinishPacket((const CarFinish *) cmd, state);
        	break;
              case MSG_CARJOIN:
        	processCarJoinPacket((const CarJoin *) cmd, state);
        	break;
              case MSG_CAREXIT:
        	processCarExitPacket((const CarExit *) cmd, state);
        	break;
              case MSG_CARQUERY:
        	processCarQueryPacket((const CarQuery *) cmd, state);
        	break;
              case MSG_STARTQUERY:
        	processStartQueryPacket((const StartQuery *) cmd, state);
        	break;
              case MSG_FINISHQUERY:
        	processFinishQueryPacket((const FinishQuery *) cmd, state);
        	break;
              default: 
        	printf("Unknown message: %ld\n", cmd);
        	break;
            }
        }
    }
}
