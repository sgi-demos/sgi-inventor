//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |      Defines these classes for creating motors to drive linkages.
 |
 |      MotorBase  - on, and speed fields (inherits origin field)
 |                 - "originTranslator" part
 |                 - "onOffButton" and  "body" parts
 |
 |      RotorMotor - adds "axle" and "speedStick" parts
 |                 - adds an engine to drive axle rotation.
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _MOTOR_CLASSES_
#define  _MOTOR_CLASSES_

#include <Inventor/nodekits/SoInteractionKit.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/engines/SoGate.h>

#include "MechPiece.h"
#include "LinkEngines.h"

//////////////////////////////////////////////////////////////////////////////
//  Class: MotorBase
//
//  Adds:         - on, and speed fields (inherits origin field)
//                - "originTranslator" part
//                - "onOffButton" and  "body" parts
//
//////////////////////////////////////////////////////////////////////////////

class MotorBase : public MechPiece {

    SO_KIT_HEADER(MotorBase);

    SO_KIT_CATALOG_ENTRY_HEADER(originTranslator);
    SO_KIT_CATALOG_ENTRY_HEADER(onOffButton);
    SO_KIT_CATALOG_ENTRY_HEADER(body);

  public:
    // Fields
    SoSFBool  on;

    // Constructor
    MotorBase();
    
    virtual SbPList *getAxlePtrs();
    virtual void setZPlane( float newZ );

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void   setDefaultOnNonWritingFields();

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

  private:
    // Destructor
    ~MotorBase();
};

////////////////////////////////////////////////////////////////////
//    Class: RotorMotor - adds "axle" and "speedStick" parts 
//                      - adds an engine to drive axle rotation.
//    
////////////////////////////////////////////////////////////////////
class RotorMotor : public MotorBase {

    SO_KIT_HEADER(RotorMotor);

    SO_KIT_CATALOG_ENTRY_HEADER(axle);
    SO_KIT_CATALOG_ENTRY_HEADER(speedStick);

  public:

    virtual SbPList *getAxlePtrs();

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // constructor
    RotorMotor();

  SoINTERNAL public:
    static void initClass();

  protected:

    SoElapsedTime *time;

    // There is a bug in SoElapsedTime.
    // It never gets properly disabled when turned off. So we'll pass the
    // timeIn through a gate, and enable/disable the gate with the
    // motor's on/off button.
    SoGate        *gate;

    ZAngleFromRotationEngine *speedEngine;
    
  private:

    // destructor
    virtual ~RotorMotor();
};
#endif /* _MOTOR_CLASSES_ */

