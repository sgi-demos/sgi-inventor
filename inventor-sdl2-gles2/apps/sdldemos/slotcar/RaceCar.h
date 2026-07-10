//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.8 $
 |
 |   Description:
 |      This file contains the definition of the RaceCar class.
 |
 |   Classes:
 |      RaceCar
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _RACE_CAR_
#define  _RACE_CAR_

#include "Car.h"
class Track;
class SoSeparator;
class SoTransform;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: RaceCar
//
//  This class defines a race car used in the slotcar game.  This class
//  defines 1 of 8 possible cars.  
//
//////////////////////////////////////////////////////////////////////////////

class RaceCar : public Car {
 public:
    RaceCar(const CarPlayer &, const SbString &name, SbBool _controlled);
    ~RaceCar();

    SoSeparator *getCarRoot();
    
    void 	setScreechAmount(float amt, float curvature);

    // Add and remove annotation above the car
    virtual void        addAnnotation();
    virtual void        removeAnnotation();

  protected:
    virtual void	positionCar(const WorldPosition &position);
    
 private:
    void buildCar(SoSeparator *geom);

    SoSeparator *carGraph;
    SoSeparator *carGeom;
    SoTransform *screechTransform;
    SoSeparator *annotation;
};

#endif /* _RACE_CAR_ */
