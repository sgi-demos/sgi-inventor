//  -*- C++ -*-

/*
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.32 $
 |
 |   Description:
 |      Defines the Player and Server classes for crossfire.
 |
 |   Author(s): David Mott, Helga Thorvaldsdottir
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _CROSSFIRE_
#define _CROSSFIRE_

#include <Inventor/SbTime.h>
#include <Inventor/SbLinear.h>
#include "SoSocket.h"

// These port numbers were randomly and arbitrarily chosen
#define SERVER_PORT 7777
#define PLAYER_PORT 7778

// Puck radii
#define PUCK_HEIGHT	1.5
#define PUCK_RADIUS	3

// Maximum number of shots by each player on the rink at one time
#define MAXSHOTS 7

// Maximum number of pucks allowed on the rink at one time
#define MAXPUCKS 1

// Messages
#define    MSG_JOIN		1
#define    MSG_GAMEFULL		2
#define    MSG_STARTGAME	3
#define    MSG_SHOTFIRED	4
#define    MSG_PLAYERID		5	
#define    MSG_UPDATESTATE	6
#define    MSG_LEAVE		7
#define    MSG_PLAYER_LEFT	8
#define    MSG_DROP_PUCK	9
#define    MSG_GOAL_SCORED	10
#define    MSG_GAME_OVER	11
#define    MSG_NEW_GAME_REQUEST 12
#define    MSG_PILLAR_UP	14
#define    MSG_COLLISION	15
#define    MSG_GUN_MOVED	16
#define    MSG_MOVE_OTHER_GUN 	17
#define    MSG_GAME_LEVEL 	18

//////////////////////////////////////////////////////////////////////////////
// The code assumes all messages sent and received
// are ultimately derived from this Command class.
// This way, received messages can perform a switch
// on the command found here.
//////////////////////////////////////////////////////////////////////////////
class Command {
  public:
    virtual int	    size() const { return sizeof(Command); }
    
    // The command, e.g. MSG_JOIN
    long    command;
    
    Command(long cmd)
	{ command = cmd; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the player to the server when the player joins the game
//////////////////////////////////////////////////////////////////////////////
class JoinGame : public Command {
  public:
    virtual int	    size() const { return sizeof(JoinGame); }
    
    char   name[50];
    
    JoinGame(char *n) : Command(MSG_JOIN)
	{ strcpy(name,n); }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the server to the player to let
// the player know what its id is (MSG_PLAYERID)
//////////////////////////////////////////////////////////////////////////////
class PlayerId : public Command {
  public:
    virtual int	    size() const { return sizeof(PlayerId); }
    
    int    id;
    
    PlayerId(int i) : Command(MSG_PLAYERID)
	{ id = i; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the server to the players to start the game
//////////////////////////////////////////////////////////////////////////////
class StartGame : public Command {
  public:
    virtual int	    size() const { return sizeof(StartGame); }
    
    SbTime	timestamp;
    int		numPlayers;
    char	names[2][50];
    
    StartGame(int i, char **n);
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the player to the server when the player leaves the game
//////////////////////////////////////////////////////////////////////////////
class LeaveGame : public Command {
  public:
    virtual int	    size() const { return sizeof(LeaveGame); }
    
    int    id;
    
    LeaveGame(int i) : Command(MSG_LEAVE)
	{ id = i; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the player to the server when the player wants to start
// a new game.
//////////////////////////////////////////////////////////////////////////////
class NewGameRequest : public Command {
  public:
    virtual int	    size() const { return sizeof(NewGameRequest); }
    
    int    id;
    
    NewGameRequest(int i) : Command(MSG_NEW_GAME_REQUEST)
	{ id = i; }
};


//////////////////////////////////////////////////////////////////////////////
// This is sent from the player to the server to set the game level
//////////////////////////////////////////////////////////////////////////////
class GameLevel : public Command {
  public:
    virtual int	    size() const { return sizeof(GameLevel); }
    
    int    level;
    
    GameLevel(int i) : Command(MSG_GAME_LEVEL)
	{ level = i; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the server to the player when a goal is scored
//////////////////////////////////////////////////////////////////////////////
class GoalScored : public Command {
  public:
    virtual int	    size() const { return sizeof(GoalScored); }
    
    int    whoScored;	// player id
    int    score;       // updated score for the scoring player
    
    GoalScored(int i, int s) : Command(MSG_GOAL_SCORED)
	{ whoScored = i, score = s; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the server to the player when the game is over
//////////////////////////////////////////////////////////////////////////////
class GameOver : public Command {
  public:
    virtual int	    size() const { return sizeof(GameOver); }
    
    int    whoWon;	// player id
    
    GameOver(int i) : Command(MSG_GAME_OVER)
	{ whoWon = i; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the server to the player to drop the puck
//////////////////////////////////////////////////////////////////////////////
class DropPuck : public Command {
  public:
    virtual int	    size() const { return sizeof(DropPuck); }
    
    SbVec2f where;
    SbTime  timestamp;
    
    DropPuck(const SbVec2f &w, const SbTime &t) : Command(MSG_DROP_PUCK)
	{ where = w, timestamp = t; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the player to the server when the gun position changes
//////////////////////////////////////////////////////////////////////////////
class Gun : public Command {
  public:
    virtual int	    size() const { return sizeof(Gun); }
    
    int	    playerId;
    SbVec3f translation;
    float   rotAngle;
    
    Gun(int id, const SbVec3f &t, float a) : Command(MSG_GUN_MOVED)
	{ playerId = id, translation = t, rotAngle = a; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the server to a player when the other player's gun moves
//////////////////////////////////////////////////////////////////////////////
class OtherGun : public Command {
  public:
    virtual int	    size() const { return sizeof(OtherGun); }
    
    SbVec3f translation;
    float   rotAngle;
    
    OtherGun(const SbVec3f &t, float a) : Command(MSG_MOVE_OTHER_GUN)
	{ translation = t, rotAngle = a; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from the server to the player to move a pillar up
//////////////////////////////////////////////////////////////////////////////
class PillarUp : public Command {
  public:
    virtual int	    size() const { return sizeof(PillarUp); }
    
    int    whichPillar;
    SbTime downTime;
    
    PillarUp(int p, const SbTime &t) : Command(MSG_PILLAR_UP)
	{ whichPillar = p, downTime = t; }
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from player to server when a shot is fired (MSG_SHOTFIRED)
//////////////////////////////////////////////////////////////////////////////
class Shot : public Command {
  public:
    virtual int	    size() const { return sizeof(Shot); }
    
    // A shot comes from some location, and is oriented along some direction.
    // And it occurred at some time, namely the timestamp of the last render.
    SbVec2f from;
    SbVec2f dir;
    int playerId;
    SbTime timestamp;
    
    Shot(int id, float x, float y, float dx, float dy, const SbTime &timestamp);
};

enum CollisionType {
    COLLISION_NONE, 
    COLLISION_SHOT_HIT_PUCK, 
    COLLISION_PUCK_HIT_WALL, 
    COLLISION_PUCK_HIT_GOALPOST, 
    COLLISION_PUCK_HIT_PILLAR, 
    COLLISION_PUCK_SCORES_GOAL 
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from server to player when there is a collision
//////////////////////////////////////////////////////////////////////////////
class CollisionState : public Command {
  public:
    virtual int	    size() const { return sizeof(CollisionState); }
    
    CollisionType   type;	// type of collision
    SbVec2f	    loc;	// location of collision
    SbVec2f	    norm;	// normal at collision (only for puck hit rink)

    CollisionState() : Command(MSG_COLLISION) {}
};

//////////////////////////////////////////////////////////////////////////////
// This is sent from server to player when the render state
// changes (MSG_UPDATESTATE)
//////////////////////////////////////////////////////////////////////////////
class RenderState : public Command {
  public:
    virtual int	    size() const { return sizeof(RenderState); }
    
    int	    numShots[2];	    // number of shots for player 0 and 1
    SbVec2f shotLoc[2][MAXSHOTS];   // locations of shots for player 0 and 1
    SbVec2f shotDir[2][MAXSHOTS];   // directions of shots for player 0 and 1
    SbVec2f puckLoc;		    // locations of puck
    SbTime  timestamp;		    // timestamp of this state
    
    RenderState() : Command(MSG_UPDATESTATE) {}
};

#endif
