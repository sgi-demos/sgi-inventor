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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes:
 |      Defines classes for creating motors to drive linkages.
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodekits/SoAppearanceKit.h>

#include <Inventor/SoDB.h>

#include <Inventor/fields/SoMFTime.h>

#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/draggers/SoRotateDiscDragger.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/errors/SoDebugError.h>

#include "Motors.h"
#include "Buttons.h"
#include "Axle.h"
#include "LinkEngines.h"

//////////////////////////////////////////////////////////////////////////////
//  Class: MotorBase
//
//  Adds:         - on fields (inherits origin field)
//                - "originTranslator" part
//                - "onOffButton" and  "body" parts
//
//////////////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(MotorBase);

void
MotorBase::initClass()
{
    SO_KIT_INIT_CLASS(MotorBase, MechPiece, "MechPiece" );
}

MotorBase::MotorBase()
{
    SO_KIT_CONSTRUCTOR(MotorBase);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(originTranslator, SoTranslation, FALSE,
				topSeparator, geomSeparator, FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(onOffButton, ToggleButton, TRUE,
				geomSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(body, SoSeparator, TRUE,
				geomSeparator, , TRUE );

    // Add the new fields.
    SO_KIT_ADD_FIELD(on,       (0));

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( originTranslator);
    makeNodeFieldStopNotifying( onOffButton);
    makeNodeFieldStopNotifying( body);

    // Create the parts.
    getAnyPart("onOffButton", TRUE);
    setPartAsDefault("body", "motorBaseBody" );

    setUpConnections( TRUE, TRUE );
}

void 
MotorBase::setZPlane( float newZ )
{
    MechPiece::setZPlane( newZ );

    SoTranslation *xl = (SoTranslation *) getAnyPart("originTranslator", TRUE);
    if (xl && xl->translation.getValue()[2] != newZ )
	xl->translation  = origin.getValue();
}

SbBool
MotorBase::undoConnections(SbBool ) 
{
    // Disconnect fields and engines
    SoTranslation *xl = SO_GET_ANY_PART(this,"originTranslator", SoTranslation);
    disconnectField( xl->translation );

    ToggleButton *but = SO_GET_ANY_PART(this,"onOffButton", ToggleButton);
    disconnectField( but->isOn );

    // We disable base class LAST.
    MechPiece::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
MotorBase::setUpConnections(SbBool onOff, SbBool doItAlways ) 
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    // We connect AFTER base class.
    MechPiece::setUpConnections( onOff, FALSE );

    // Set up the default geometries.
    ToggleButton *but = (ToggleButton *) onOffButton.getValue();
    if (but) {
	but->setPartAsDefault("buttonGeom", "motorBaseButtonGeom");
	but->setPartAsDefault("buttonActiveGeom", "motorBaseButtonActiveGeom");
        connectFromField( &on, &but->isOn);
    }

    // Connect fields and engines
    SoTranslation *xl = SO_GET_ANY_PART(this,"originTranslator", SoTranslation);
    connectFromField( &origin, &xl->translation);

    return !(connectionsSetUp = onOff);
}

void
MotorBase::setDefaultOnNonWritingFields()
{
    body.setDefault(TRUE);
    originTranslator.setDefault(TRUE);

    MechPiece::setDefaultOnNonWritingFields();
}

MotorBase::~MotorBase()
{
}

SbPList *
MotorBase::getAxlePtrs()
{
    return MechPiece::getAxlePtrs();
}


////////////////////////////////////////////////////////////////////
//    Class: RotorMotor - adds "axle" and "speedStick" parts 
//                      - adds an engine to drive axle rotation.
//    
////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(RotorMotor);

void
RotorMotor::initClass()
{
    SO_KIT_INIT_CLASS(RotorMotor, MotorBase, "MotorBase" );
}

RotorMotor::RotorMotor()
{
    SO_KIT_CONSTRUCTOR(RotorMotor);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(axle, Axle, TRUE,
				topSeparator, originTranslator, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(speedStick, SoRotateDiscDragger, TRUE,
				geomSeparator, , TRUE );

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( axle);
    makeNodeFieldStopNotifying( speedStick);

    // Create the parts.
    getAnyPart("axle", TRUE);
    getAnyPart("speedStick", TRUE);
    set("speedStick", "rotation 0 0 1 1");
    setPartAsDefault("body", "rotorMotorBody");

    // This engine is necessary only because of a bug in SoElapsedTime,
    // which does not enable/disable properly when the on field changes.
    // Workaround:
    // The on/off button and the realTime file are inputs to the gate.
    // When the on/off button is on, the gate is enabled and the 
    // realTime flows out to the elapsedTime engine.
    gate = new SoGate( SoMFTime::getClassTypeId() );
    gate->ref();

    // The time engine will be used to control the angle of rotation
    // of the axle.
    time = new SoElapsedTime;
    time->ref();

    // This engine extracts the zrotation from an SbRotation.
    // The angle will be used to control the speed input to the time engine.
    speedEngine = new ZAngleFromRotationEngine;
    speedEngine->ref();

    setUpConnections( TRUE, TRUE );
}

SbBool
RotorMotor::undoConnections(SbBool ) 
{
    // Disconnect fields and engines
    disconnectField( gate->enable );
    disconnectField( *gate->input );

    disconnectField( time->timeIn );
    disconnectField( time->speed );

    disconnectField( speedEngine->inRotation );

    Axle *ax = (Axle *) axle.getValue();
    if (ax) {
	disconnectField( ax->zRotation );
    }
    disconnectField( origin );

    // Turn off draggers

    // We disable base class LAST.
    MotorBase::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
RotorMotor::setUpConnections(SbBool onOff, SbBool doItAlways ) 
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    // We connect AFTER base class.
    MotorBase::setUpConnections( onOff, FALSE );

    // Set up the default geometries.
    ToggleButton *but = (ToggleButton *) onOffButton.getValue();
    if (but) {
	but->setPartAsDefault("buttonGeom", "rotorMotorButtonGeom");
	but->setPartAsDefault("buttonActiveGeom", "rotorMotorButtonActiveGeom");
	but->setPartAsDefault("buttonText", "rotorMotorButtonText");
	but->setPartAsDefault("buttonActiveText", "rotorMotorButtonActiveText");
    }
    SoRotateDiscDragger *stick 
	= SO_GET_ANY_PART(this,"speedStick",SoRotateDiscDragger );
    stick->setPartAsDefault("rotator", "rotorMotorSpeedStickGeom");
    stick->setPartAsDefault("rotatorActive", "rotorMotorSpeedStickGeomActive");

    // Connect fields and engines
	// The gate:
	connectFromField( SoDB::getGlobalField("realTime"), gate->input );
	connectFromField( &but->isOn, &gate->enable );

	// The speed:
        connectFromField( &stick->rotation, &speedEngine->inRotation );

	// The elapsed time:
        connectFromEngine( gate->output, &time->timeIn );
        connectFromEngine( &speedEngine->outAngle, &time->speed);

    Axle *ax = (Axle *) axle.getValue();
    if (ax) {
        connectFromEngine( &time->timeOut, &ax->zRotation);
	ax->canFollowRot = FALSE;
	ax->origin = origin.getValue();
	connectFromField( &ax->origin, &origin);
    }

    return !(connectionsSetUp = onOff);
}

void
RotorMotor::setDefaultOnNonWritingFields()
{

    MotorBase::setDefaultOnNonWritingFields();
}

RotorMotor::~RotorMotor()
{
    if (gate)
	gate->unref();
    if (time)
	time->unref();
    if (speedEngine)
	speedEngine->unref();
}

SbPList *
RotorMotor::getAxlePtrs()
{
    Axle *ax = SO_GET_ANY_PART( this, "axle", Axle );

    SbPList *ap = MotorBase::getAxlePtrs();
    if (ap->getLength() != 1 || ap->find(ax) == -1) {
	ap->truncate(0);
	ap->append( ax);
    }

    return ap;
}

