//  -*- C++ -*-

/*
 * Copyright 1994   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.15 $
 |
 |   Description:
 |      Defines the packet classes for sending data across the network.
 |
 |      IMPORTANT NOTE:  THESE CLASSES MUST NOT HAVE VIRTUAL METHODS!
 |
 |   Author(s): David Mott, Helga Thorvaldsdottir, Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _PACKET_
#define _PACKET_

#include <Inventor/SbTime.h>
#include "Network.h"
#include "Car.h"

// Messages
#define MSG_CARINFO		1
#define MSG_CARCRASH		2
#define MSG_CARCOLLISION	3
#define MSG_RACESTARTING	4
#define MSG_CARFINISH		5
#define MSG_CARJOIN		6
#define MSG_CAREXIT		7
#define MSG_CARQUERY		8
#define MSG_STARTQUERY		9
#define MSG_FINISHQUERY		10

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Command
//
//  The code assumes all messages sent and received are ultimately derived
//  from this Command class.  This way, received messages can perform a switch
//  on the command found here.
//
//////////////////////////////////////////////////////////////////////////////
class Command : public NetworkMsg {
  public:
    Command(unsigned char cmd)	{ command = cmd; }
    unsigned char	getCommand() const { return command; }

  private:
    // The command, i.e. MSG_CARINFO
    unsigned char	command;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: CarInfo
//
//  This subclass of Command stores car position information
//
//////////////////////////////////////////////////////////////////////////////
class CarInfo : public Command {
  public:
    CarInfo(unsigned char id, signed char targetL,
	    const TrackPosition &pos, const TrackPosition &vel,
	    float _screech,
	    const SbTime &tStamp) : Command(MSG_CARINFO)
	{ carId = id; targetLane = targetL;
	  position = pos; velocity = vel; screech = _screech;
	  timeStamp = tStamp;
	}

    CarPlayer		getPlayer() const
	{ return CarPlayer(getFromAddress(), carId); }
    const TrackPosition &getTrackPosition() const { return position; }
    const TrackPosition &getVelocity() const { return velocity; }
    float getScreech() const { return screech; }
    signed char getTargetLane() const { return targetLane; }
    const SbTime &getTimeStamp() const { return timeStamp; }

  private:
    unsigned char	carId;
    signed char		targetLane;
    TrackPosition    	position;
    TrackPosition	velocity;
    float		screech;
    SbTime		timeStamp;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: CarCrash
//
//  This subclass of Command denotes a car crash
//
//////////////////////////////////////////////////////////////////////////////
class CarCrash : public Command {
  public:
    CarCrash(unsigned char id, const SbTime &t, const TrackPosition &p)
	: Command(MSG_CARCRASH) { carId = id; timeStamp = t;
				  position = p; }
    CarPlayer		getPlayer() const
	{ return CarPlayer(getFromAddress(), carId); }

    const SbTime &getTimeStamp() const { return timeStamp; }
    const TrackPosition &getTrackPosition() const { return position; }

  private:
    unsigned char	carId;
    SbTime		timeStamp;
    TrackPosition    	position;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: RaceStartInfo
//
//  This subclass of Command denotes a race is about to start
//
//////////////////////////////////////////////////////////////////////////////
class RaceStartInfo : public Command {
  public:
    RaceStartInfo(SbTime t) : Command(MSG_RACESTARTING)
	{ raceStartTime = t; }

    const SbTime &getStartTime() const { return raceStartTime; }

  private:
    SbTime		raceStartTime;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: CarCollision
//
//  This subclass of Command denotes a car collision
//
//////////////////////////////////////////////////////////////////////////////
class CarCollision : public Command {
  public:
    CarCollision(const CarPlayer &_player,
		 const SbTime &t, const TrackPosition &p,
		 float s)
	: Command(MSG_CARCOLLISION), player(_player) {
	    timeStamp = t;
	    position = p;
	    sideSpeed = s;
	    player = _player;
	}

    const SbTime &getTimeStamp() const { return timeStamp; }
    const TrackPosition &getTrackPosition() const { return position; }
    float getSideSpeed() const { return sideSpeed; }
    const CarPlayer &getPlayer() const { return player; }

  private:
    SbTime		timeStamp;
    TrackPosition    	position;
    float		sideSpeed;
    CarPlayer    	player;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: CarJoin
//
//  This subclass of Command denotes a car joining the game
//
//////////////////////////////////////////////////////////////////////////////
class CarJoin : public Command {
  public:
    CarJoin(unsigned char id, const char *_name, int _pointTotal,
			int _numGames) 
		: Command(MSG_CARJOIN)
    	{
	    carId = id;
            strncpy(name, _name, 15);
            pointTotal = (short)_pointTotal;
            numGames = (short)_numGames;
	 }
    int 		getPointTotal() const { return (int)pointTotal; }
    int			getNumGames() const { return (int)numGames; }
    CarPlayer		getPlayer() const
	{ return CarPlayer(getFromAddress(), carId); }

    const char *	getName() const	{ return name; }

  private:
    unsigned char	carId;
    char		name[16];
    short		pointTotal;
    short		numGames;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: CarExit
//
//  This subclass of Command denotes a car leaving the game
//
//////////////////////////////////////////////////////////////////////////////
class CarExit : public Command {
  public:
    CarExit(unsigned char id) : Command(MSG_CAREXIT) { carId = id; }
    CarPlayer		getPlayer() const
	{ return CarPlayer(getFromAddress(), carId); }

  private:
    unsigned char	carId;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: CarQuery
//
//  This subclass of Command denotes a car asking about another car
//
//////////////////////////////////////////////////////////////////////////////
class CarQuery : public Command {
  public:
    CarQuery(const CarPlayer &_player) :
	Command(MSG_CARQUERY), player(_player) {}

    const CarPlayer &	getPlayer() const	{ return player; }

  private:
    CarPlayer		player;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: StartQuery
//
//  This subclass of Command denotes a car asking about the race start
//
//////////////////////////////////////////////////////////////////////////////
class StartQuery : public Command {
  public:
    StartQuery() : Command(MSG_STARTQUERY) {}
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: FinishQuery
//
//  This subclass of Command denotes a car asking about the race finish
//
//////////////////////////////////////////////////////////////////////////////
class FinishQuery : public Command {
  public:
    FinishQuery(const CarPlayer &_player) :
	Command(MSG_FINISHQUERY), player(_player) {}

    const CarPlayer &	getPlayer() const	{ return player; }

  private:
    CarPlayer		player;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: CarFinish
//
//  This subclass of Command denotes a car crossing the finish line
//
//////////////////////////////////////////////////////////////////////////////
class CarFinish : public Command {
  public:
    CarFinish(unsigned char id, const SbTime &t) 
	: Command(MSG_CARFINISH) { carId = id; timeStamp = t; }
    CarPlayer		getPlayer() const
	{ return CarPlayer(getFromAddress(), carId); }
    const SbTime &getTimeStamp() const { return timeStamp; }

  private:
    unsigned char	carId;
    SbTime		timeStamp;
};

#endif /* _PACKET_ */
