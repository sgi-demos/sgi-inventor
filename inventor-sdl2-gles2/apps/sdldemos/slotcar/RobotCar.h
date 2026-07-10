//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.4 $
 |
 |   Description:
 |      This file contains the definition of the RobotCar class.
 |
 |   To Do:
 |      It would be nice if the robot cars adjusted their skill to the
 |      level of the player racing them.  This would be fairly easy--
 |      just add routines increase/decreaseSkill that could be called
 |      by the main simulation routine when the player either beats or
 |      is beaten by a robot car.
 |
 |   Classes:
 |      RobotCar
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _ROBOT_CAR_
#define  _ROBOT_CAR_

#include "Car.h"
class Track;
class SoSeparator;
class SoTransform;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: RobotCar
//
//  This class defines a robot car used in the slotcar game.  Robot
//  cars are really dumb...
//
//////////////////////////////////////////////////////////////////////////////

class GameState;

class RobotCar : public Car {
 public:
    RobotCar();				// Local robot
    RobotCar(const CarPlayer &);	// Network robot
    ~RobotCar();

    static void	SimulateRobots(GameState *state, const SbTime &);

    SoSeparator *getCarRoot();
    
    void 	setScreechAmount(float amt, float curvature);

    // Add and remove annotation above the car
    virtual void        addAnnotation();
    virtual void        removeAnnotation();

  protected:
    virtual void	positionCar(const WorldPosition &position);
    
 private:
    void buildCar();

    void simulate(GameState *, const SbTime &);
    SbBool changeLanesSafely(GameState *, const SbTime &, int);
    SbBool carAhead(GameState *, const SbTime &,
		    const TrackPosition &, float);

    float skill;  // Fraction, 0.0 to 1.0, of max track velocity
    float maxV;   // Maximum velocity on straight

    SoSeparator *carGraph;
    SoSeparator *carGeom;
    SoTransform *screechTransform;
    SoSeparator *annotation;

    static SoSeparator *sharedGeom;
    static SbPList roboList;
    static int robotCarNum;
};

#endif /* _ROBOT_CAR_ */
