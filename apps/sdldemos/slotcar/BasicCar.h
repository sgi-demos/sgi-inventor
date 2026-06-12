//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Description:
 |      This file contains the definition of the BasicCar class.
 |
 |   Classes:
 |      BasicCar
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _BASIC_CAR_
#define  _BASIC_CAR_

#include "Car.h"
class Track;
class SoTransform;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: BasicCar
//
//  This class defines a basic car used in the slotcar game.
//
//////////////////////////////////////////////////////////////////////////////

class BasicCar : public Car {
 public:
    BasicCar();
    ~BasicCar();

    virtual SoSeparator *getCar();
    
    SoTransform *getTransform() { return transform; }

  protected:
    virtual void	positionCar(const WorldPosition &position);
    
 private:
    virtual void buildCar();

    SoTransform *transform;
    SoSeparator *carGraph;
};

#endif /* _BASIC_CAR_ */
