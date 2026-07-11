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
 |   $Revision: 1.47 $
 |
 |   Description:
 |      Definition of the Server class
 | 
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifdef SDL_PORT
#include <SoSDL.h>
#include <unistd.h>
#define SoXt SoSDL
#else
#include <X11/Intrinsic.h>
#include <Inventor/Xt/SoXt.h>
#endif
#include <Inventor/sensors/SoTimerSensor.h>
#include "SoSocket.h"
#include "Server.h"
#include "Simulation.h"
#include "msg.h"
#include "Rink.h"

// 30 frames per second
#define FRAME_RATE 1/30.0

// 2 pillars come up each second
#define PILLAR_RATE 1/2.0

// seconds/shot made by the computer in single player game
#define ROBOT_GUN_RATE_VERY_SLOW 5.0/1.
#define ROBOT_GUN_RATE_SLOW 3.0/1.
#define ROBOT_GUN_RATE_FAST 1.25/1.

// Amount of time after robot shot is fired before puck is hit.
// We'll pick the speed of the shot so that this lag holds true.
#define ROBOT_SHOT_LAG .3

#define GOAL_POINTS 	1
#define WINNING_POINTS 	5

//////////////////////////////////////////////////////////////////////////////
//
// Constructor creates a socket, locates the server, and joins the game.
//
Server::Server()
//
//////////////////////////////////////////////////////////////////////////////
{
    int serverPortNum = SERVER_PORT;
    
    // Allow user to specify a different port number at run time
    char *portNumEnv = getenv("PUCK_SOCKET_PORT");
    if (portNumEnv != NULL)
	serverPortNum = atoi(portNumEnv);
	
    socket = new SoSocket(serverPortNum);
    if (socket->getfd() == -1) {
	exit(0);
    }
    
    numPlayers = 0; // max of 2 players
    player[0] = player[1] = NULL;
    name[0] = name[1] = NULL;
    
    // Have Xt monitor the socket for us
    XtAppAddInput(
	SoXt::getAppContext(),
	socket->getfd(),
	(XtPointer) XtInputReadMask,
	Server::processSocketCB,
	(XtPointer) this);

    animationSensor = new SoTimerSensor(Server::animationSensorCB, this);
    animationSensor->setInterval(FRAME_RATE);

    pillarSensor = new SoTimerSensor(Server::pillarSensorCB, this);
    pillarSensor->setInterval(PILLAR_RATE);
    
    // Robot gun for single player game
    // Set the gun rate to the default value (ROBOT_GUN_RATE_SLOW).  
    // This can be changed at any time by the player.
    robotGunSensor = new SoTimerSensor(Server::robotGunSensorCB, this);
    robotGunSensor->setInterval(ROBOT_GUN_RATE_SLOW);  

    // We need to know when there are collision so we can provide feedback
    simulationState.setCollisionCallback(Server::collisionCB, this);
    
    // Init the state of the game
    renderState.numShots[0] = 0;
    renderState.numShots[1] = 0;

    printf("server is ready.\n");
}

//////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
Server::~Server()
//
//////////////////////////////////////////////////////////////////////////////
{
    delete socket;
    delete animationSensor;
    delete pillarSensor;
    delete robotGunSensor;
    if (name[0] != NULL) free(name[0]);
    if (name[1] != NULL) free(name[1]);
}

//////////////////////////////////////////////////////////////////////////////
//
// Send the command to all players
//
void
Server::sendToPlayers(const Command &cmd)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (player[0] != NULL)
	socket->send((const void *) &cmd, cmd.size(), player[0]);
    if (player[1] != NULL)
	socket->send((const void *) &cmd, cmd.size(), player[1]);
}

//////////////////////////////////////////////////////////////////////////////
//
// Send a command to one player
//
void
Server::sendToPlayer(const Command &cmd, struct sockaddr_in *player)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (player != NULL)
	socket->send((const void *) &cmd, cmd.size(), player);
}

//////////////////////////////////////////////////////////////////////////////
//
// Called when the socket should be read.
//
void
Server::processSocket()
//
//////////////////////////////////////////////////////////////////////////////
{
    Command *cmd;
    struct sockaddr_in addr;

//??? should this be a while loop on a non-blocking socket, like the players?
    cmd = (Command *) socket->get(&addr);
    if (cmd == NULL) return;
    
    switch(cmd->command) {
	case MSG_JOIN:
	    joinGame(&addr, (JoinGame *) cmd);
	    break;
        case MSG_LEAVE:
	    leaveGame((LeaveGame *) cmd);
	    break;
        case MSG_GUN_MOVED:
	    gunMoved((Gun *) cmd);
	    break;
	case MSG_SHOTFIRED:
	    shotFired((Shot *) cmd);
	    break;
	case MSG_NEW_GAME_REQUEST:
	    requestNewGame(((NewGameRequest *) cmd)->id);
	    break;
	case MSG_GAME_LEVEL:
 	    setRobotGunRate(((GameLevel *) cmd)->level);
	    break;
	default: 
	    printf("Unknown message: %ld\n", cmd);
	    break;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Add a player to the game. At most 2 players can play.
//
void
Server::joinGame(struct sockaddr_in *newPlayer, JoinGame *cmd)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Already enough players?
    if (numPlayers == 2) {
	sendToPlayer(Command(MSG_GAMEFULL), newPlayer);
    }
    else {
	// Add the player
	int newPlayerId = -1;
	if (player[0] == NULL) newPlayerId = 0;
	else if (player[1] == NULL) newPlayerId = 1;
	assert(newPlayerId != -1);
	
	player[newPlayerId] = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
	bcopy(newPlayer, player[newPlayerId], sizeof(struct sockaddr_in));
	setName(newPlayerId, cmd->name);
	sendToPlayer(PlayerId(newPlayerId), newPlayer); // give player an id
	numPlayers++;
	
	// Single player game so far?
	// (If so, then player 0 is set, player 1 will be the puck)
	if (numPlayers == 1)
	    setName(1, "the puck");
	
	// Start a new game!
	restartGame();
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Let the games begin!
//
void
Server::startGame()
//
//////////////////////////////////////////////////////////////////////////////
{
    gameOn = TRUE;

    sendToPlayers(StartGame(numPlayers, name));
    
    // Drop the puck!
    dropPuck();
    
    // Schedule the animation sensors
    animationSensor->schedule();
    pillarSensor->schedule();
}

//////////////////////////////////////////////////////////////////////////////
//
// The player of the specified playerid requests a new game
//
void
Server::requestNewGame(int playerid)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (gameOn) {

	if (numPlayers == 2) {
	    // Either player can restart the game at any time, but
	    // we let the other player know why the game has restarted.
    	    if (playerid == 0) 
	    	sendToPlayer(NewGameRequest(0), player[1]);
	    else 
	    	sendToPlayer(NewGameRequest(1), player[0]);
  	} 

	restartGame();
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Restart the game with the same player(s)
//
void
Server::restartGame()
//
//////////////////////////////////////////////////////////////////////////////
{
    int players = numPlayers;	

    resetState();
    numPlayers = players;
    startGame();
}

//////////////////////////////////////////////////////////////////////////////
//
// Tell the players to drop the puck into play,
// and set the initial puck information on the simulation state.
//
void
Server::dropPuck()
//
//////////////////////////////////////////////////////////////////////////////
{
    // The puck starts at some random spot close to center ice.
    float x = (random() % 100)/20.0;  // within 5 units of center
    float y = (random() % 100)/20.0;  // within 5 units of center 
    SbVec2f where(x, y);
    
    // Drop that puck!
    SbTime when = SbTime::getTimeOfDay();
    sendToPlayers(DropPuck(where, when));
    
    // Tell the simulation where the puck is.
    SbVec2f dir(0, 0); // not moving initially
    renderState.puckLoc = where;
    PuckData *pdata = new PuckData(where, dir, when);
    pdata->setRadius(PUCK_RADIUS);
    
    simulationState.setPuck(pdata);
    
    // If single player game, have the computer shoot
    if (numPlayers == 1)
	robotGunSensor->schedule();
}

//////////////////////////////////////////////////////////////////////////////
//
// One player left the game.
// Tell the other player to quit.
//
void
Server::leaveGame(LeaveGame *cmd)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (numPlayers == 2) {
	// One player left - make this a single player game.
    	if (((LeaveGame *) cmd)->id == 0) {
	    // Player 0 left
	    free(player[0]);
	    player[0] = NULL;
	    setName(0, "the puck");
	    
	    // Tell player 1
            sendToPlayer(Command(MSG_PLAYER_LEFT), player[1]);
    	}
	else {
	    // Player 1 left
	    free(player[1]);
	    player[1] = NULL;
	    setName(1, "the puck");
	    
	    // Tell player 0
            sendToPlayer(Command(MSG_PLAYER_LEFT), player[0]);
	}
	
	// Restart with one player
	numPlayers--;
	restartGame();
    }
    else {
	// Single player just quit. We should quit too.
	exit(0);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  Set the robot gun rate to the requested level
//
void
Server::setRobotGunRate(int level)
//
//////////////////////////////////////////////////////////////////////////////
{
    switch (level) {
	case 1:
	    robotGunSensor->setInterval(ROBOT_GUN_RATE_SLOW);
	    break;
	case 2:
	    robotGunSensor->setInterval(ROBOT_GUN_RATE_FAST);
	    break;
	case 0:
	    robotGunSensor->setInterval(ROBOT_GUN_RATE_VERY_SLOW);
	    break;
	default:
	    break;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  Reset all state, so we're ready to start a new game.
//
void
Server::resetState()
//
//////////////////////////////////////////////////////////////////////////////
{
    animationSensor->unschedule();
    pillarSensor->unschedule();
    robotGunSensor->unschedule();
    
    gameOn = FALSE; 

    numPlayers = 0;
    score[0] = 0;
    score[1] = 0;
    renderState.numShots[0] = 0;
    renderState.numShots[1] = 0;

    simulationState.reset();
}

//////////////////////////////////////////////////////////////////////////////
//
// Called when a player notifies a change in its gun position.
// The server sends the info on to the other player.
//
void
Server::gunMoved(Gun *gun)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (gun->playerId == 0)
    	sendToPlayer(OtherGun(gun->translation, gun->rotAngle), player[1]);
    else
    	sendToPlayer(OtherGun(gun->translation, gun->rotAngle), player[0]);
    
}


//////////////////////////////////////////////////////////////////////////////
//
// Called when a player fires a shot.
//
void
Server::shotFired(Shot *shot)
//
//////////////////////////////////////////////////////////////////////////////
{
    int id = shot->playerId;
    if (simulationState.getNumShots(id) == MAXSHOTS) {
#ifdef DEBUG
	fprintf(stderr, "Server: too many shots fired by player %d\n", id);
#endif
	return; // have to ignore this shot...
    }
    
    // Add this shot to the shot data array
    ShotData *newshot = new ShotData(shot->from, shot->dir, shot->timestamp);
    simulationState.addShot(id, newshot);
    
    // Send the state to the players
    updateState();
}

//////////////////////////////////////////////////////////////////////////////
//
// Called to fire a computer generated shot in single player mode
//
void
Server::shootRobotGun()
//
//////////////////////////////////////////////////////////////////////////////
{
    SbVec2f gunLoc;	// our gun
    SbVec2f goalLoc;	// opponents goal
    int robotPlayer;
    
    // Where is the puck right now?
    SbVec2f puckLoc = simulationState.getPuck()->loc();
    SbVec2f puckDir = simulationState.getPuck()->dir0();
    
    // Where is the robot gun and the opponents goal?
    if (player[0] == NULL) {
	robotPlayer = 0;
	gunLoc.setValue(puckLoc[0], -RINK_Y);// use X value of puck to shadow it
	goalLoc.setValue(0.0, RINK_Y);
    }
    else {
	robotPlayer = 1;
	gunLoc.setValue(puckLoc[0], RINK_Y);// use X value of puck to shadow it
	goalLoc.setValue(0.0, -RINK_Y);
    }
    
    // Where will the puck be soon?
    puckLoc = puckLoc + puckDir * ROBOT_SHOT_LAG;
    
    // What direction is the goal?
    SbVec2f goalDir = goalLoc - puckLoc;
    
    // The puck won't get to the goal in one instant.
    // The shot only has so much impact on the puck direction.
#define SHOT_PUCK_FORCE_CHANGE 60 /* from Simulation.c++ */
    goalDir.normalize();
    goalDir *= SHOT_PUCK_FORCE_CHANGE;
    
    // What change in velocity-direction to get the puck in the goal?
    SbVec2f newPuckDir = goalDir - puckDir;
    newPuckDir.normalize();
   
    // What loc on the puck sends the puck in this direction towards the goal?
    SbVec2f sweetSpot = puckLoc - newPuckDir       * PUCK_RADIUS;
    
    // Shoot at this point...
    SbVec2f shotDir = sweetSpot - gunLoc;
   
    // ...at a speed to get there in ROBOT_SHOT_LAG amount of time.
    shotDir *= (1./ROBOT_SHOT_LAG);
    
    Shot shot(robotPlayer, 
		gunLoc[0], gunLoc[1],
		shotDir[0], shotDir[1], 
		SbTime::getTimeOfDay());
    shotFired(&shot);
}

//////////////////////////////////////////////////////////////////////////////
//
// Called each frame to update location and orientation of robot gun
//
void
Server::updateRobotGun()
//
//////////////////////////////////////////////////////////////////////////////
{
    // Where is the puck right now?
    SbVec2f puckLoc = simulationState.getPuck()->loc();
    
    // Where is the robot gun? (shadow the puck)
    SbVec3f gunLoc;
    float angle;
    int robotPlayer;

#define GUN_BARREL_RADIUS  2.0 // from Player.c++

    if (player[0] == NULL) {
	robotPlayer = 0;
	gunLoc.setValue(puckLoc[0], GUN_BARREL_RADIUS, RINK_Y);
	angle = 0.0;
    }
    else {
	robotPlayer = 1;
	gunLoc.setValue(puckLoc[0], GUN_BARREL_RADIUS, -RINK_Y);
	angle = 0.0;
    }
    
    Gun gun(robotPlayer, gunLoc, angle);
    gunMoved(&gun);
}

//////////////////////////////////////////////////////////////////////////////
//
// Called each frame to update locations of shots and pucks
//
void
Server::updateState()
//
//////////////////////////////////////////////////////////////////////////////
{
    // Run a step in the simulation.
    // Only render if something is moving.
    SbTime time = SbTime::getTimeOfDay();
    if (simulationState.simulate(time)) {
	// If single player game, update the robot gun position/direction
	if (numPlayers == 1)
	    updateRobotGun();
	    
	// Copy the relevant simulation state to the render state
	simulationState.updateRenderState(renderState, time);
	
	// Send the state to the players
	sendToPlayers(renderState);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Randomly have pillars rise and fall from the rink floor
// to provide challenging obstacles for the players to shoot around!
//
// When a pillar is chosen to go up, we send a message to the players
// so they can animate the pillar going up. The message also lets the
// player know at what time the pillar should go down.
//
void
Server::pillarUp()
//
//////////////////////////////////////////////////////////////////////////////
{
    // Randomly choose a pillar to go up.
    // (Don't choose the pillar at center ice, which is the last pillar)
    int which = int(random() % (NUM_PILLARS));

    // If already up, then we'll try again next time.
    if (simulationState.isPillarUp(which)) {
	return;
    }
    
    // If puck is inside the circle defining this pillar, 
    // don't bring it up.
    PuckData *puck = simulationState.getPuck();
    SbVec2f v(Rink::getPillarLoc(which) - puck->loc());
    if (v.length() < (puck->radius() + PILLAR_RADIUS)) {
#ifdef DEBUG
	printf("puck right over pillar %d - not bringing pillar up\n", which);
#endif
	return;
    }
    
    // Else, put that pillar up, logging what time it should come down.
    // The simulation class will put it down at that time.
#define PILLAR_UP_TIME 3.0 /* 3 seconds */
    SbTime timeNow = SbTime::getTimeOfDay();
    SbTime downTime = timeNow + PILLAR_UP_TIME;
    simulationState.setPillarUp(which, timeNow, downTime);
    sendToPlayers(PillarUp(which, downTime));
}

//////////////////////////////////////////////////////////////////////////////
//
// Static callback, called by Xt when data is received on the socket
//
void
Server::processSocketCB(XtPointer userData, int *, XtInputId *)
//
//////////////////////////////////////////////////////////////////////////////
{
    Server *server = (Server *) userData;
    server->processSocket();
}

//////////////////////////////////////////////////////////////////////////////
//
// Static callback, called when timer sensor fires
//
void
Server::animationSensorCB(void *userData, SoSensor *)
//
//////////////////////////////////////////////////////////////////////////////
{ 
    ((Server *) userData)->updateState();
}

//////////////////////////////////////////////////////////////////////////////
//
// Static callback, called when a pillar should come up.
//
void
Server::pillarSensorCB(void *userData, SoSensor *)
//
//////////////////////////////////////////////////////////////////////////////
{ 
    ((Server *) userData)->pillarUp();
}

//////////////////////////////////////////////////////////////////////////////
//
// Static callback, called when a pillar should come up.
//
void
Server::robotGunSensorCB(void *userData, SoSensor *)
//
//////////////////////////////////////////////////////////////////////////////
{ 
    ((Server *) userData)->shootRobotGun();
}

//////////////////////////////////////////////////////////////////////////////
//
// Called when a goal scores.
//
void
Server::goalScored(int whichPlayer)
//
//////////////////////////////////////////////////////////////////////////////
{
    score[whichPlayer] += GOAL_POINTS;

    sendToPlayers(GoalScored(whichPlayer, score[whichPlayer]));
    
    // Stop the simulation while the crowd cheers the goal!
    // Restart in a few seconds with a new puck.
    simulationState.reset();
    
    // Turn the robot gun off in single player mode
    if (numPlayers == 1)
	robotGunSensor->unschedule();

    // Winning goal?
    if (score[whichPlayer] >= WINNING_POINTS) {
	// This was the winning goal! Game's over.
	sendToPlayers(GameOver(whichPlayer));
	animationSensor->unschedule();
	pillarSensor->unschedule();
    } 
    else {
	// Wait for a few seconds before dropping the next puck...
	sleep(5);
	dropPuck();	
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Static callback, called when there is a collision.
// Send a message to the players.
//
void
Server::collisionCB(void *userData, CollisionState *state)
//
//////////////////////////////////////////////////////////////////////////////
{ 
    Server *server = (Server *) userData;
    
    // Did a goal score?
    if (state->type == COLLISION_PUCK_SCORES_GOAL) {
    
	// Determine who scored the goal by looking at the y value of the state
	// (+y means player at -y end scored, i.e. player 0)
	int whichPlayer;
	if (state->loc[1] > 0) 
	     whichPlayer = 0; 
	else whichPlayer = 1; 
	
	server->goalScored(whichPlayer);
    }
    else {
	server->sendToPlayers(*state);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Set the name of a player.
//
void
Server::setName(int id, const char *n)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (name[id] != NULL)
	free(name[id]);

    if (n != NULL) {
	name[id] = strdup(n);
    }
}
