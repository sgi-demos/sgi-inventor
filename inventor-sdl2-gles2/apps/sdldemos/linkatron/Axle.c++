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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Classes:
 |	Axle, AxleShaft, AxleLeg, AxlePin
 |
 |	The Axle class is an interactively moveable axle.
 |	It contains instances of three other new classes,
 |      AxleShaft, AxleLeg, and AxlePin.
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSwitch.h>

#include "Axle.h"
#include "TableTop.h"
#include "WorldInfo.h"
#include "Sound.h"

//////////////////////////////////////////////////////////////////////
//  Class: AxleShaft
//////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(AxleShaft);

void
AxleShaft::initClass()
{
    SO_KIT_INIT_CLASS(AxleShaft, SoTranslate2Dragger, "Translate2Dragger");
}

AxleShaft::AxleShaft()
{
    SO_KIT_CONSTRUCTOR(AxleShaft);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(zRotator, SoRotationXYZ, TRUE,
			     topSeparator, geomSeparator, FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(pickStyle, SoPickStyle, FALSE,
			     geomSeparator, translatorSwitch, FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(translatorInert, SoSeparator, TRUE,
			     translatorSwitch, , TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(zBoundsOkSwitch, SoSwitch, TRUE,
			     geomSeparator, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zBoundsSep, SoSeparator, TRUE,
			     zBoundsOkSwitch, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zBoundsXf, SoTransform, TRUE,
			     zBoundsSep, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zScaledTranslatorSwitch, SoSwitch, TRUE,
			     zBoundsSep, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zScaledTranslator, SoSeparator, TRUE,
			     zScaledTranslatorSwitch, , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(zScaledTranslatorActive, SoSeparator, TRUE,
			     zScaledTranslatorSwitch, , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(zScaledTranslatorInert, SoSeparator, TRUE,
			     zScaledTranslatorSwitch, , TRUE);

    SO_KIT_ADD_FIELD(zRotation, (0.0));
    SO_KIT_ADD_FIELD(zMin,      (-.2));
    SO_KIT_ADD_FIELD(zMax,      ( .2));

    SO_KIT_INIT_INSTANCE();

    // From SoInteractionKit class
    MechPiece::makeNodeFieldStopNotifying( topSeparator);
    MechPiece::makeNodeFieldStopNotifying( geomSeparator);
    // From SoDragger class
    MechPiece::makeNodeFieldStopNotifying( motionMatrix);
    // From SoTranslate2Dragger class
    MechPiece::makeNodeFieldStopNotifying( translatorSwitch);
    MechPiece::makeNodeFieldStopNotifying( translator );
    MechPiece::makeNodeFieldStopNotifying( translatorActive );
    MechPiece::makeNodeFieldStopNotifying( feedbackSwitch);
    MechPiece::makeNodeFieldStopNotifying( feedback );
    MechPiece::makeNodeFieldStopNotifying( feedbackActive );
    MechPiece::makeNodeFieldStopNotifying( axisFeedbackSwitch );
    MechPiece::makeNodeFieldStopNotifying( xAxisFeedback );
    MechPiece::makeNodeFieldStopNotifying( yAxisFeedback );
    // From this class
    MechPiece::makeNodeFieldStopNotifying( zRotator);
    MechPiece::makeNodeFieldStopNotifying( pickStyle);
    MechPiece::makeNodeFieldStopNotifying( translatorInert);
    MechPiece::makeNodeFieldStopNotifying( zBoundsOkSwitch);
    MechPiece::makeNodeFieldStopNotifying( zBoundsSep);
    MechPiece::makeNodeFieldStopNotifying( zBoundsXf);
    MechPiece::makeNodeFieldStopNotifying( zScaledTranslatorSwitch);
    MechPiece::makeNodeFieldStopNotifying( zScaledTranslator);
    MechPiece::makeNodeFieldStopNotifying( zScaledTranslatorActive);
    MechPiece::makeNodeFieldStopNotifying( zScaledTranslatorInert);

    SoSeparator *sep;
    sep = SO_GET_ANY_PART( this, "topSeparator", SoSeparator);
    sep->renderCaching = OFF;
    sep->boundingBoxCaching = OFF;
    sep->renderCulling= OFF;
    sep->pickCulling = OFF;

    // This engine will set the zBoundsXf to scale and translate a cube of
    // depth 2 between the zMin and zMax field values (relative to origin).
    encloseEngine = new EncloseEngine;
    encloseEngine->ref();
    encloseEngine->xEnabled = FALSE;
    encloseEngine->yEnabled = FALSE;

    // Set up the default geometries
	setPartAsDefault("translator",       "axleShaftTranslator");
	setPartAsDefault("translatorActive", "axleShaftTranslatorActive");
	setPartAsDefault("translatorInert", "axleShaftTranslatorInert");
	setPartAsDefault("zScaledTranslator",       
			 "axleShaftZScaledTranslator");
	setPartAsDefault("zScaledTranslatorActive", 
			 "axleShaftZScaledTranslatorActive");
	setPartAsDefault("zScaledTranslatorInert", 
			 "axleShaftZScaledTranslatorInert");
	setPartAsDefault("feedback", "axleShaftFeedback");
	setPartAsDefault("feedbackActive", "axleShaftFeedbackActive");
	setPartAsDefault("xAxisFeedback", "axleShaftXAxisFeedback");
	setPartAsDefault("yAxisFeedback", "axleShaftYAxisFeedback");

    // Initialize switch nodes.
    setSwitchValue( zBoundsOkSwitch.getValue(), SO_SWITCH_ALL );
    setSwitchValue( zScaledTranslatorSwitch.getValue(), 0 );

    setUpConnections( TRUE, TRUE );

}

SbBool
AxleShaft::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {
	// We connect AFTER base class.
	SoTranslate2Dragger::setUpConnections( onOff, FALSE );

	// Replace default geometries of sub-parts if desired.

	// Connect fields and engines
	    // (zRotator->angle) <== (zRotation field)
	    SoRotationXYZ *rot = (SoRotationXYZ *) getAnyPart("zRotator",TRUE);
	    if (rot->axis.getValue() != SoRotationXYZ::Z )
		rot->axis.setValue(SoRotationXYZ::Z);
	    MechPiece::connectFromField( &zRotation, &rot->angle );

	    // Hookup the encloseEngine 
	    MechPiece::connectFromField( &zMin, &encloseEngine->zMin );
	    MechPiece::connectFromField( &zMax, &encloseEngine->zMax );
	    SoTransform *zXf = (SoTransform *) getAnyPart("zBoundsXf",TRUE);
	    MechPiece::connectFromEngine( &encloseEngine->translation,
	    				  &zXf->translation );
	    MechPiece::connectFromEngine( &encloseEngine->scaleFactor,
	    				  &zXf->scaleFactor );
	    SoSwitch *zOk = (SoSwitch *) getAnyPart("zBoundsOkSwitch",TRUE);
	    MechPiece::connectFromEngine( &encloseEngine->switcher,
	    				  &zOk->whichChild );

	// This insures internal consistency
	    setDraggable( isDraggable() );

	// Call the sensor CBs to make things are up-to-date.

	// Add callbacks
	addStartCallback( &AxleShaft::startCB );
	addFinishCallback( &AxleShaft::finishCB );

	// We OWN these nodes, and they are private. We can turn
	// notification off since only we will change them. We do this
	// to save time on notification.
	SoNode *myN;
	if (myN = zRotator.getValue())        myN->enableNotify(FALSE);
    }
    else {
	// We disconnect BEFORE base class.

	// Remove callbacks
	removeStartCallback( &AxleShaft::startCB );
	removeFinishCallback( &AxleShaft::finishCB );

	// Disconnect fields and engines
	    // (zRotator->angle) <== (zRotation field)
	    SoRotationXYZ *rot = (SoRotationXYZ *) getAnyPart("zRotator",TRUE);
	    MechPiece::disconnectField( rot->angle );

	    // Unhook the encloseEngine
	    MechPiece::disconnectField( encloseEngine->zMin );
	    MechPiece::disconnectField( encloseEngine->zMax );
	    SoTransform *zXf = (SoTransform *) getAnyPart("zBoundsXf",TRUE);
	    MechPiece::disconnectField( zXf->translation );
	    MechPiece::disconnectField( zXf->scaleFactor );
	    SoSwitch *zOk = (SoSwitch *) getAnyPart("zBoundsOkSwitch",TRUE);
	    MechPiece::disconnectField( zOk->whichChild );

	SoTranslate2Dragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

void 
AxleShaft::startCB( void *, SoDragger *inDragger )
{
    // When dragging begins, show active version of zScaledTranslator
    AxleShaft *ax = (AxleShaft *) inDragger;
    ax->setSwitchValue( ax->zScaledTranslatorSwitch.getValue(), 1 );

    if (ax->translation.isConnected() && ax->translation.isConnectionEnabled())
	ax->translation.enableConnection(FALSE);
}
void 
AxleShaft::finishCB( void *, SoDragger *inDragger )
{
    // When dragging ends, show inactive version of zScaledTranslator
    AxleShaft *ax = (AxleShaft *) inDragger;
    ax->setSwitchValue( ax->zScaledTranslatorSwitch.getValue(), 0 );
    
    if (ax->translation.isConnected() && !ax->translation.isConnectionEnabled())
	ax->translation.enableConnection(TRUE);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
AxleShaft::~AxleShaft()
//
////////////////////////////////////////////////////////////////////////
{
    if (encloseEngine)
	encloseEngine->unref();
}

SbBool
AxleShaft::isDraggable()
{
    SoPickStyle *ps = (SoPickStyle *) pickStyle.getValue();
    if (ps && ps->style.getValue() == SoPickStyle::UNPICKABLE)
	return FALSE;
    else
	return TRUE;
}

void
AxleShaft::setDraggable( SbBool onOff )
{
    // Set the pickStyle accordingly:
	SoPickStyle *ps = (SoPickStyle *) pickStyle.getValue();
	if (ps==NULL) ps = SO_GET_ANY_PART(this,"pickStyle",SoPickStyle);
	if ( onOff && ps->style.getValue() != SoPickStyle::SHAPE)
	    ps->style = SoPickStyle::SHAPE;
	else if ( ! onOff && ps->style.getValue() != SoPickStyle::UNPICKABLE )
	    ps->style = SoPickStyle::UNPICKABLE;

    // Set the translatorSwitch:
	if (onOff) setSwitchValue( translatorSwitch.getValue(), 0);
	else       setSwitchValue( translatorSwitch.getValue(), 2);

    // Set the zScaledTranslatorSwitch:
	if (onOff) setSwitchValue( zScaledTranslatorSwitch.getValue(), 0);
	else       setSwitchValue( zScaledTranslatorSwitch.getValue(), 2);
}

void
AxleShaft::setDefaultOnNonWritingFields()
{
    zRotator.setDefault(TRUE);
    pickStyle.setDefault(TRUE);
    zBoundsOkSwitch.setDefault(TRUE);
    zBoundsSep.setDefault(TRUE);
    zBoundsXf.setDefault(TRUE);
    zScaledTranslatorSwitch.setDefault(TRUE);

    SoTranslate2Dragger::setDefaultOnNonWritingFields();
}

//////////////////////////////////////////////////////////////////////
//  Class: AxleLeg
//////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(AxleLeg);

void
AxleLeg::initClass()
{
    SO_KIT_INIT_CLASS(AxleLeg, SoTranslate1Dragger, "Translate1Dragger");
}

AxleLeg::AxleLeg()
{
    SO_KIT_CONSTRUCTOR(AxleLeg);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(pickStyle, SoPickStyle, FALSE,
			     geomSeparator, translatorSwitch, FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(translatorInert, SoSeparator, TRUE,
			     translatorSwitch, , TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(yBoundsOkSwitch, SoSwitch, TRUE,
			     geomSeparator, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yBoundsSep, SoSeparator, TRUE,
			     yBoundsOkSwitch, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yBoundsXf, SoTransform, TRUE,
			     yBoundsSep, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yScaledTranslatorSwitch, SoSwitch, TRUE,
			     yBoundsSep, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yScaledTranslator, SoSeparator, TRUE,
			     yScaledTranslatorSwitch, , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(yScaledTranslatorActive, SoSeparator, TRUE,
			     yScaledTranslatorSwitch, , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(yScaledTranslatorInert, SoSeparator, TRUE,
			     yScaledTranslatorSwitch, , TRUE);

    SO_KIT_ADD_FIELD(yMin,      (-1));

    SO_KIT_INIT_INSTANCE();

    // From SoInteractionKit class
    MechPiece::makeNodeFieldStopNotifying( topSeparator);
    MechPiece::makeNodeFieldStopNotifying( geomSeparator);
    // From SoDragger class
    MechPiece::makeNodeFieldStopNotifying( motionMatrix);
    // From SoTranslate2Dragger class
    MechPiece::makeNodeFieldStopNotifying( translatorSwitch);
    MechPiece::makeNodeFieldStopNotifying( translator );
    MechPiece::makeNodeFieldStopNotifying( translatorActive );
    MechPiece::makeNodeFieldStopNotifying( feedbackSwitch);
    MechPiece::makeNodeFieldStopNotifying( feedback );
    MechPiece::makeNodeFieldStopNotifying( feedbackActive );
    // From this class
    MechPiece::makeNodeFieldStopNotifying( pickStyle);
    MechPiece::makeNodeFieldStopNotifying( translatorInert);
    MechPiece::makeNodeFieldStopNotifying( yBoundsOkSwitch);
    MechPiece::makeNodeFieldStopNotifying( yBoundsSep);
    MechPiece::makeNodeFieldStopNotifying( yBoundsXf);
    MechPiece::makeNodeFieldStopNotifying( yScaledTranslatorSwitch);
    MechPiece::makeNodeFieldStopNotifying( yScaledTranslator);
    MechPiece::makeNodeFieldStopNotifying( yScaledTranslatorActive);
    MechPiece::makeNodeFieldStopNotifying( yScaledTranslatorInert);

    SoSeparator *sep;
    sep = SO_GET_ANY_PART( this, "topSeparator", SoSeparator);
    sep->renderCaching = OFF;
    sep->boundingBoxCaching = OFF;
    sep->renderCulling= OFF;
    sep->pickCulling = OFF;

    // This engine extracts a value for the upper y value of the enclose
    // engine from the origin's y value.
    calcEngine = new SoCalculator;
    calcEngine->ref();

    // This engine will set the yBoundsXf to scale and translate a cube of
    // height 2 between the yMin and yMax field values.
    encloseEngine = new EncloseEngine;
    encloseEngine->ref();
    encloseEngine->xEnabled = FALSE;
    encloseEngine->zEnabled = FALSE;
    encloseEngine->relativeToMax = TRUE;

    // Set up the default geometries
	setPartAsDefault("translator",       "axleLegTranslator");
	setPartAsDefault("translatorActive", "axleLegTranslatorActive");
	setPartAsDefault("translatorInert", "axleLegTranslatorInert");
	setPartAsDefault("yScaledTranslator",       
			 "axleLegYScaledTranslator");
	setPartAsDefault("yScaledTranslatorActive", 
			 "axleLegYScaledTranslatorActive");
	setPartAsDefault("yScaledTranslatorInert", 
			 "axleLegYScaledTranslatorInert");
	setPartAsDefault("feedback", "axleLegFeedback");
	setPartAsDefault("feedbackActive", "axleLegFeedbackActive");

    // Initialize switch nodes.
    setSwitchValue( yBoundsOkSwitch.getValue(), SO_SWITCH_ALL );
    setSwitchValue( yScaledTranslatorSwitch.getValue(), 0 );

    setUpConnections( TRUE, TRUE );

}

SbBool
AxleLeg::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {
	// We connect AFTER base class.
	SoTranslate1Dragger::setUpConnections( onOff, FALSE );

	// Replace default geometries of sub-parts if desired.

	// Connect fields and engines
	    // Hookup the calcEngine
	    MechPiece::connectFromField( &translation, &calcEngine->A );
	    calcEngine->expression = "oa = A[1]";
	    // Hookup the encloseEngine 
	    MechPiece::connectFromField( &yMin, &encloseEngine->yMin );
	    MechPiece::connectFromEngine( &calcEngine->oa, 
					  &encloseEngine->yMax);
	    SoTransform *yXf = (SoTransform *) getAnyPart("yBoundsXf",TRUE);
	    MechPiece::connectFromEngine( &encloseEngine->translation,
	    				  &yXf->translation );
	    MechPiece::connectFromEngine( &encloseEngine->scaleFactor,
	    				  &yXf->scaleFactor );
	    SoSwitch *yOk = (SoSwitch *) getAnyPart("yBoundsOkSwitch",TRUE);
	    MechPiece::connectFromEngine( &encloseEngine->switcher,
	    				  &yOk->whichChild );

	// This insures internal consistency
	    setDraggable( isDraggable() );

	// Add callbacks
	addStartCallback( &AxleLeg::startCB );
	addFinishCallback( &AxleLeg::finishCB );

    }
    else {
	// We disconnect BEFORE base class.

	// Remove callbacks
	removeStartCallback( &AxleLeg::startCB );
	removeFinishCallback( &AxleLeg::finishCB );

	// Disconnect fields and engines
	    // Unhook the calcEngine
	    MechPiece::disconnectField( calcEngine->A );
	    // Unhook the encloseEngine
	    MechPiece::disconnectField( encloseEngine->yMin );
	    MechPiece::disconnectField( encloseEngine->yMax );
	    SoTransform *yXf = (SoTransform *) getAnyPart("yBoundsXf",TRUE);
	    MechPiece::disconnectField( yXf->translation );
	    MechPiece::disconnectField( yXf->scaleFactor );
	    SoSwitch *yOk = (SoSwitch *) getAnyPart("yBoundsOkSwitch",TRUE);
	    MechPiece::disconnectField( yOk->whichChild );

	SoTranslate1Dragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

void 
AxleLeg::startCB( void *, SoDragger *inDragger )
{
    // When dragging begins, show active version of zScaledTranslator
    AxleLeg *ax = (AxleLeg *) inDragger;
    ax->setSwitchValue( ax->yScaledTranslatorSwitch.getValue(), 1 );

    if (ax->translation.isConnected() && ax->translation.isConnectionEnabled())
	ax->translation.enableConnection(FALSE);
}
void 
AxleLeg::finishCB( void *, SoDragger *inDragger )
{
    // When dragging ends, show inactive version of zScaledTranslator
    AxleLeg *ax = (AxleLeg *) inDragger;
    ax->setSwitchValue( ax->yScaledTranslatorSwitch.getValue(), 0 );
    
    if (ax->translation.isConnected() && !ax->translation.isConnectionEnabled())
	ax->translation.enableConnection(TRUE);
}

////////////////////////////////////////////////////////////////////////
//    Destructor
//
AxleLeg::~AxleLeg()
{
    if (calcEngine)
	calcEngine->unref();
    if (encloseEngine)
	encloseEngine->unref();
}

SbBool
AxleLeg::isDraggable()
{
    SoPickStyle *ps = (SoPickStyle *) pickStyle.getValue();
    if (ps && ps->style.getValue() == SoPickStyle::UNPICKABLE)
	return FALSE;
    else
	return TRUE;
}

void
AxleLeg::setDraggable( SbBool onOff )
{
    // Set the pickStyle accordingly:
	SoPickStyle *ps = (SoPickStyle *) pickStyle.getValue();
	if (ps==NULL) ps = SO_GET_ANY_PART(this,"pickStyle",SoPickStyle);
	if ( onOff && ps->style.getValue() != SoPickStyle::SHAPE)
	    ps->style = SoPickStyle::SHAPE;
	else if ( ! onOff && ps->style.getValue() != SoPickStyle::UNPICKABLE )
	    ps->style = SoPickStyle::UNPICKABLE;

    // Set the translatorSwitch:
	if (onOff) setSwitchValue( translatorSwitch.getValue(), 0);
	else       setSwitchValue( translatorSwitch.getValue(), 2);

    // Set the yScaledTranslatorSwitch:
	if (onOff) setSwitchValue( yScaledTranslatorSwitch.getValue(), 0);
	else       setSwitchValue( yScaledTranslatorSwitch.getValue(), 2);
}

void
AxleLeg::setDefaultOnNonWritingFields()
{
    pickStyle.setDefault(TRUE);
    yBoundsOkSwitch.setDefault(TRUE);
    yBoundsSep.setDefault(TRUE);
    yBoundsXf.setDefault(TRUE);
    yScaledTranslatorSwitch.setDefault(TRUE);

    SoTranslate1Dragger::setDefaultOnNonWritingFields();
}

//////////////////////////////////////////////////////////////////////
//  Class: AxlePin
//////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(AxlePin);

void
AxlePin::initClass()
{
    SO_KIT_INIT_CLASS(AxlePin, SoTranslate1Dragger, "Translate1Dragger");
}

AxlePin::AxlePin()
{
    SO_KIT_CONSTRUCTOR(AxlePin);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(pickStyle, SoPickStyle, FALSE,
			     geomSeparator, translatorSwitch, FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(translatorInert, SoSeparator, TRUE,
			     translatorSwitch, , TRUE);

    SO_KIT_INIT_INSTANCE();

    // From SoInteractionKit class
    MechPiece::makeNodeFieldStopNotifying( topSeparator);
    MechPiece::makeNodeFieldStopNotifying( geomSeparator);
    // From SoDragger class
    MechPiece::makeNodeFieldStopNotifying( motionMatrix);
    // From SoTranslate1Dragger class
    MechPiece::makeNodeFieldStopNotifying( translatorSwitch);
    MechPiece::makeNodeFieldStopNotifying( translator );
    MechPiece::makeNodeFieldStopNotifying( translatorActive );
    MechPiece::makeNodeFieldStopNotifying( feedbackSwitch);
    MechPiece::makeNodeFieldStopNotifying( feedback );
    MechPiece::makeNodeFieldStopNotifying( feedbackActive );
    // From this class
    MechPiece::makeNodeFieldStopNotifying( pickStyle);
    MechPiece::makeNodeFieldStopNotifying( translatorInert);

    // Set up the default geometries
	setPartAsDefault("translator",       "axlePinTranslator");
	setPartAsDefault("translatorActive", "axlePinTranslatorActive");
	setPartAsDefault("translatorInert", "axlePinTranslatorInert");
	setPartAsDefault("feedback", "axlePinFeedback");
	setPartAsDefault("feedbackActive", "axlePinFeedbackActive");

    setUpConnections( TRUE, TRUE );

}

SbBool
AxlePin::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {
	// We connect AFTER base class.
	SoTranslate1Dragger::setUpConnections( onOff, FALSE );
    }
    else {

	SoTranslate1Dragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

////////////////////////////////////////////////////////////////////////
//    Destructor
//
AxlePin::~AxlePin()
{
}

SbBool
AxlePin::isDraggable()
{
    SoPickStyle *ps = (SoPickStyle *) pickStyle.getValue();
    if (ps && ps->style.getValue() == SoPickStyle::UNPICKABLE)
	return FALSE;
    else
	return TRUE;
}

void
AxlePin::setDraggable( SbBool onOff )
{
    // Set the pickStyle accordingly:
	SoPickStyle *ps = (SoPickStyle *) pickStyle.getValue();
	if (ps==NULL) ps = SO_GET_ANY_PART(this,"pickStyle",SoPickStyle);
	if ( onOff && ps->style.getValue() != SoPickStyle::SHAPE)
	    ps->style = SoPickStyle::SHAPE;
	else if ( ! onOff && ps->style.getValue() != SoPickStyle::UNPICKABLE )
	    ps->style = SoPickStyle::UNPICKABLE;

    // Set the translatorSwitch:
	if (onOff) setSwitchValue( translatorSwitch.getValue(), 0);
	else       setSwitchValue( translatorSwitch.getValue(), 2);
}

void
AxlePin::setDefaultOnNonWritingFields()
{
    pickStyle.setDefault(TRUE);

    SoTranslate1Dragger::setDefaultOnNonWritingFields();
}

//////////////////////////////////////////////////////////////////////
//  Class: Axle
//////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(Axle);

void
Axle::initClass()
{
    SO_KIT_INIT_CLASS(Axle, MechPiece, "MechPiece");
}

Axle::Axle()
{
    SO_KIT_CONSTRUCTOR(Axle);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(shaftAssemblySwitch, SoSwitch, FALSE,
			     topSeparator, geomSeparator, FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(shaft, AxleShaft, FALSE,
			     shaftAssemblySwitch, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(leg, AxleLeg, FALSE,
			     shaftAssemblySwitch, , FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(translateToOrigin, SoTranslation, FALSE,
			     topSeparator, geomSeparator, FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(pinSwitch, SoSwitch, FALSE,
			     topSeparator, geomSeparator, FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(pinSep, SoSeparator, FALSE,
			     pinSwitch, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(pinZTranslationOffset, SoTranslation, FALSE,
			     pinSep, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(pinZRotationOffset, SoRotationXYZ, FALSE,
			     pinSep, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(pinZRotation, SoRotationXYZ, FALSE,
			     pinSep, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(pin, AxlePin, FALSE,
			     pinSep, , FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(feedbackSwitch, SoSwitch, TRUE,
			     geomSeparator, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(movingFeedback, SoSeparator, TRUE,
			     feedbackSwitch, , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(connectFeedback, SoSeparator, TRUE,
			     feedbackSwitch, , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(disconnectFeedback, SoSeparator, TRUE,
			     feedbackSwitch, , TRUE);

    SO_KIT_ADD_FIELD( zRotation, (0.0));
    SO_KIT_ADD_FIELD( shaftZDepth, (.3));
    SO_KIT_ADD_FIELD( floorHeight, (0));
    SO_KIT_ADD_FIELD( pinZTransOffset, (0,0,.2));
    SO_KIT_ADD_FIELD( pinZRotOffset, (.7));
    SO_KIT_ADD_FIELD( pinActivationDist, (.5));
    SO_KIT_ADD_FIELD( canFollowRot, (TRUE));
    SO_KIT_ADD_FIELD( leadingAxle, (NULL));

    // shaft, leg, and pin are created by default during this macro.
    SO_KIT_INIT_INSTANCE();

    // From this class
    makeNodeFieldStopNotifying( shaftAssemblySwitch);
    makeNodeFieldStopNotifying( shaft);
    makeNodeFieldStopNotifying( leg);
    makeNodeFieldStopNotifying( translateToOrigin);
    makeNodeFieldStopNotifying( pinSwitch);
    makeNodeFieldStopNotifying( pinSep);
    makeNodeFieldStopNotifying( pinZTranslationOffset);
    makeNodeFieldStopNotifying( pinZRotationOffset);
    makeNodeFieldStopNotifying( pinZRotation);
    makeNodeFieldStopNotifying( pin);
    makeNodeFieldStopNotifying( feedbackSwitch);
    makeNodeFieldStopNotifying( movingFeedback);
    makeNodeFieldStopNotifying( connectFeedback);
    makeNodeFieldStopNotifying( disconnectFeedback);
    makeNodeFieldStopNotifying( leadingAxle );

    SoSeparator *sep;
    sep = SO_GET_ANY_PART( this, "pinSep", SoSeparator);
    sep->renderCaching = OFF;
    sep->boundingBoxCaching = OFF;
    sep->renderCulling= OFF;
    sep->pickCulling = OFF;

    // Initialize switch nodes.
    setSwitchValue( shaftAssemblySwitch.getValue(), SO_SWITCH_ALL );
    setSwitchValue( feedbackSwitch.getValue(), SO_SWITCH_NONE ); 
    setSwitchValue( pinSwitch.getValue(), SO_SWITCH_NONE );

    // Set up default geometries.
    setPartAsDefault("movingFeedback","axleMovingFeedback");
    setPartAsDefault("connectFeedback","axleConnectFeedback");
    setPartAsDefault("disconnectFeedback","axleDisconnectFeedback");

    prevZPlane = 0.0;

    // These field sensors update the zMin and zMax fields of the shaft.
    shaftZDepthFieldSensor
	= new SoFieldSensor(&Axle::axleFieldSensorCB,this);
    shaftZDepthFieldSensor->setPriority(0);

    axleFollowEngine = new AxleFollowEngine;
    axleFollowEngine->ref();

    recursiveFollowersList = new SbPList;

    setUpConnections( TRUE, TRUE );

}

void 
Axle::setZPlane( float newZ )
{
    MechPiece::setZPlane(newZ);

    // We do these special since the base class disables notification
    // from the origin field.
    AxleShaft *shf = (AxleShaft *) shaft.getValue();
    if (shf && shf->translation.getValue()[2] != newZ) {
	shf->translation = origin.getValue();
    }
    AxleLeg   *lg = (AxleLeg *) leg.getValue();
    if (lg && lg->translation.getValue()[2] != newZ) {
	lg->translation = origin.getValue();
    }
    SoTranslation *xl = (SoTranslation *) translateToOrigin.getValue();
    if (xl && xl->translation.getValue()[2] != newZ) {
	xl->translation = origin.getValue();
    }

    adjustShaftZMinMax( TRUE );
}

SbBool
Axle::undoConnections(SbBool )
{
    // Disconnect the field sensors.
    if (shaftZDepthFieldSensor->getAttachedField())
	shaftZDepthFieldSensor->detach();

    // If we're following another axle, we need to undo the axleFollowEngine
	setUpFollowConnections( FALSE );

    AxleShaft *shf = (AxleShaft *) shaft.getValue();
    AxleLeg   *lg = (AxleLeg *) leg.getValue();
    AxlePin   *pn = (AxlePin *) pin.getValue();

    // Remove callbacks
	if (shf) shf->removeStartCallback( &Axle::subDraggerStartCB, this );
	if (lg) lg->removeStartCallback( &Axle::subDraggerStartCB, this );
	if (pn) pn->removeStartCallback( &Axle::subDraggerStartCB, this );
	if (pn) pn->removeStartCallback( &Axle::pinStartCB, this );

	if (shf) shf->removeValueChangedCallback( &Axle::shaftMovedCB, this );
	if (lg) lg->removeValueChangedCallback( &Axle::legMovedCB, this );
	if (pn) pn->removeValueChangedCallback( &Axle::pinMovedCB, this );

	if (shf) shf->removeFinishCallback( &Axle::shaftFinishCB, this );
	if (lg) lg->removeFinishCallback( &Axle::legFinishCB, this );
	if (pn) pn->removeFinishCallback( &Axle::pinFinishCB, this );

    // Disconnect fields and engines
    // Disconnect shaft fields
	if (shf) {
	    disconnectField( shf->translation );
	    disconnectField( shf->zRotation );
	}
    // Disconnect leg fields
	if (lg) {
	    disconnectField( lg->translation );
	    disconnectField( lg->yMin );
	}
    // Disconnect pinZRotation pinZRotationOffset & pinZTranslationOffset fields
	SoRotationXYZ *zrot;
	zrot = (SoRotationXYZ *) pinZRotation.getValue();
	if (zrot) disconnectField( zrot->angle );
	zrot = (SoRotationXYZ *) pinZRotationOffset.getValue();
	if (zrot) disconnectField( zrot->angle );
	SoTranslation *xl;
	xl = (SoTranslation *) pinZTranslationOffset.getValue();
	if (xl) disconnectField( xl->translation );
    // Disconnect the node that translates the feedback and pin to origin
	xl = (SoTranslation *) translateToOrigin.getValue();
	if (xl) disconnectField( xl->translation );

    // We disconnect base class LAST.
    MechPiece::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
Axle::setUpConnections(SbBool onOff, SbBool doItAlways)
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    // We connect AFTER base class.
	MechPiece::setUpConnections( onOff, FALSE );

    // Replace default geometries of sub-parts if desired.
    // (none)  Leave it to each class to make its own.

    // Connect fields and engines
    // Connect shaft fields
	AxleShaft *shf = (AxleShaft *) shaft.getValue();
	if (shf)  {
	    connectFromField( &origin, &shf->translation );
	    connectFromField( &zRotation, &shf->zRotation );
	}
    // Connect leg fields
	AxleLeg   *lg = (AxleLeg *) leg.getValue();
	if (lg) {
	    connectFromField( &origin, &lg->translation );
	    connectFromField( &floorHeight, &lg->yMin );
	}
    // Connect pin fields
	AxlePin   *pn = (AxlePin *) pin.getValue();
    // Connect pinZRotation, pinZRotationOffset and pinZTranslationOffset fields
	SoRotationXYZ *zrot;
	zrot = SO_GET_ANY_PART(this,"pinZRotation",SoRotationXYZ);
	if (zrot->axis.getValue() != SoRotationXYZ::Z)
	    zrot->axis = SoRotationXYZ::Z;
	connectFromField( &zRotation, &zrot->angle );
	zrot = SO_GET_ANY_PART(this,"pinZRotationOffset",SoRotationXYZ);
	if (zrot->axis.getValue() != SoRotationXYZ::Z)
	    zrot->axis = SoRotationXYZ::Z;
	connectFromField( &pinZRotOffset, &zrot->angle );
	SoTranslation *xl;
	xl = SO_GET_ANY_PART(this,"pinZTranslationOffset",SoTranslation);
	connectFromField( &pinZTransOffset, &xl->translation );
    // Connect the node that translates the feedback and pin to origin
	xl = SO_GET_ANY_PART(this,"translateToOrigin",SoTranslation);
	connectFromField( &origin, &xl->translation );

    // This insures internal consistency
	setDraggable( isDraggable() );

    // Set up the shaft depth.
	tellLeaderToUpdateFollowers();
	axleFieldSensorCB( this, NULL );

    // Add callbacks
	if (shf) shf->addStartCallback( &Axle::subDraggerStartCB, this );
	if (lg) lg->addStartCallback( &Axle::subDraggerStartCB, this );
	if (pn) pn->addStartCallback( &Axle::subDraggerStartCB, this );
	if (pn) pn->addStartCallback( &Axle::pinStartCB, this );

	if (shf) shf->addValueChangedCallback( &Axle::shaftMovedCB, this );
	if (lg) lg->addValueChangedCallback( &Axle::legMovedCB, this );
	if (pn) pn->addValueChangedCallback( &Axle::pinMovedCB, this );

	if (shf) shf->addFinishCallback( &Axle::shaftFinishCB, this );
	if (lg) lg->addFinishCallback( &Axle::legFinishCB, this );
	if (pn) pn->addFinishCallback( &Axle::pinFinishCB, this );

	// We OWN these nodes, and they are private. We can turn
	// notification off since only we will change them. We do this
	// to save time on notification.
	SoNode *myN;
	if (myN = translateToOrigin.getValue())     myN->enableNotify(FALSE);

    // Connect the field sensors
	if (shaftZDepthFieldSensor->getAttachedField() != &shaftZDepth)
	    shaftZDepthFieldSensor->attach( &shaftZDepth );

    // If we're following another axle, we need to connect axleFollowEngine
        tellLeaderToUpdateFollowers();
	follow( (Axle *) leadingAxle.getValue(), TRUE );

    return !(connectionsSetUp = onOff);
}

Axle::~Axle()
{
    if (recursiveFollowersList)
	delete recursiveFollowersList;

    if (axleFollowEngine)
        axleFollowEngine->unref();
    if (shaftZDepthFieldSensor)
	delete shaftZDepthFieldSensor;
}

void 
Axle::subDraggerStartCB( void *inAxle, SoDragger *)
{
    Axle *myself = (Axle *) inAxle;
    myself->tellLeaderToUpdateFollowers();

    // Show the default draggin' around feedback
    myself->setSwitchValue(myself->feedbackSwitch.getValue(), 0 ); 
}

void 
Axle::pinStartCB( void *inAxle, SoDragger *inDragger)
{
    Axle *myself = (Axle *) inAxle;
    AxlePin *pn = (AxlePin *) inDragger;
    pn->touch();
}

void 
Axle::shaftMovedCB( void *inAxle, SoDragger *inDragger )
{
    SoTranslate2Dragger *dgr = (SoTranslate2Dragger *) inDragger;
    SbVec3f newLoc = dgr->translation.getValue();

    Axle *myself = (Axle *) inAxle;
    if ( myself->origin.getValue() != newLoc) {

	// Move our own origin to match the shaft
        myself->origin = newLoc;

	// Establish appropriate feedback:
	if (dgr && dgr->isActive.getValue()) {
	    Axle *newLeader = myself->findAxleToFollow();
	    if (newLeader && newLeader != myself->leadingAxle.getValue()) {
    		Sound::playOnce(Sound::READY_TO_CONNECT);
		myself->setSwitchValue( myself->feedbackSwitch.getValue(), 1 );
	    }
	    else
		myself->setSwitchValue( myself->feedbackSwitch.getValue(), 0 );
        }
    }
}

void 
Axle::legMovedCB( void *inAxle, SoDragger *inDragger )
{
    SoTranslate1Dragger *dgr = (SoTranslate1Dragger *) inDragger;
    SbVec3f newLoc = dgr->translation.getValue();

    Axle *myself = (Axle *) inAxle;
    if ( myself->origin.getValue() != newLoc) {

	// Move our own origin to match the shaft
        myself->origin = newLoc;

	// Establish appropriate feedback:
	if (dgr && dgr->isActive.getValue()) {
	    Axle *newLeader = myself->findAxleToFollow();
	    if (newLeader && newLeader != myself->leadingAxle.getValue()) {
    		Sound::playOnce(Sound::READY_TO_CONNECT);
		myself->setSwitchValue( myself->feedbackSwitch.getValue(), 1 );
	    }
	    else
		myself->setSwitchValue( myself->feedbackSwitch.getValue(), 0 );
        }
    }
}

void 
Axle::pinMovedCB( void *inAxle, SoDragger *inDragger )
{
    SoTranslate1Dragger *dgr = (SoTranslate1Dragger *) inDragger;

    // Establish appropriate feedback:
    if (dgr && dgr->isActive.getValue()) {
	Axle   *myself = (Axle *) inAxle;
	SbBool canUnlock = myself->isPinReadyToUnlock();
	if (canUnlock) {
	    Sound::playOnce(Sound::READY_TO_DISCONNECT);
	    myself->setSwitchValue(myself->feedbackSwitch.getValue(), 2 ); 
	}
	else
	    myself->setSwitchValue(myself->feedbackSwitch.getValue(), 0 ); 
    }
}

void 
Axle::shaftFinishCB( void *inAxle, SoDragger *inDragger )
{
    SoTranslate2Dragger *dgr = (SoTranslate2Dragger *) inDragger;
    SbVec3f newLoc = dgr->translation.getValue();

    Axle *myself = (Axle *) inAxle;
    if ( myself->origin.getValue() != newLoc)
         myself->origin = newLoc;

    // Remove the motion feedback
    myself->setSwitchValue(myself->feedbackSwitch.getValue(), SO_SWITCH_NONE ); 

    Axle *newLeader = myself->findAxleToFollow();
    if (newLeader != myself->leadingAxle.getValue()) {
	Sound::playOnce(Sound::CONNECTED);
	myself->follow(newLeader);
    }
}

void 
Axle::legFinishCB( void *inAxle, SoDragger *inDragger )
{
    SoTranslate1Dragger *dgr = (SoTranslate1Dragger *) inDragger;
    SbVec3f newLoc = dgr->translation.getValue();

    Axle *myself = (Axle *) inAxle;
    if ( myself->origin.getValue() != newLoc)
         myself->origin = newLoc;

    // Remove the motion feedback
    myself->setSwitchValue(myself->feedbackSwitch.getValue(), SO_SWITCH_NONE ); 

    Axle *newLeader = myself->findAxleToFollow();
    if (newLeader != myself->leadingAxle.getValue()) {
	Sound::playOnce(Sound::CONNECTED);
	myself->follow(newLeader);
    }
}

void 
Axle::pinFinishCB( void *inAxle, SoDragger *inDragger )
{
    Axle *myself = (Axle *) inAxle;

    // Remove the motion feedback
    myself->setSwitchValue(myself->feedbackSwitch.getValue(), SO_SWITCH_NONE ); 

    if ( myself->isPinReadyToUnlock() ) {
	myself->follow( NULL );
	Sound::playOnce(Sound::DISCONNECTED);
    }

    // Finally, set the pin back to its local origin.
    AxlePin *pn = (AxlePin *) inDragger;
    pn->translation = SbVec3f(0,0,0);

}

Axle * 
Axle::findAxleToFollow()
{
    if (getWorld() == NULL)
	return ((Axle *) leadingAxle.getValue());

    // If activationDist is 0 or less, can't change configuration.
    float   thresh = pinActivationDist.getValue();
    if (thresh <= 0.0)
	return ((Axle *) leadingAxle.getValue());

    float   threshSq = thresh * thresh;

    // Find closest axle (in Z) with an origin that lies within
    // a planar distance of pinActivationDistance or our own origin.
    // If there is one, we should now follow it.
    // If not, then we should stop following any other axles.

    SbPList *worldAxles = getWorld()->getAxleList();
    SbPList *worldAxleOwners = getWorld()->getAxleOwnerList();

    // Figure out which piece owns this axle.
    MechPiece *myOwner = NULL;
    int whichAmI = worldAxles->find(this);
    if (whichAmI >= 0)
        myOwner = (MechPiece *) (*worldAxleOwners)[whichAmI];

    Axle *newLeader = NULL;
    float minZDist = 500000;

    SbVec3f myLoc = origin.getValue();

    Axle    *otherAxle;
    SbVec3f diff;
    for (int i = 0; i < worldAxles->getLength(); i++ ) {
	otherAxle = (Axle *) (*worldAxles)[i];

	// Never follow yourself
	if (otherAxle == this)
	    continue;
	// Never follow another follower. Find its leader instead.
	if (otherAxle->leadingAxle.getValue() != NULL )
	    continue;

	diff = myLoc - otherAxle->origin.getValue();

	if (fabs(diff[0]) > thresh) 
	    continue;
	if (fabs(diff[1]) > thresh) 
	    continue;
	if ( (diff[0] * diff[0] + diff[1] * diff[1]) < threshSq ) {
	    // Planar distance is small enough. Is this closest in Z?
	    float zDist = fabs( myLoc[2] - otherAxle->origin.getValue()[2]);
	    if (newLeader == NULL || zDist < minZDist) {

		// Make sure that this axle doesn't have the same owner as 
		// this axle.
    		MechPiece *owner = (MechPiece *) (*worldAxleOwners)[i];
		if (owner != myOwner) {
		    newLeader = otherAxle;
		    minZDist = zDist;
		}
	    }
	}
    }

    return newLeader;
}

SbBool 
Axle::isPinReadyToUnlock()
{
    // If activationDist is 0 or less, can't change configuration.
    float   thresh = pinActivationDist.getValue();
    if (thresh <= 0.0)
	return FALSE;

    SbVec3f pinTrans = ((AxlePin *)pin.getValue())->translation.getValue();

    // If > pinActivationDistance, then we can unlock
    if ( fabs(pinTrans[0]) > thresh )
	return TRUE;
    else
	return FALSE;
}

void
Axle::axleFieldSensorCB( void *inAxle, SoSensor * )
{
    Axle *myself = (Axle *) inAxle;
    myself->adjustShaftZMinMax( TRUE );
}

SbBool
Axle::isDraggable()
{
    // If an existing sub-piece is draggable, then
    // the axle is considered draggable.

    AxleShaft *shf = (AxleShaft *) shaft.getValue();
    AxleLeg   *lg = (AxleLeg *) leg.getValue();
    AxlePin   *pn = (AxlePin *) pin.getValue();

    SbBool answer = FALSE;
    if ( shf && shf->isDraggable() )
	answer = TRUE;
    if ( lg && lg->isDraggable() )
	answer = TRUE;
    if ( pn && pn->isDraggable() )
	answer = TRUE;

    return answer;
}

void
Axle::setDraggable( SbBool onOff )
{
    AxleShaft *shf = (AxleShaft *)shaft.getValue();
    AxleLeg   *lg = (AxleLeg *)leg.getValue();
    AxlePin   *pn = (AxlePin *)pin.getValue();

    if (shf ) shf->setDraggable( onOff );
    if (lg ) lg->setDraggable( onOff );
    if (pn ) pn->setDraggable( onOff );
}

SbBool 
Axle::isShowingParts()
{
    // Return TRUE if either the shaftAssemblySwitch or pinSwitch is
    // showing its children.
    SoSwitch *sw;

    sw = (SoSwitch *) shaftAssemblySwitch.getValue();
    if (sw && sw->whichChild.getValue() == SO_SWITCH_ALL )
	return TRUE;

    sw = (SoSwitch *) pinSwitch.getValue();
    if (sw && sw->whichChild.getValue() == SO_SWITCH_ALL )
	return TRUE;

    return FALSE;
}
    
void   
Axle::setShowingParts( SbBool onOff )
{
    if ( !onOff ) {
	setSwitchValue( shaftAssemblySwitch.getValue(), SO_SWITCH_NONE );
	setSwitchValue( pinSwitch.getValue(), SO_SWITCH_NONE );
    }
    else {
	setSwitchValue( shaftAssemblySwitch.getValue(), SO_SWITCH_ALL );
	setSwitchValue( pinSwitch.getValue(), SO_SWITCH_ALL );
    }
}


SbBool
Axle::isWithLegs()
{
    AxleLeg   *lg = (AxleLeg *)leg.getValue();

    return (lg == NULL);
}

void
Axle::setWithLegs( SbBool onOff )
{
    AxleLeg   *lg = (AxleLeg *)leg.getValue();

    // Leg already exists.
    if ( onOff && (lg != NULL) )
	return;

    // Leg does not exist.
    if ( !onOff && (lg == NULL) )
	return;

    if ( onOff == FALSE )
	setAnyPart( "leg", NULL );
    else 
	setAnyPart( "leg", new AxleLeg );

    // Undo and redo the connections.
    setUpConnections( FALSE );
    setUpConnections( TRUE );
}

void
Axle::adjustShaftZMinMax( SbBool doLeader, SbBool checkZPlane )
{
    Axle      *leader = (Axle *) leadingAxle.getValue();

    float newZMin, newZMax;

    float originZ = origin.getValue()[2];

#define CLOSE_IN_Z .005
    SbBool originZChanged = fabs(originZ - prevZPlane ) > CLOSE_IN_Z;
    prevZPlane = originZ;
#undef CLOSE_IN_Z

    if ( leader != NULL ) {
	newZMin = -shaftZDepth.getValue();
	newZMax =  shaftZDepth.getValue();
	if (doLeader == TRUE && (originZChanged || !checkZPlane))
	    leader->adjustShaftZMinMax( TRUE, FALSE );
    }
    else {
        // If this Axle is leading, adjust to include all followers.

	// Find the nearest and farthest z locations among this axle
	// and all of the following axles (if any).
	newZMax = originZ;
	newZMin = newZMax;

	SbPList *fols = getRecursiveFollowersList();
	for (int i = 0; i < fols->getLength(); i++ ) {
	    Axle *ax = (Axle *) (*fols)[i];
	    float z = ax->origin.getValue()[2];
	    newZMin = (z < newZMin) ? z : newZMin;
	    newZMax = (z > newZMax) ? z : newZMax;
	}
	// Spread the bounds a distance of shaftZDepth from the ends.
	newZMin -= shaftZDepth.getValue();
	newZMax += shaftZDepth.getValue();

	// Make them relative to our own origin.
	newZMin -= originZ;
	newZMax -= originZ;
    }

    AxleShaft *shf    = (AxleShaft *)shaft.getValue();
    if (shf == NULL)
	return;


    if (shf->zMin.getValue() != newZMin)
	shf->zMin = newZMin;
    if (shf->zMax.getValue() != newZMax)
	shf->zMax = newZMax;

}

// Description:
//    Checks entries, then returns the axleList.
SbPList *
Axle::getAxlePtrs()
{
    SbPList *ap = MechPiece::getAxlePtrs();

    if (ap->getLength() != 1 || ap->find(this) == -1) {
	ap->truncate(0);
	ap->append(this);
    }
    return ap;
}

void 
Axle::transferFollowers( Axle *newLeader )
{
    if (getWorld() == NULL)
	return;

    // Any axle that has this as leader should change to newLeader
    SbPList *worldAxles = getWorld()->getAxleList();
    for (int i = 0; i < worldAxles->getLength(); i++) {
	Axle *ax = (Axle *) (*worldAxles)[i];
	if (ax->leadingAxle.getValue() == this )
	    ax->follow( newLeader );
    }
    tellLeaderToUpdateFollowers();
    adjustShaftZMinMax( TRUE, FALSE );
}

SbPList *
Axle::getRecursiveFollowersList()
{
    return recursiveFollowersList;
}

void     
Axle::tellLeaderToUpdateFollowers()
{
    Axle   *leader = this;

    SoNode *testNode;

    while ( (testNode = leader->leadingAxle.getValue()) != NULL )
	leader = (Axle *) testNode;

    leader->updateRecursiveFollowersList();
}

void     
Axle::updateRecursiveFollowersList()
{
    // Clear out the list.
    recursiveFollowersList->truncate(0);

    Axle    *follower;

    // For each axle in the world:
    if (getWorld() == NULL)
	return;

    SbPList *worldAxles = getWorld()->getAxleList();
    for (int i = 0; i < worldAxles->getLength(); i++) {
	follower = (Axle *) (*worldAxles)[i];

	// Keep going until leader is either 'this' or NULL:
	for ( Axle *leader = (Axle *) follower->leadingAxle.getValue();
	      leader != NULL; 
	      leader = (Axle *) leader->leadingAxle.getValue() ) {
	    if (leader == this ) {
		// add to list
		recursiveFollowersList->append( follower );
	    }
	}
    }
}

void 
Axle::setWorld( WorldInfo *newWorld )
{
    MechPiece::setWorld( newWorld );
    if (newWorld)
	floorHeight = newWorld->getTableTop()->bottom.getValue();
    tellLeaderToUpdateFollowers();
    adjustShaftZMinMax( TRUE, TRUE );
}

// Description:
//    Makes sure the axle follows no other axle.
//    And no other axle follows this one.
//
void
Axle::undoAxleDependencies()
{
    // First, make followers follow our leader (which might be NULL)
    Axle *leader = (Axle *) leadingAxle.getValue();
    transferFollowers( leader );

    // Next, follow no other axle.
    follow( NULL );
}

void
Axle::setDefaultOnNonWritingFields()
{
    shaftAssemblySwitch.setDefault(TRUE);

    AxleShaft *shf = (AxleShaft *) shaft.getValue();
    if (shf != NULL) {
	shf->translation.setDefault(TRUE);
	shf->zRotation.setDefault(TRUE);
	shf->zMin.setDefault(TRUE);
	shf->zMax.setDefault(TRUE);
    }
    shaft.setDefault(TRUE);

    AxleLeg   *lg = (AxleLeg *) leg.getValue();
    if (lg != NULL) {
	lg->translation.setDefault(TRUE);
	lg->yMin.setDefault(TRUE);
    }
    leg.setDefault(TRUE);

    translateToOrigin.setDefault(TRUE);
    pinSwitch.setDefault(TRUE);
    pinZTranslationOffset.setDefault(TRUE);
    pinZRotationOffset.setDefault(TRUE);
    pinZRotation.setDefault(TRUE);

    AxlePin   *pn = (AxlePin *)pin.getValue();
    if (pn != NULL) {
	pn->translation.setDefault(TRUE);
    }
    pin.setDefault(TRUE);

    shaftZDepth.setDefault(TRUE);
    floorHeight.setDefault(TRUE);

    MechPiece::setDefaultOnNonWritingFields();
}

Axle *
Axle::getLeader()
{
    return (Axle *) leadingAxle.getValue();
}

// newLeader becomes the leadingAxle (protected field)
// If NULL, doesn't follow any axle.
void 
Axle::follow( Axle *newLeader, SbBool forceIt )
{
    Axle *oldLeader = (Axle *) leadingAxle.getValue();
    if (oldLeader == newLeader && forceIt == FALSE )
	return;

    // Set my leadingAxle field
    leadingAxle = newLeader;

    if (oldLeader != NULL ) {
	oldLeader->tellLeaderToUpdateFollowers();
        oldLeader->adjustShaftZMinMax( TRUE, FALSE );
    }

    if (newLeader != NULL) {

	setUpFollowConnections( TRUE );

	// set the switches.
	setSwitchValue( shaftAssemblySwitch.getValue(), SO_SWITCH_NONE );
	setSwitchValue( pinSwitch.getValue(), SO_SWITCH_ALL );
    }
    else {
	setUpFollowConnections( FALSE );

	// set the switches.
	setSwitchValue( shaftAssemblySwitch.getValue(), SO_SWITCH_ALL );
	setSwitchValue( pinSwitch.getValue(), SO_SWITCH_NONE );
    }

    // call the fieldChangedCB to adjust the zMin and zMax, and update
    // list first.
    tellLeaderToUpdateFollowers();
    adjustShaftZMinMax( TRUE, FALSE );
}

void 
Axle::setUpFollowConnections( SbBool onOff )
{
    if (onOff == TRUE) {
        Axle *myLeader = (Axle *) leadingAxle.getValue();
	if (myLeader) {
    	    // Our origin keeps its z value but gets x and y from the leader.
	    connectFromField( &myLeader->origin,
			      &axleFollowEngine->leaderOrigin );

	    connectFromEngine( &axleFollowEngine->newFollowerOrigin,
	    		       &origin );
	    if (canFollowRot.getValue())
	        connectFromField( &myLeader->zRotation, &zRotation );
	}
    }
    else {
	disconnectField( axleFollowEngine->leaderOrigin );

	disconnectField( origin );
	if (canFollowRot.getValue())
	    disconnectField( zRotation );
    }
}

void 
Axle::addShaftStartCallback( SoDraggerCB *f, void *d )
{  
    AxleShaft *shf = (AxleShaft *) shaft.getValue();
    if (shf)
	shf->addStartCallback( f, d );
}
void 
Axle::removeShaftStartCallback( SoDraggerCB *f, void *d )
{  
    AxleShaft *shf = (AxleShaft *) shaft.getValue();
    if (shf)
	shf->removeStartCallback( f, d );
}

void 
Axle::addShaftFinishCallback( SoDraggerCB *f, void *d )
{  
    AxleShaft *shf = (AxleShaft *) shaft.getValue();
    if (shf)
	shf->addFinishCallback( f, d );
}
void 
Axle::removeShaftFinishCallback( SoDraggerCB *f, void *d )
{  
    AxleShaft *shf = (AxleShaft *) shaft.getValue();
    if (shf)
	shf->removeFinishCallback( f, d );
}
