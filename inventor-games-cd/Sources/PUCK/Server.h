//  -*- C++ -*-

/*
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.21 $
 |
 |   Description:
 |      Defines the Server class for crossfire.
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _CROSSFIRE_SERVER_
#define _CROSSFIRE_SERVER_

// These port numbers were randomly chosen
#define SERVER_PORT 7777
#define PLAYER_PORT 7778

#include <X11/Intrinsic.h>
#include <Inventor/SbTime.h>
#include "SoSocket.h"
#include "Simulation.h"
#include "msg.h"

class SoTimerSensor;
class SoSensor;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Server
//
//////////////////////////////////////////////////////////////////////////////

class Server {
  public:
    Server();
    ~Server();
      
  protected:
    // Send a command to all players
    void		sendToPlayers(const Command &cmd);
    
    // Send a command to one player
    void		sendToPlayer(const Command &cmd, struct sockaddr_in *p);
    
    // Called when a player wants to join the game
    void		joinGame(struct sockaddr_in *newPlayer, 
				 JoinGame *cmd);
    
    // Called after two players have joined - let the games begin!
    void		startGame();
    
    // Called when one of the players leaves the game => end of game
    void		leaveGame(LeaveGame *cmd);
    
    // Called when a player shoots
    void		shotFired(Shot *shot);
    
    // Called to fire a computer generated shot in single player mode
    void		shootRobotGun();
    
    // Called to update the location/direction of the robot gun
    void		updateRobotGun();

    // Called when a player moves the gun
    void		gunMoved(Gun *gun);
    
    // Called when a goal is scored
    void		goalScored(int whichPlayer);
    
    // This is called to read data from the socket.
    void		processSocket();
    
    // Called each frame to update locations of shots and pucks
    void		updateState();

    // Called to reset all state, so we're ready to start a new game
    void		resetState();
  
    // Drop the puck
    void		dropPuck();
    
    // Add obstacles to the rink to annoy the players!
    void		pillarUp();

    // Called when a player requests a new game
    void		requestNewGame(int playerId);
    
    // Called when players agree to start a new game
    void		restartGame();
    
    // Called when a player sets the game level
    void		setRobotGunRate(int level);
    
    // Set name
    void		setName(int id, const char *name);
    
  private:
    static void		processSocketCB(XtPointer, int *, XtInputId *);
    static void		animationSensorCB(void *userData, SoSensor *);
    static void		pillarSensorCB(void *userData, SoSensor *);
    static void		robotGunSensorCB(void *userData, SoSensor *);
    static void		collisionCB(void *userData, CollisionState *state);

    SoSocket		*socket;
    int			numPlayers;
    struct sockaddr_in	*player[2];
    int			score[2];
    char		*name[2];		
    SoTimerSensor	*animationSensor;
    SoTimerSensor	*pillarSensor;
    SoTimerSensor	*robotGunSensor;
    SbTime		rinkUpdateTime;
    SbBool		gameOn;

    // State of the game. The render state contains the number of shots
    // and number of pucks, along with their current locations.
    // The shotData and puckData lists contain the initial locations
    // and directions of shots and pucks, along with precomputed data
    // used in intersection tests.
    RenderState	    renderState;     // everything needed to render
    SimulationState simulationState; // everything needed to compute collisions
};

#endif
