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
 |   $Revision: 1.3 $
 |
 |   Classes:
 |      Defines classes for creating links to be used in linkages.
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodekits/SoAppearanceKit.h>

#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/draggers/SoRotateDiscDragger.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoScale2UniformDragger.h>
#include <Inventor/draggers/SoRotateDiscDragger.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/errors/SoDebugError.h>

#include "LinkClasses.h"
#include "Sound.h"

//////////////////////////////////////////////////////////////////////////////
//    Class: LinkBase
//////////////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(LinkBase);

//
// Description:
//    This initializes the LinkBase class.
//
// Use: public
//
void
LinkBase::initClass()
{
    SO_KIT_INIT_CLASS(LinkBase, MechPiece, "MechPiece" );
}

//
// Description:
//    Constructor
//
// Use: public
//
LinkBase::LinkBase()
{
    SO_KIT_CONSTRUCTOR(LinkBase);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(appearance, SoAppearanceKit, TRUE,
				topSeparator, , FALSE );

    // Add the new fields.
    SO_KIT_ADD_FIELD(isError,    (0));

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( appearance);

    isShowingErrorColor = FALSE;
    savedMaterial = NULL;

    isErrorSensor = new SoFieldSensor(&LinkBase::isErrorSensorCB, this );;

    setUpConnections( TRUE, TRUE );
}

void
LinkBase::isErrorSensorCB(  void *mePtr, SoSensor *)
{
    LinkBase *myself = (LinkBase *) mePtr;

    myself->errorColor( myself->isError.getValue() );
}

SbBool
LinkBase::undoConnections(SbBool ) 
{
    // Disconnect the field sensors.
    isErrorSensor->detach();

    // We disable base class LAST.
    MechPiece::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
LinkBase::setUpConnections(SbBool onOff, SbBool doItAlways ) 
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    // We connect AFTER base class.
    MechPiece::setUpConnections( onOff, FALSE );

    // Call the sensor CBs to make things are up-to-date.
    isErrorSensorCB(  this, NULL);

    // Connect the field sensors
    isErrorSensor->attach(&isError);

    return !(connectionsSetUp = onOff);
}

void
LinkBase::setDefaultOnNonWritingFields()
{
    appearance.setDefault(TRUE);

    MechPiece::setDefaultOnNonWritingFields();
}

void 
LinkBase::errorColor( SbBool useErrorColor )
{
    if (isShowingErrorColor == useErrorColor )
	return;

    if (useErrorColor == FALSE) {
	setPartAsDefault( "material", savedMaterial );
	isShowingErrorColor = FALSE;
    }
    else {
	SoNode *newSavedMtl = getAnyPart( "material", FALSE );
	if (newSavedMtl)
	    newSavedMtl->ref();
	if (savedMaterial)
	    savedMaterial->unref();
	savedMaterial = newSavedMtl;
	SoNode *errorMat = SoNode::getByName("linkBaseErrorMaterial");
	setPartAsDefault( "material", errorMat );	
	isShowingErrorColor = TRUE;
    }
}

LinkBase::~LinkBase()
{
    if (isErrorSensor)
        delete isErrorSensor;
}

SbPList *
LinkBase::getAxlePtrs()
{
    return MechPiece::getAxlePtrs();
}


//////////////////////////////////////////////////////////////////////////////
//    Class: SimpleLink
//
//    New nodes in this subclass are:
//      originTranslator, originTranslateGeom,
//      angleRotator, angleRotateGeom,
//      endPointTranslateSeparator, endPointTranslator, endPointTranslateGeom,
//      oneDScaleSeparator, oneDScaler, oneDScaleGeom,
//      twoDScaleSeparator, twoDScaler, twoDScaleGeom,
//      threeDScaleSeparator, threeDScaler, threeDScaleGeom,
//
//    No New fields in this subclass are:
//////////////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(SimpleLink);

void
SimpleLink::initClass()
{
    SO_KIT_INIT_CLASS(SimpleLink, LinkBase, "LinkBase" );
}

SimpleLink::SimpleLink()
{
    SO_KIT_CONSTRUCTOR(SimpleLink);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(originTranslator, SoTranslation, TRUE,
				topSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(originTranslateGeom, SoSeparator, TRUE,
				topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(angleRotator, SoRotationXYZ, TRUE,
				topSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(angleRotateGeom, SoSeparator, TRUE,
				topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(endPointTranslateSeparator, SoSeparator, TRUE,
				topSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(endPointTranslator, SoTranslation, TRUE,
				endPointTranslateSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(endPointTranslateGeom, SoNode, SoCube, TRUE,
				endPointTranslateSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(oneDScaleSeparator, SoSeparator, TRUE,
				topSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(oneDScaler, SoScale, TRUE,
				oneDScaleSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(oneDScaleGeom, SoNode, SoCube, TRUE,
				oneDScaleSeparator, , TRUE );

     SO_KIT_ADD_CATALOG_ENTRY(twoDScaleSeparator, SoSeparator, TRUE,
 				topSeparator, , FALSE );
     SO_KIT_ADD_CATALOG_ENTRY(twoDScaler, SoScale, TRUE,
 				twoDScaleSeparator, , FALSE );
     SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(twoDScaleGeom, SoNode, SoCube, TRUE,
 				twoDScaleSeparator, , TRUE );

     SO_KIT_ADD_CATALOG_ENTRY(threeDScaleSeparator, SoSeparator, TRUE,
 				topSeparator, , FALSE );
     SO_KIT_ADD_CATALOG_ENTRY(threeDScaler, SoScale, TRUE,
 				threeDScaleSeparator, , FALSE );
     SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(threeDScaleGeom, SoNode, SoCube, TRUE,
 				threeDScaleSeparator, , TRUE );

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( originTranslator);
    makeNodeFieldStopNotifying( originTranslateGeom);
    makeNodeFieldStopNotifying( angleRotator);
    makeNodeFieldStopNotifying( angleRotateGeom);
    makeNodeFieldStopNotifying( endPointTranslateSeparator);
    makeNodeFieldStopNotifying( endPointTranslator);
    makeNodeFieldStopNotifying( endPointTranslateGeom);
    makeNodeFieldStopNotifying( oneDScaleSeparator);
    makeNodeFieldStopNotifying( oneDScaler);
    makeNodeFieldStopNotifying( oneDScaleGeom);
    makeNodeFieldStopNotifying( twoDScaleSeparator);
    makeNodeFieldStopNotifying( twoDScaler);
    makeNodeFieldStopNotifying( twoDScaleGeom);
    makeNodeFieldStopNotifying( threeDScaleSeparator);
    makeNodeFieldStopNotifying( threeDScaler);
    makeNodeFieldStopNotifying( threeDScaleGeom);

    SoSeparator *sep;
    sep = SO_GET_ANY_PART( this, "endPointTranslateSeparator", SoSeparator);
    sep->renderCaching = OFF;
    sep->boundingBoxCaching = OFF;
    sep->renderCulling= OFF;
    sep->pickCulling = OFF;

    sep = SO_GET_ANY_PART( this, "oneDScaleSeparator", SoSeparator);
    sep->renderCaching = OFF;
    sep->boundingBoxCaching = OFF;
    sep->renderCulling= OFF;
    sep->pickCulling = OFF;

    sep = SO_GET_ANY_PART( this, "twoDScaleSeparator", SoSeparator);
    sep->renderCaching = OFF;
    sep->boundingBoxCaching = OFF;
    sep->renderCulling= OFF;
    sep->pickCulling = OFF;

    sep = SO_GET_ANY_PART( this, "threeDScaleSeparator", SoSeparator);
    sep->renderCaching = OFF;
    sep->boundingBoxCaching = OFF;
    sep->renderCulling= OFF;
    sep->pickCulling = OFF;

    setUpConnections( TRUE, TRUE );
}

void
SimpleLink::setZPlane(float  newZ )
{
    LinkBase::setZPlane( newZ );

    SoTranslation *xl = (SoTranslation *) getAnyPart("originTranslator",TRUE);
    if (xl && xl->translation.getValue()[2] != newZ )
	xl->translation  = origin.getValue();
}

SbBool
SimpleLink::undoConnections(SbBool )
{
    // Disconnect fields and engines
    SoTranslation *xl = (SoTranslation *) getAnyPart("originTranslator",TRUE);
    if (xl)  disconnectField( xl->translation );

    LinkBase::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
SimpleLink::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    LinkBase::setUpConnections(onOff, FALSE);

    // Connect fields and engines
    SoTranslation *xl = (SoTranslation *) getAnyPart("originTranslator",TRUE);
    if (xl) connectFromField( &origin, &xl->translation );

    // We OWN these nodes, and they are private. We can turn
    // notification off since only we will change them. We do this
    // to save time on notification.
    SoNode *myN;
    if (myN = originTranslator.getValue())        myN->enableNotify(FALSE);
    if (myN = angleRotator.getValue())        myN->enableNotify(FALSE);
    if (myN = oneDScaler.getValue())        myN->enableNotify(FALSE);
    if (myN = twoDScaler.getValue())        myN->enableNotify(FALSE);
    if (myN = threeDScaler.getValue())        myN->enableNotify(FALSE);

    return !(connectionsSetUp = onOff);
}
void
SimpleLink::setDefaultOnNonWritingFields()
{
    // This information shouldn't write to file. It will be calculated
    // when read in from file by the engines.
    originTranslator.setDefault(TRUE);
    angleRotator.setDefault(TRUE);
    endPointTranslateSeparator.setDefault(TRUE);
    endPointTranslator.setDefault(TRUE);
    oneDScaleSeparator.setDefault(TRUE);
    oneDScaler.setDefault(TRUE);
    twoDScaleSeparator.setDefault(TRUE);
    twoDScaler.setDefault(TRUE);
    threeDScaleSeparator.setDefault(TRUE);
    threeDScaler.setDefault(TRUE);

    LinkBase::setDefaultOnNonWritingFields();
}

SimpleLink::~SimpleLink()
{
}

////////////////////////////////////////////////////////////////////
//    Class: Strut - 'origin' and 'endPoint' control internal xfs.
//                 - originAxle and endPointAxle control the 
//  			'origin' and 'endPoint'
//
//    New fields in this subclass are:
//         endPoint         - location of second link point
//
////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(Strut);

void
Strut::initClass()
{
    SO_KIT_INIT_CLASS(Strut, SimpleLink, "SimpleLink" );
}

Strut::Strut()
{
    SO_KIT_CONSTRUCTOR(Strut);

    isBuiltIn = TRUE;

    // Axles are created by default.
    SO_KIT_ADD_CATALOG_ENTRY(originAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(endPointAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );

    // Add the new fields.
    SO_KIT_ADD_FIELD(endPoint,   (1,0,0));

    SO_KIT_INIT_INSTANCE();

    // create the axles
    getAnyPart("originAxle", TRUE);
    getAnyPart("endPointAxle", TRUE);

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( originAxle);
    makeNodeFieldStopNotifying( endPointAxle);

    // Set the default geometries
    setPartAsDefault("originTranslateGeom", "strutOriginTranslateGeom");
    setPartAsDefault("angleRotateGeom", "strutAngleRotateGeom");
    setPartAsDefault("endPointTranslateGeom", "strutEndPointTranslateGeom");
    setPartAsDefault("oneDScaleGeom", "strutOneDScaleGeom");
    setPartAsDefault("twoDScaleGeom", "strutTwoDScaleGeom");
    setPartAsDefault("threeDScaleGeom", "strutThreeDScaleGeom");
    setPartAsDefault("material", "strutMaterial");

    strutEngine = new StrutEngine;
    strutEngine->ref();

    setUpConnections( TRUE, TRUE );
}

void
Strut::setZPlane(float  newZ )
{
    SimpleLink::setZPlane( newZ );

    SbVec3f end = endPoint.getValue();
    if (end[2] != newZ)
        setVec3fDisabled( endPoint, SbVec3f(end[0],end[1],newZ) );
}

SbBool
Strut::undoConnections(SbBool )
{
    // Disconnect the origin and endPoint fields from the axles.
    disconnectField( origin );
    disconnectField( endPoint );

    // Disconnect the strutEngine and the internal transforms.
    disconnectField( strutEngine->origin );
    disconnectField( strutEngine->endPoint );

    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;
    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator", TRUE );
	disconnectField( rot->angle );
    // Translation to EndPoint
	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
	disconnectField( trans->translation );
    // oneD scale to EndPoint
	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
	disconnectField( scale->scaleFactor );
    // twoD scale to EndPoint
	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
	disconnectField( scale->scaleFactor );
    // threeD scale to EndPoint
	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
	disconnectField( scale->scaleFactor );

    SimpleLink::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
Strut::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    SimpleLink::setUpConnections(onOff, FALSE);

    // Connect strutEngine
    connectFromField( &origin, &strutEngine->origin );
    connectFromField( &endPoint, &strutEngine->endPoint );
    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;
    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator",TRUE);
	if (rot->axis.getValue() != SoRotationXYZ::Z )
	    rot->axis.setValue(SoRotationXYZ::Z);
        connectFromEngine( &strutEngine->angle, &rot->angle );
    // Translation to EndPoint
	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
        connectFromEngine( &strutEngine->endPointOffset, &trans->translation );
    // oneD scale to EndPoint
	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
        connectFromEngine( &strutEngine->oneDScale, &scale->scaleFactor );
    // twoD scale to EndPoint
	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
        connectFromEngine( &strutEngine->twoDScale, &scale->scaleFactor );
    // threeD scale to EndPoint
	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
        connectFromEngine( &strutEngine->threeDScale, &scale->scaleFactor );

    // Connect the origin and endPoint fields from the Axles.
    Axle *orAx = (Axle *) originAxle.getValue();
    Axle *epAx = (Axle *) endPointAxle.getValue();
    if (orAx) {
	orAx->origin = origin.getValue();
	connectFromField( &orAx->origin, &origin );
    }
    if (epAx) {
	epAx->origin = endPoint.getValue();
	connectFromField( &epAx->origin, &endPoint );
    }
    return !(connectionsSetUp = onOff);
}

void
Strut::setDefaultOnNonWritingFields()
{
    // We turned off notification on these with enableNotify(), but
    // we need to write them. So make sure they are not set to default.
    originAxle.setDefault(FALSE);
    endPointAxle.setDefault(FALSE);

    SimpleLink::setDefaultOnNonWritingFields();
}

SbPList *
Strut::getAxlePtrs()
{
    Axle *orAx = (Axle *) originAxle.getValue();
    Axle *epAx = (Axle *) endPointAxle.getValue();

    SbPList *ap = SimpleLink::getAxlePtrs();

    int num = 0;
    if (orAx) num++;
    if (epAx) num++;

    if (ap->getLength() != num || (orAx && ap->find(orAx) == -1)
			     || (epAx && ap->find(epAx) == -1)) {
	ap->truncate(0);
	if (orAx) ap->append(orAx);
	if (epAx) ap->append(epAx);
    }
    return ap;
}

Strut::~Strut()
{
    if (strutEngine)
	strutEngine->unref();
}

////////////////////////////////////////////////////////////////////
//    Class: RigidStrut - 'origin' and 'endPoint' control internal xfs.
//                 - originAxle and endPointAxle control the 
//  			'origin' and 'endPoint'
//                   Only the originAxle can be connected. The endPointAxle 
//                   moves relative to it.
//
//    New fields in this subclass are:
//         endPoint         - location of second link point
//
////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(RigidStrut);

void
RigidStrut::initClass()
{
    SO_KIT_INIT_CLASS(RigidStrut, SimpleLink, "SimpleLink" );
}

RigidStrut::RigidStrut()
{
    SO_KIT_CONSTRUCTOR(RigidStrut);

    isBuiltIn = TRUE;

    // Axles are created by default.
    SO_KIT_ADD_CATALOG_ENTRY(originAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(endPointAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );

    SO_KIT_INIT_INSTANCE();

    // create the axles
    getAnyPart("originAxle", TRUE);
    getAnyPart("endPointAxle", TRUE);

    origin = SbVec3f( 0, 2, 0);
    set("originAxle",   "origin 0 2 0");
    set("endPointAxle", "origin 1 2 0");

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( originAxle);
    makeNodeFieldStopNotifying( endPointAxle);

    // Set the default geometries
    setPartAsDefault("originTranslateGeom", "rigidStrutOriginTranslateGeom");
    setPartAsDefault("angleRotateGeom", "rigidStrutAngleRotateGeom");
    setPartAsDefault("endPointTranslateGeom", "rigidStrutEndPointTranslateGeom");
    setPartAsDefault("oneDScaleGeom", "rigidStrutOneDScaleGeom");
    setPartAsDefault("twoDScaleGeom", "rigidStrutTwoDScaleGeom");
    setPartAsDefault("threeDScaleGeom", "rigidStrutThreeDScaleGeom");
    setPartAsDefault("material", "rigidStrutMaterial");

    rigidStrutEngine = new RigidStrutEngine;
    rigidStrutEngine->ref();

    setUpConnections( TRUE, TRUE );
}

SbBool
RigidStrut::undoConnections(SbBool )
{
    // Disconnect the origin and endPoint fields from the axles.
    disconnectField( origin );

    // Disconnect the rigidStrutEngine and the internal transforms.
    disconnectField( rigidStrutEngine->origin );

    // Remove callbacks.
    Axle *epAx = (Axle *) endPointAxle.getValue();
    if (epAx) {
	epAx->removeShaftStartCallback( &RigidStrut::endAxleStartCB, this );
	epAx->removeShaftFinishCallback( &RigidStrut::endAxleFinishCB,this);
	disconnectField(epAx->origin);
    }

    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;
    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator", TRUE );
	disconnectField( rot->angle );
    // Translation to EndPoint
	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
	disconnectField( trans->translation );
    // oneD scale to EndPoint
	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
	disconnectField( scale->scaleFactor );
    // twoD scale to EndPoint
	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
	disconnectField( scale->scaleFactor );
    // threeD scale to EndPoint
	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
	disconnectField( scale->scaleFactor );

    SimpleLink::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

void 
RigidStrut::endAxleStartCB( void *mePtr, SoDragger * )
{
    RigidStrut *myself = (RigidStrut *) mePtr;
    RigidStrutEngine *myEngine = (RigidStrutEngine *) myself->rigidStrutEngine;

    Axle *epAx = (Axle *) myself->endPointAxle.getValue();
    if (epAx) {
	epAx->origin.disconnect();
        myself->connectFromField( &epAx->origin, &myEngine->endPoint );
    }

    myEngine->isOffsetMode = FALSE;
}

void 
RigidStrut::endAxleFinishCB( void *mePtr, SoDragger * )
{
    RigidStrut *myself = (RigidStrut *) mePtr;
    RigidStrutEngine *myEngine = (RigidStrutEngine *) myself->rigidStrutEngine;

    myEngine->isOffsetMode = TRUE;
    myEngine->endPoint.disconnect();

//XXX
    myEngine->offset = myEngine->endPoint.getValue() - myEngine->origin.getValue();
//XXX
    Axle *epAx = (Axle *) myself->endPointAxle.getValue();
    if (epAx) {
	myself->connectFromEngine(&myEngine->outEndPoint, &epAx->origin);
    }
}


SbBool
RigidStrut::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    SimpleLink::setUpConnections(onOff, FALSE);

    // Connect rigidStrutEngine
    connectFromField( &origin, &rigidStrutEngine->origin );
    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;
    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator",TRUE);
	if (rot->axis.getValue() != SoRotationXYZ::Z )
	    rot->axis.setValue(SoRotationXYZ::Z);
        connectFromEngine( &rigidStrutEngine->angle, &rot->angle );
    // Translation to EndPoint
	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
        connectFromEngine( &rigidStrutEngine->endPointOffset, &trans->translation );
    // oneD scale to EndPoint
	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
        connectFromEngine( &rigidStrutEngine->oneDScale, &scale->scaleFactor );
    // twoD scale to EndPoint
	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
        connectFromEngine( &rigidStrutEngine->twoDScale, &scale->scaleFactor );
    // threeD scale to EndPoint
	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
        connectFromEngine( &rigidStrutEngine->threeDScale, &scale->scaleFactor );

    // Connect the origin and endPoint fields from the Axles.
    Axle *orAx = (Axle *) originAxle.getValue();
    Axle *epAx = (Axle *) endPointAxle.getValue();
    if (orAx) {
	orAx->origin = origin.getValue();
	connectFromField( &orAx->origin, &origin );
    }
    if (epAx) {
	// Make it so endPointAxle can't connect from other things.
	// Remove the legs and set pinActivationDist to -1
	epAx->setWithLegs(FALSE);
	epAx->pinActivationDist = -1;  // Serves to disable pins.

	// Add callbacks on endPointAxle to update the offset of the engine,
	// but only when in motion.
	epAx->addShaftStartCallback( &RigidStrut::endAxleStartCB, this );
	epAx->addShaftFinishCallback( &RigidStrut::endAxleFinishCB,this);

	rigidStrutEngine->offset = epAx->origin.getValue() - origin.getValue();
	connectFromEngine(&rigidStrutEngine->outEndPoint, &epAx->origin);
    }

    return !(connectionsSetUp = onOff);
}

void
RigidStrut::setDefaultOnNonWritingFields()
{
    // We turned off notification on these with enableNotify(), but
    // we need to write them. So make sure they are not set to default.
    originAxle.setDefault(FALSE);
    endPointAxle.setDefault(FALSE);

    SimpleLink::setDefaultOnNonWritingFields();
}
SbPList *
RigidStrut::getAxlePtrs()
{
    Axle *orAx = (Axle *) originAxle.getValue();
    Axle *epAx = (Axle *) endPointAxle.getValue();

    SbPList *ap = SimpleLink::getAxlePtrs();

    int num = 0;
    if (orAx) num++;
    if (epAx) num++;

    if (ap->getLength() != num || (orAx && ap->find(orAx) == -1)
			     || (epAx && ap->find(epAx) == -1)) {
	ap->truncate(0);
	if (orAx) ap->append(orAx);
	if (epAx) ap->append(epAx);
    }
    return ap;
}

RigidStrut::~RigidStrut()
{
    if (rigidStrutEngine)
	rigidStrutEngine->unref();
}

////////////////////////////////////////////////////////////////////
//    Class: DoubleStrut - a class that contains a two-bar linkage.
//
//    New nodes in this subclass are:
//      strut1, strut2
//
//    New fields in this subclass are:
//         endPoint       - endPoint of second link
//         size1         - size of first link
//         size2         - size of second link
//
////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(DoubleStrut);

void
DoubleStrut::initClass()
{
    SO_KIT_INIT_CLASS(DoubleStrut, LinkBase, "LinkBase" );
}

DoubleStrut::DoubleStrut()
{
    SO_KIT_CONSTRUCTOR(DoubleStrut);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(strut1, Strut, TRUE, topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(strut2, Strut, TRUE, topSeparator, , TRUE );

    // Add the new fields.
    SO_KIT_ADD_FIELD(endPoint,     (2,0,0));
    SO_KIT_ADD_FIELD(size1,       (1.732));
    SO_KIT_ADD_FIELD(size2,       (1.732));

    SO_KIT_INIT_INSTANCE();

    // create the struts
    getAnyPart("strut1", TRUE);
    getAnyPart("strut2", TRUE);

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( strut1);
    makeNodeFieldStopNotifying( strut2);

    // Set an initial position for the middle point.
    set("strut1", "endPoint 1 2 0");
    set("strut2", "origin   1 2 0 endPoint 2 0 0");

    moveEndEngine = new DoubleStrutMoveEndEngine;
    moveEndEngine->ref();
    moveMiddleEngine = new DoubleStrutMoveMiddleEngine;
    moveMiddleEngine->ref();

    setPartAsDefault("material",   "doubleStrutMaterial");

    setUpConnections( TRUE, TRUE );
}

void 
DoubleStrut::sharedAxleStartCB( void *mePtr, SoDragger * )
{
    DoubleStrut *myself = (DoubleStrut *) mePtr;
    myself->setConnectedEngine( FALSE );
}

void 
DoubleStrut::sharedAxleFinishCB( void *mePtr, SoDragger * )
{
    DoubleStrut *myself = (DoubleStrut *) mePtr;
    myself->setConnectedEngine( TRUE );
}

void
DoubleStrut::setConnectedEngine( SbBool useEndEngineNotMiddle ) 
{
    Axle *spAx = SO_CHECK_ANY_PART(this,"strut1.endPointAxle", Axle );

    if (spAx == NULL)
	return;

    if (useEndEngineNotMiddle) {
	// To make sure our old sharedPoint doesn't get oriented wrong,
	// we gotta do weird stuff before:
	SbVec3f saveShared = spAx->origin.getValue();

        connectFromEngine( &moveEndEngine->outSharedPoint, &spAx->origin );
	size1.disconnect();
	size2.disconnect();
        connectFromEngine( &moveEndEngine->outError, &isError );

	// the sharedPoint might have gotten flipped from desired orientation.
	// Re-establish old one.
	spAx->origin.setValue( saveShared );
	// Now trigger engine to fire again. The engine uses saveShared
	// this time to pick a preverred orientation.
	spAx->origin.getValue();
    }
    else {
	spAx->origin.disconnect();
        connectFromEngine( &moveMiddleEngine->outSize1, &size1 );
        connectFromEngine( &moveMiddleEngine->outSize2, &size2 );
        connectFromEngine( &moveMiddleEngine->outError, &isError );
    }
}

void 
DoubleStrut::setZPlane( float newZ )
{
    SbBool wasEn = endPoint.enableNotify(FALSE);
	LinkBase::setZPlane( newZ );
	Strut *s1 = (Strut *) strut1.getValue();
	Strut *s2 = (Strut *) strut2.getValue();
	if ( s1 ) s1->setZPlane( newZ );
	if ( s2 ) s2->setZPlane( newZ );
    endPoint.enableNotify(wasEn);

    SbVec3f end = endPoint.getValue();
    if (end[2] != newZ)
        setVec3fDisabled( endPoint, SbVec3f(end[0],end[1],newZ) );

    // Set the z values in the engine inputs as well:
    if ( moveEndEngine->origin.getValue()[2] != newZ  ||
	 moveMiddleEngine->origin.getValue()[2] != newZ ) {
	SbVec3f orV = origin.getValue();   // SDL port: 'or' is a C++ keyword
	setVec3fDisabled( moveMiddleEngine->origin, orV );
	setVec3fDisabled( moveEndEngine->origin, orV );
    }
    if ( moveEndEngine->sharedPoint.getValue()[2] != newZ ||
	 moveMiddleEngine->sharedPoint.getValue()[2] != newZ ) {
        SbVec3f newVal = moveEndEngine->sharedPoint.getValue();
	newVal.setValue( newVal[0], newVal[1], newZ);
	setVec3fDisabled( moveEndEngine->sharedPoint, newVal );
	setVec3fDisabled( moveMiddleEngine->sharedPoint, newVal );
    }
    if ( moveEndEngine->endPoint.getValue()[2] != newZ ||
	 moveMiddleEngine->endPoint.getValue()[2] != newZ ) {
        SbVec3f ep = endPoint.getValue();
	setVec3fDisabled( moveEndEngine->endPoint, ep );
	setVec3fDisabled( moveMiddleEngine->endPoint, ep );
    }
}

void 
DoubleStrut::setWorld( WorldInfo *newWorld )
{
    Strut *s1 = (Strut *) strut1.getValue();
    Strut *s2 = (Strut *) strut2.getValue();
    if ( s1 ) s1->setWorld( newWorld );
    if ( s2 ) s2->setWorld( newWorld );

    LinkBase::setWorld( newWorld );
}

void 
DoubleStrut::undoAxleDependencies()
{
    Strut *s1 = (Strut *) strut1.getValue();
    Strut *s2 = (Strut *) strut2.getValue();
    if ( s1 ) s1->undoAxleDependencies();
    if ( s2 ) s2->undoAxleDependencies();

    LinkBase::undoAxleDependencies();
}

SbPList *
DoubleStrut::getAxlePtrs()
{
    SbPList *ap = LinkBase::getAxlePtrs();
    ap->truncate(0);

    Strut *s1 = (Strut *) strut1.getValue();
    if (s1) {
        SbPList *extra = s1->getAxlePtrs();
	for (int i = 0; i < extra->getLength(); i++ )
	    ap->append( (*extra)[i] );
    }
    Strut *s2 = (Strut *) strut2.getValue();
    if (s2) {
        SbPList *extra = s2->getAxlePtrs();
	for (int i = 0; i < extra->getLength(); i++ )
	    ap->append( (*extra)[i] );
    }

    return ap;
}

SbBool
DoubleStrut::undoConnections(SbBool )
{
    // Disconnect the two struts.
    Strut *s2 = (Strut *) getAnyPart("strut2", TRUE );
    disconnectField( s2->origin );

    // Disconnect fields
    disconnectField( origin );
    disconnectField( endPoint );
    disconnectField( size1 );
    disconnectField( size2 );

    // Disconnect other engine outputs.
    Axle *spAx = SO_CHECK_ANY_PART(this,"strut1.endPointAxle",Axle); 
    if (spAx) {
	disconnectField( spAx->origin );
	spAx->removeShaftStartCallback( &DoubleStrut::sharedAxleStartCB, this );
	spAx->removeShaftFinishCallback( &DoubleStrut::sharedAxleFinishCB,this);
    }

    // Disconnect engine inputs.
    disconnectField( moveEndEngine->origin ); 
    disconnectField( moveEndEngine->sharedPoint ); 
    disconnectField( moveEndEngine->endPoint ); 
    disconnectField( moveEndEngine->size1 ); 
    disconnectField( moveEndEngine->size2 ); 

    disconnectField( moveMiddleEngine->origin ); 
    disconnectField( moveMiddleEngine->sharedPoint ); 
    disconnectField( moveMiddleEngine->endPoint ); 

    disconnectField( isError );

    LinkBase::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
DoubleStrut::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    LinkBase::setUpConnections(onOff, FALSE);

    // Set the Axles to be where the fields say:
    Axle *spAx = SO_CHECK_ANY_PART(this,"strut1.endPointAxle",Axle);
    Strut *s1 = (Strut *) getAnyPart("strut1", TRUE );
    Strut *s2 = (Strut *) getAnyPart("strut2", TRUE );

    s1->origin = origin.getValue();;
    connectFromField( &s1->origin, &origin );
    s2->endPoint = endPoint.getValue();
    connectFromField( &s2->endPoint, &endPoint );

    // strut1.endPointAxle:  remove leg and
    // make it so it won't attach to other axles.
    if (spAx) {
	spAx->setWithLegs(FALSE);
	spAx->pinActivationDist = -1;  // Serves to disable pins.
    }

    // strut2.originAxle: do not display!
    // We only use strut 1 for interaction.
    Axle *s2OrAx = SO_CHECK_ANY_PART(this,"strut2.originAxle",Axle);
    if (s2OrAx) s2OrAx->setShowingParts(FALSE);

    // Set parts within the two links if they haven't already been
    // set to a non-default value...
    SoNode *part;
    part = SoNode::getByName("doubleStrutOriginTranslateGeom");
    s1->setPartAsDefault("originTranslateGeom", part); 
    s2->setPartAsDefault("originTranslateGeom", part); 
    part = SoNode::getByName("doubleStrutAngleRotateGeom");
    s1->setPartAsDefault("angleRotateGeom", part); 
    s2->setPartAsDefault("angleRotateGeom", part); 
    part = SoNode::getByName("doubleStrutEndPointTranslateGeom");
    s1->setPartAsDefault("endPointTranslateGeom", part); 
    s2->setPartAsDefault("endPointTranslateGeom", part); 
    part = SoNode::getByName("doubleStrutOneDScaleGeom");
    s1->setPartAsDefault("oneDScaleGeom", part); 
    s2->setPartAsDefault("oneDScaleGeom", part); 
    part = SoNode::getByName("doubleStrutTwoDScaleGeom");
    s1->setPartAsDefault("twoDScaleGeom", part); 
    s2->setPartAsDefault("twoDScaleGeom", part); 
    part = SoNode::getByName("doubleStrutThreeDScaleGeom");
    s1->setPartAsDefault("threeDScaleGeom", part); 
    s2->setPartAsDefault("threeDScaleGeom", part); 
    part = SoNode::getByName("doubleStrutMaterial");
    s1->setPartAsDefault("material", part); 
    s2->setPartAsDefault("material", part); 

    // Attach connections into both engines.
    connectFromField( &origin, &moveEndEngine->origin );
    connectFromField( &s1->endPoint, &moveEndEngine->sharedPoint );
    connectFromField( &endPoint, &moveEndEngine->endPoint );

    connectFromField( &size1, &moveEndEngine->size1 );
    connectFromField( &size2, &moveEndEngine->size2 );

    connectFromField( &origin, &moveMiddleEngine->origin );
    connectFromField( &s1->endPoint, &moveMiddleEngine->sharedPoint );
    connectFromField( &endPoint, &moveMiddleEngine->endPoint );

    if (spAx) {
	spAx->addShaftStartCallback( &DoubleStrut::sharedAxleStartCB, this );
	spAx->addShaftFinishCallback( &DoubleStrut::sharedAxleFinishCB,this);
    }
    setConnectedEngine( TRUE );

    // Connect into the two struts as well.
    connectFromField( &s1->endPoint, &s2->origin);

    // Call the sensor CBs to make things are up-to-date.
    sharedAxleFinishCB(  this, NULL);

    return !(connectionsSetUp = onOff);
}

void
DoubleStrut::setDefaultOnNonWritingFields()
{
    // We turned off notification on these with enableNotify(), but
    // we need to write them. So make sure they are not set to default.
    strut1.setDefault(FALSE);
    strut2.setDefault(FALSE);
    isError.setDefault(TRUE);

    Strut *s1 = (Strut *) getAnyPart("strut1", TRUE );
    if (s1 ) {
	s1->isError.setDefault(TRUE);
    }
    Strut *s2 = (Strut *) getAnyPart("strut2", TRUE );
    if (s2 ) {
	s2->isError.setDefault(TRUE);
    }

    LinkBase::setDefaultOnNonWritingFields();
}

void 
DoubleStrut::errorColor( SbBool useErrorColor )
{
    LinkBase::errorColor( useErrorColor );

    Strut *s1 = (Strut *) getAnyPart("strut1", TRUE );
    Strut *s2 = (Strut *) getAnyPart("strut2", TRUE );

    s1->isError.setValue( isError.getValue() );
    s2->isError.setValue( isError.getValue() );
}

DoubleStrut::~DoubleStrut()
{
    if (moveEndEngine)
	moveEndEngine->unref();
    if (moveMiddleEngine)
	moveMiddleEngine->unref();
}

////////////////////////////////////////////////////////////////////
//    Class: Wheel - 'origin' 'radius', and 'zRotation' 
//                       control internal xfs.
//                  - An axle in the middle,
//                    a read-only (inert) axle,
//                    a sizing ring and a rotator ring.
//
//    New fields in this subclass are:
//         radius
//         zRotation
//
////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(Wheel);

float Wheel::minRadius = .35;

void
Wheel::initClass()
{
    SO_KIT_INIT_CLASS(Wheel, SimpleLink, "SimpleLink" );
}

Wheel::Wheel()
{
    SO_KIT_CONSTRUCTOR(Wheel);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(originAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(inertAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(offsetAngleRotator, SoRotation, TRUE,
				topSeparator, angleRotateGeom, FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(rotatorRingSep, SoSeparator, TRUE,
				topSeparator, angleRotateGeom, FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(rotatorRingScaler, SoScale, FALSE,
				rotatorRingSep, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(rotatorRing, SoRotateDiscDragger, TRUE,
				rotatorRingSep, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(sizingRing, SoScale2UniformDragger, TRUE,
			    topSeparator, endPointTranslateSeparator, TRUE);

    // Add the new fields.
    SO_KIT_ADD_FIELD(radius,      (1));
    SO_KIT_ADD_FIELD(zRotation,   (0));
    SO_KIT_ADD_FIELD(offsetRotation,   (0));

    SO_KIT_INIT_INSTANCE();

    // create the axles and draggers
    getAnyPart("originAxle", TRUE);
    getAnyPart("inertAxle", TRUE);
    getAnyPart("rotatorRing", TRUE);
    getAnyPart("sizingRing", TRUE);

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( originAxle);
    makeNodeFieldStopNotifying( inertAxle);
    makeNodeFieldStopNotifying( offsetAngleRotator);
    makeNodeFieldStopNotifying( rotatorRingSep);
    makeNodeFieldStopNotifying( rotatorRingScaler);
    makeNodeFieldStopNotifying( rotatorRing);
    makeNodeFieldStopNotifying( sizingRing);

    // Set the default geometries
    setPartAsDefault("originTranslateGeom", "wheelOriginTranslateGeom");
    setPartAsDefault("angleRotateGeom", "wheelAngleRotateGeom");
    setPartAsDefault("endPointTranslateGeom", "wheelEndPointTranslateGeom");
    setPartAsDefault("oneDScaleGeom", "wheelOneDScaleGeom");
    setPartAsDefault("twoDScaleGeom", "wheelTwoDScaleGeom");
    setPartAsDefault("threeDScaleGeom", "wheelThreeDScaleGeom");
    setPartAsDefault("material", "wheelMaterial");

    wheelEngine = new WheelEngine;
    wheelEngine->ref();
    zAngleFromRotEngine = new ZAngleFromRotationEngine;
    zAngleFromRotEngine->ref();

    setUpConnections( TRUE, TRUE );
}

void
Wheel::sizingRingCB( void *mePtr, SoDragger *inDragger )
{
    // When it moves, we append copy the scaling to our own radius.
    SoScale2UniformDragger *dgr = (SoScale2UniformDragger *) inDragger;
    SbVec3f newScl = dgr->scaleFactor.getValue();

    // First disable stuff
    Wheel *myself = (Wheel *) mePtr;
    dgr->removeValueChangedCallback( &Wheel::sizingRingCB, myself );

    // Change our radius if permissible:
    float   newRad = newScl[0];
    if (newRad != myself->radius.getValue() && newRad >= getMinRadius()) {
	myself->radius = newRad;
    }
    if ( newRad < getMinRadius() ) {
	// reset dragger if illegal scale.
	dgr->scaleFactor = SbVec3f(getMinRadius(),getMinRadius(),newScl[2]);
    }

    // RE-enable
    dgr->addValueChangedCallback( &Wheel::sizingRingCB, myself );
}

void
Wheel::setZPlane( float newZ )
{
    SimpleLink::setZPlane( newZ );

    // Set the z values in the engine inputs as well:
    if ( wheelEngine->origin.getValue()[2] != newZ )
	setVec3fDisabled( wheelEngine->origin, origin.getValue() );
}

SbBool
Wheel::undoConnections(SbBool )
{
    // Disconnect the origin from the axle.
    disconnectField( origin );
    disconnectField( zRotation );

    // Disconnect the engine from our offsetRotation field
    disconnectField( wheelEngine->offsetAngle );

    // Disconnect the offsetAngleRotator from the rotatorRing's rotation
    SoRotation *offRot = (SoRotation *) offsetAngleRotator.getValue();
    if (offRot)
        disconnectField( offRot->rotation );

    // Disconnect the zAngleFromRotEngine inRotation from the
    // rotatorRing's rotation.
    disconnectField( zAngleFromRotEngine->inRotation );
    // Disconnect the offsetRotation from the zAngleFromRotEngine
    disconnectField( offsetRotation );

    // Disconnect the inert axle
    Axle *itAx = (Axle *) inertAxle.getValue();
    if (itAx) disconnectField( itAx->origin );
    if (itAx) disconnectField( itAx->zRotation );

    // Changes size of rotatorRing
    SoScale    *sclr = (SoScale *) rotatorRingScaler.getValue();
    if (sclr) disconnectField( sclr->scaleFactor );

    // Remove callback from the sizingRing
    SoScale2UniformDragger *szr 
	= (SoScale2UniformDragger *) sizingRing.getValue();
    if (szr) szr->removeValueChangedCallback( &Wheel::sizingRingCB, this );

    // Disconnect the wheelEngine and the internal transforms.
    disconnectField( wheelEngine->radius );
    disconnectField( wheelEngine->origin );
    disconnectField( wheelEngine->angle );
    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;
    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator", TRUE );
	disconnectField( rot->angle );
    // Translation to EndPoint
	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
	disconnectField( trans->translation );
    // oneD scale to EndPoint
	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
	disconnectField( scale->scaleFactor );
    // twoD scale to EndPoint
	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
	disconnectField( scale->scaleFactor );
    // threeD scale to EndPoint
	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
	disconnectField( scale->scaleFactor );

    SimpleLink::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
Wheel::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    SimpleLink::setUpConnections(onOff, FALSE);

    // Replace the default geometries in sub-parts, if desiered.

    // Connect wheelEngine
    connectFromField( &origin, &wheelEngine->origin);
    connectFromField( &zRotation, &wheelEngine->angle);
    connectFromField( &radius, &wheelEngine->radius);

    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;
    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator",TRUE);
	if (rot->axis.getValue() != SoRotationXYZ::Z )
	    rot->axis.setValue(SoRotationXYZ::Z);
	connectFromField( &zRotation, &rot->angle);
    // Translation to EndPoint
	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
	connectFromEngine( &wheelEngine->endPointOffset, &trans->translation);
    // oneD scale to EndPoint
	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
	connectFromEngine( &wheelEngine->oneDScale, &scale->scaleFactor);
    // twoD scale to EndPoint
	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
	connectFromEngine( &wheelEngine->twoDScale, &scale->scaleFactor);
    // threeD scale to EndPoint
	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
	connectFromEngine( &wheelEngine->threeDScale, &scale->scaleFactor);


    // Connect the zAngleRotationEngine:
    SoRotateDiscDragger    *rtr 
	= (SoRotateDiscDragger *) rotatorRing.getValue();
    if (rtr) {
	SbRotation curRot( SbVec3f(0,0,1), offsetRotation.getValue() );
	rtr->rotation.setValue( curRot );
	connectFromField( &rtr->rotation, &zAngleFromRotEngine->inRotation );
	connectFromEngine( &zAngleFromRotEngine->outAngle, &offsetRotation );
    }

    // Connect the engine from our offsetRotation field
    connectFromField( &offsetRotation, &wheelEngine->offsetAngle );

    // Replace the default geometries in sub-parts, if desiered.
    Axle *itAx = (Axle *) inertAxle.getValue();
    if (itAx) {
	itAx->setWithLegs(FALSE);
        itAx->pinActivationDist = -1; // Serves to disable pins.
	itAx->setDraggable(FALSE);
	connectFromEngine( &wheelEngine->endPointLoc, &itAx->origin);
	connectFromEngine( &wheelEngine->totalRot, &itAx->zRotation);
    }

    // Connect the origin fields from the Axles.
    Axle *orAx = (Axle *) originAxle.getValue();
    if (orAx) {
	orAx->origin = origin.getValue();
	connectFromField( &orAx->origin, &origin);
	orAx->zRotation = zRotation.getValue();
	connectFromField( &orAx->zRotation, &zRotation);
    }

    // Changes size of rotatorRing
    SoScale    *sclr = (SoScale *) rotatorRingScaler.getValue();
    if (sclr) connectFromEngine( &wheelEngine->twoDScale, &sclr->scaleFactor);

    // Change geometry of draggers
    SoScale2UniformDragger *szr 
	= (SoScale2UniformDragger *) sizingRing.getValue();


    // Connect the offsetAngleRotator from the rotatorRing's rotation
    SoRotation *offRot = (SoRotation *) offsetAngleRotator.getValue();
    if (offRot) {
	if (rtr->rotation.getValue() != offRot->rotation.getValue() )
	    rtr->rotation = offRot->rotation.getValue();
        connectFromField( &rtr->rotation, &offRot->rotation );
    }

    rtr->setPartAsDefault("rotator","wheelRotator");
    rtr->setPartAsDefault("rotatorActive","wheelRotatorActive");
    szr->setPartAsDefault("scaler","wheelScaler");
    szr->setPartAsDefault("scalerActive","wheelScalerActive");

    // Add the callbacks to the draggers
    if (szr) szr->addValueChangedCallback( &Wheel::sizingRingCB, this );

    // Connect the field sensors

    return !(connectionsSetUp = onOff);
}

void
Wheel::setDefaultOnNonWritingFields()
{
    // We turned off notification on these with enableNotify(), but
    // we need to write them. So make sure they are not set to default.
    originAxle.setDefault(FALSE);
    inertAxle.setDefault(FALSE);
    rotatorRing.setDefault(FALSE);
    sizingRing.setDefault(FALSE);

    Axle *inAx = (Axle *) inertAxle.getValue();
    if (inAx) {
	inAx->origin.setDefault(TRUE);
	inAx->zRotation.setDefault(TRUE);
    }

    rotatorRingSep.setDefault(TRUE);
    rotatorRingScaler.setDefault(TRUE);

    SimpleLink::setDefaultOnNonWritingFields();
}

SbPList *
Wheel::getAxlePtrs()
{
    Axle *orAx = (Axle *) originAxle.getValue();
    Axle *itAx = (Axle *) inertAxle.getValue();

    int num = 0; 
    if (orAx) num++;
    if (itAx) num++;

    SbPList *ap = SimpleLink::getAxlePtrs();
    if (ap->getLength() != num || (orAx && ap->find(orAx) == -1)
			       || (itAx && ap->find(itAx) == -1)) {
	ap->truncate(0);
	if (orAx) ap->append(orAx);
	if (itAx) ap->append(itAx);
    }
    return ap;
}

Wheel::~Wheel()
{
    if (wheelEngine)
	wheelEngine->unref();
    if (zAngleFromRotEngine)
	zAngleFromRotEngine->unref();
}

////////////////////////////////////////////////////////////////////
//    Class: Arrow     - defined by origin, dirPoint, and length.
//                    - An axle at the origin,
//                      an axle at a pivoting sleeve,
//                      an inert axle at the end,
//                      and a dragger to change the length.
//
//    New fields in this subclass are:
//         dirPoint         - location of point that determines direction.
//         length           - length of arrow.
//
////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(Arrow);

void
Arrow::initClass()
{
    SO_KIT_INIT_CLASS(Arrow, SimpleLink, "SimpleLink" );
}

Arrow::Arrow()
{
    SO_KIT_CONSTRUCTOR(Arrow);

    isBuiltIn = TRUE;

    // Add the new fields.
    SO_KIT_ADD_FIELD(dirPoint,   (1,0,0));
    SO_KIT_ADD_FIELD(length,     (1.5));

    SO_KIT_ADD_CATALOG_ENTRY(originAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(dirPointAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(inertAxle, Axle, TRUE,
				topSeparator, appearance, TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(sizingDragger, SoTranslate1Dragger, TRUE,
			    topSeparator, endPointTranslateSeparator, TRUE);

    SO_KIT_INIT_INSTANCE();

    // Create the axles and the dragger
    getAnyPart("originAxle", TRUE);
    getAnyPart("dirPointAxle", TRUE);
    getAnyPart("inertAxle", TRUE);
    getAnyPart("sizingDragger", TRUE);

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( originAxle);
    makeNodeFieldStopNotifying( dirPointAxle);
    makeNodeFieldStopNotifying( inertAxle);
    makeNodeFieldStopNotifying( sizingDragger);

    // Set the default geometries
    setPartAsDefault("originTranslateGeom", "arrowOriginTranslateGeom");
    setPartAsDefault("angleRotateGeom", "arrowAngleRotateGeom");
    setPartAsDefault("endPointTranslateGeom", "arrowEndPointTranslateGeom");
    setPartAsDefault("oneDScaleGeom", "arrowOneDScaleGeom");
    setPartAsDefault("twoDScaleGeom", "arrowTwoDScaleGeom");
    setPartAsDefault("threeDScaleGeom", "arrowThreeDScaleGeom");
    setPartAsDefault("material", "arrowMaterial");

    arrowEngine = new ArrowEngine;
    arrowEngine->ref();

    setUpConnections( TRUE, TRUE );
}

void 
Arrow::sizingDraggerStartCB( void *, SoDragger *inDragger )
{
    SoTranslate1Dragger *dgr = (SoTranslate1Dragger *) inDragger;
    if (dgr->translation.isConnected() && dgr->translation.isConnectionEnabled())
	dgr->translation.enableConnection(FALSE);
}

void
Arrow::sizingDraggerCB( void *mePtr, SoDragger *inDragger )
{
    // When it moves, we copy the x-translation to our own length field.
    SoTranslate1Dragger *dgr = (SoTranslate1Dragger *) inDragger;
    SbVec3f newXl = dgr->translation.getValue();

    Arrow *myself = (Arrow *) mePtr;

    if (newXl[0] != myself->length.getValue() )
	myself->length = newXl[0];
}

void 
Arrow::sizingDraggerFinishCB( void *, SoDragger *inDragger )
{
    SoTranslate1Dragger *dgr = (SoTranslate1Dragger *) inDragger;
    if (dgr->translation.isConnected() && !dgr->translation.isConnectionEnabled())
	dgr->translation.enableConnection(TRUE);
}

void
Arrow::setZPlane(float  newZ )
{
    SbBool wasEn = dirPoint.enableNotify( FALSE );
        SimpleLink::setZPlane( newZ );
	if ( arrowEngine->origin.getValue()[2] != newZ )
	    setVec3fDisabled( arrowEngine->origin, origin.getValue() );
	if ( arrowEngine->dirPoint.getValue()[2] != newZ )
	    setVec3fDisabled( arrowEngine->dirPoint, dirPoint.getValue() );
    dirPoint.enableNotify( wasEn );

    SbVec3f dirPt = dirPoint.getValue();
    if (dirPt[2] != newZ)
        setVec3fDisabled( dirPoint, SbVec3f(dirPt[0],dirPt[1],newZ) );
}

SbBool
Arrow::undoConnections(SbBool )
{
    // Disconnect the origin and dirPoint fields from the axles.
    disconnectField( origin );
    disconnectField( dirPoint );

    Axle *itAx = (Axle *) inertAxle.getValue();
    if (itAx) disconnectField( itAx->origin );
    if (itAx) disconnectField( itAx->zRotation );

    // Remove callback from the dragger
    SoTranslate1Dragger *dgr
	= (SoTranslate1Dragger *) sizingDragger.getValue();
    if (dgr) dgr->removeStartCallback( &Arrow::sizingDraggerStartCB, this );
    if (dgr) dgr->removeValueChangedCallback( &Arrow::sizingDraggerCB, this );
    if (dgr) dgr->removeFinishCallback( &Arrow::sizingDraggerFinishCB, this );
    if (dgr) disconnectField( dgr->translation );

    // Disconnect the arrowEngine and the internal transforms.
    disconnectField( arrowEngine->origin );
    disconnectField( arrowEngine->dirPoint );
    disconnectField( arrowEngine->length );
    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;
    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator", TRUE );
	disconnectField( rot->angle );
    // Translation to EndPoint
	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
	disconnectField( trans->translation );
    // oneD scale to EndPoint
	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
	disconnectField( scale->scaleFactor );
    // twoD scale to EndPoint
	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
	disconnectField( scale->scaleFactor );
    // threeD scale to EndPoint
	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
	disconnectField( scale->scaleFactor );

    SimpleLink::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
Arrow::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    SimpleLink::setUpConnections(onOff, FALSE);

    // Replace the default geometries in sub-parts, if desiered.

    // Connect arrowEngine
    connectFromField( &origin, &arrowEngine->origin);
    connectFromField( &dirPoint, &arrowEngine->dirPoint);
    connectFromField( &length, &arrowEngine->length);
    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;
    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator",TRUE);
	if (rot->axis.getValue() != SoRotationXYZ::Z )
	    rot->axis.setValue(SoRotationXYZ::Z);
	connectFromEngine( &arrowEngine->angle, &rot->angle);
    // Translation to EndPoint
	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
	connectFromEngine( &arrowEngine->endPointOffset, &trans->translation);
    // oneD scale to EndPoint
	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
	connectFromEngine( &arrowEngine->oneDScale, &scale->scaleFactor);
    // twoD scale to EndPoint
	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
	connectFromEngine( &arrowEngine->twoDScale, &scale->scaleFactor);
    // threeD scale to EndPoint
	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
	connectFromEngine( &arrowEngine->threeDScale, &scale->scaleFactor);

    Axle *itAx = (Axle *) inertAxle.getValue();
    Axle *orAx = (Axle *) originAxle.getValue();
    Axle *dpAx = (Axle *) dirPointAxle.getValue();

    // Replace the default geometries in sub-parts, if desiered.
    if (itAx) {
	itAx->setWithLegs(FALSE);
	itAx->pinActivationDist = -1; // Serves to disable pins.
	itAx->setDraggable(FALSE);
	connectFromEngine( &arrowEngine->endPointLoc, &itAx->origin);
	connectFromEngine( &arrowEngine->angle, &itAx->zRotation);
    }

    // Connect the origin and endPoint fields from the Axles.
    if (orAx) {
	orAx->origin = origin.getValue();
	connectFromField( &orAx->origin, &origin);
    }
    if ( dpAx ) {
	dpAx->origin = dirPoint.getValue();
	connectFromField( &dpAx->origin, &dirPoint);
    }

    // Change geometry of draggers
    SoTranslate1Dragger *dgr
	= (SoTranslate1Dragger *) sizingDragger.getValue();
    if (dgr) {
	dgr->setPartAsDefault("translator","arrowLengthTranslator");
        dgr->setPartAsDefault("translatorActive","arrowLengthTranslatorActive");
	connectFromEngine( &arrowEngine->endPointOffset, &dgr->translation);
    }

    // Add the callback to the dragger
    if (dgr) dgr->addStartCallback( &Arrow::sizingDraggerStartCB, this );
    if (dgr) dgr->addValueChangedCallback( &Arrow::sizingDraggerCB, this );
    if (dgr) dgr->addFinishCallback( &Arrow::sizingDraggerFinishCB, this );

    // Connect the field sensors

    return !(connectionsSetUp = onOff);
}

void
Arrow::setDefaultOnNonWritingFields()
{
    // We turned off notification on these with enableNotify(), but
    // we need to write them. So make sure they are not set to default.
    originAxle.setDefault(FALSE);
    dirPointAxle.setDefault(FALSE);
    inertAxle.setDefault(FALSE);
    sizingDragger.setDefault(FALSE);

    Axle *inAx = (Axle *) inertAxle.getValue();
    if (inAx) {
	inAx->origin.setDefault(TRUE);
	inAx->zRotation.setDefault(TRUE);
    }

    SimpleLink::setDefaultOnNonWritingFields();
}

Arrow::~Arrow()
{
    if (arrowEngine)
	arrowEngine->unref();
}

SbPList *
Arrow::getAxlePtrs()
{
    Axle *orAx = (Axle *) originAxle.getValue();
    Axle *dpAx = (Axle *) dirPointAxle.getValue();
    Axle *itAx = (Axle *) inertAxle.getValue();

    int num = 0;
    if (orAx) num++;
    if (dpAx) num++;
    if (itAx) num++;

    SbPList *ap = SimpleLink::getAxlePtrs();
    if (ap->getLength() != num  
	|| (orAx && ap->find(orAx) == -1)
	|| (dpAx && ap->find(dpAx) == -1)
	|| (itAx && ap->find(itAx) == -1)) {
	ap->truncate(0);
	if (orAx) ap->append(orAx);
	if (dpAx) ap->append(dpAx);
	if (itAx) ap->append(itAx);
    }
    return ap;
}

////////////////////////////////////////////////////////////////////
//    Class: DoubleArrow
//
//    New nodes in this subclass are:
//      originAxle, endPointAxle
//      arrow1, arrow2
//
//    New fields in this subclass are:
//         endPoint         - location of second link's origin
//
////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(DoubleArrow);

void
DoubleArrow::initClass()
{
    SO_KIT_INIT_CLASS(DoubleArrow, LinkBase, "LinkBase" );
}

DoubleArrow::DoubleArrow()
{
    SO_KIT_CONSTRUCTOR(DoubleArrow);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(arrow1, Arrow, TRUE, topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(arrow2, Arrow, TRUE, topSeparator, , TRUE );

    // Add the new fields.
    SO_KIT_ADD_FIELD(endPoint,      (2,0,0));

    SO_KIT_INIT_INSTANCE();

    // Create the arrows
    getAnyPart("arrow1", TRUE);
    getAnyPart("arrow2", TRUE);

    // Turn off notification from the SoSFNode fields used by the nodekit.
    makeNodeFieldStopNotifying( arrow1);
    makeNodeFieldStopNotifying( arrow2);

    // Create the axles:
    set("arrow1", "origin 0 0 0");
    set("arrow1", "dirPoint 2 0 0");
    set("arrow2", "origin 2 0 0");
    set("arrow2", "dirPoint 0 0 0");

    setPartAsDefault("material", "doubleArrowMaterial"); 

    setUpConnections( TRUE, TRUE );
}

void 
DoubleArrow::setZPlane( float newZ )
{
    SbBool wasEn = endPoint.enableNotify(FALSE);
        LinkBase::setZPlane( newZ );
	Arrow *a1 = (Arrow *) arrow1.getValue();
	Arrow *a2 = (Arrow *) arrow2.getValue();
	if ( a1 ) a1->setZPlane( newZ );
	if ( a2 ) a2->setZPlane( newZ );
    endPoint.enableNotify(wasEn);

    SbVec3f end = endPoint.getValue();
    if (end[2] != newZ)
        setVec3fDisabled( endPoint, SbVec3f(end[0],end[1],newZ) );
}

void 
DoubleArrow::setWorld( WorldInfo *newWorld )
{
    Arrow *a1 = (Arrow *) arrow1.getValue();
    Arrow *a2 = (Arrow *) arrow2.getValue();
    if ( a1 ) a1->setWorld( newWorld );
    if ( a2 ) a2->setWorld( newWorld );

    LinkBase::setWorld( newWorld );
}


void 
DoubleArrow::undoAxleDependencies()
{
    Arrow *a1 = (Arrow *) arrow1.getValue();
    Arrow *a2 = (Arrow *) arrow2.getValue();
    if ( a1 ) a1->undoAxleDependencies();
    if ( a2 ) a2->undoAxleDependencies();

    LinkBase::undoAxleDependencies();
}

SbPList *
DoubleArrow::getAxlePtrs()
{
    SbPList *ap = LinkBase::getAxlePtrs();
    ap->truncate(0);

    Arrow *a1 = (Arrow *) arrow1.getValue();
    if (a1) {
        SbPList *extra = a1->getAxlePtrs();
	for (int i = 0; i < extra->getLength(); i++ )
	    ap->append( (*extra)[i] );
    }
    Arrow *a2 = (Arrow *) arrow2.getValue();
    if (a2) {
        SbPList *extra = a2->getAxlePtrs();
	for (int i = 0; i < extra->getLength(); i++ )
	    ap->append( (*extra)[i] );
    }

    return ap;
}

SbBool
DoubleArrow::undoConnections(SbBool )
{
    // Disconnect the two arrows
    Arrow *a1 = (Arrow *) getAnyPart("arrow1", TRUE);
    Arrow *a2 = (Arrow *) getAnyPart("arrow2", TRUE);
    disconnectField( a1->origin );
    disconnectField( a2->origin );
    disconnectField( a1->dirPoint );
    disconnectField( a2->dirPoint );
    disconnectField( a1->length );
    disconnectField( a2->length );

    //Disconnect fields
    disconnectField( origin );
    disconnectField( endPoint );
    disconnectField( isError );

    // Disconnect engine

    LinkBase::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
DoubleArrow::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    LinkBase::setUpConnections(onOff, FALSE);

    // Remove the dirPoint axles from the arrows. Each origin will
    // become the dirPoint of the other.
    Arrow *a1 = (Arrow *) getAnyPart("arrow1", TRUE );
    Arrow *a2 = (Arrow *) getAnyPart("arrow2", TRUE );

    Axle *a1DpAx = SO_CHECK_ANY_PART(this,"arrow1.dirPointAxle",Axle);
    if (a1DpAx) a1DpAx->setShowingParts(FALSE);
    Axle *a2DpAx = SO_CHECK_ANY_PART(this,"arrow2.dirPointAxle",Axle);
    if (a2DpAx) a2DpAx->setShowingParts(FALSE);

    // Connect our origin and dirPoint with the remaining axles.
    Axle *orAx = SO_CHECK_ANY_PART(this,"arrow1.originAxle",Axle);
    Axle *epAx = SO_CHECK_ANY_PART(this,"arrow2.originAxle",Axle);
    if (orAx) {
	orAx->origin = origin.getValue();
	connectFromField( &orAx->origin, &origin);
    }
    if (epAx) {
	epAx->origin = endPoint.getValue();
	connectFromField( &epAx->origin, &endPoint);
    }

    // Turn off the parts of the arrows we will replace.
    // Leave the sizing draggers intact, however.

    // Set the parts that we can find as resources...
    a1->setPartAsDefault("originTranslateGeom","doubleArrow1OriginTranslateGeom"); 
    a2->setPartAsDefault("originTranslateGeom","doubleArrow2OriginTranslateGeom"); 
    a1->setPartAsDefault("angleRotateGeom", "doubleArrow1AngleRotateGeom");
    a2->setPartAsDefault("angleRotateGeom", "doubleArrow2AngleRotateGeom");
    a1->setPartAsDefault("endPointTranslateGeom", "doubleArrow1EndPointTranslateGeom");
    a2->setPartAsDefault("endPointTranslateGeom", "doubleArrow2EndPointTranslateGeom");
    a1->setPartAsDefault("oneDScaleGeom", "doubleArrow1OneDScaleGeom");
    a2->setPartAsDefault("oneDScaleGeom", "doubleArrow2OneDScaleGeom");
    a1->setPartAsDefault("twoDScaleGeom", "doubleArrow1TwoDScaleGeom");
    a2->setPartAsDefault("twoDScaleGeom", "doubleArrow2TwoDScaleGeom");
    a1->setPartAsDefault("threeDScaleGeom", "doubleArrow1ThreeDScaleGeom");
    a2->setPartAsDefault("threeDScaleGeom", "doubleArrow2ThreeDScaleGeom");
    a1->setPartAsDefault("material", "doubleArrow1Material");
    a2->setPartAsDefault("material", "doubleArrow2Material");

    // Set the parts for the sizing dragers in the arrows
    a1->setPartAsDefault("sizingDragger.translator", "doubleArrow1LengthTranslator");
    a2->setPartAsDefault("sizingDragger.translator", "doubleArrow2LengthTranslator");
    a1->setPartAsDefault("sizingDragger.translatorActive", "doubleArrow1LengthTranslatorActive");
    a2->setPartAsDefault("sizingDragger.translatorActive", "doubleArrow2LengthTranslatorActive");

    connectFromField( &origin, &a1->origin);
    connectFromField( &endPoint, &a2->origin);
    connectFromField( &endPoint, &a1->dirPoint);
    connectFromField( &origin, &a2->dirPoint);

    return !(connectionsSetUp = onOff);
}
void
DoubleArrow::setDefaultOnNonWritingFields()
{
    // We turned off notification on these with enableNotify(), but
    // we need to write them. So make sure they are not set to default.
    arrow1.setDefault(FALSE);
    arrow2.setDefault(FALSE);

    Arrow *a1 = (Arrow *) getAnyPart("arrow1", TRUE );
    if (a1 ) {
	a1->isError.setDefault(TRUE);
    }
    Arrow *a2 = (Arrow *) getAnyPart("arrow2", TRUE );
    if (a2 ) {
	a2->isError.setDefault(TRUE);
    }

    LinkBase::setDefaultOnNonWritingFields();
}

void 
DoubleArrow::errorColor( SbBool useErrorColor )
{
    LinkBase::errorColor( useErrorColor );

    Arrow *a1 = (Arrow *) getAnyPart("arrow1", TRUE );
    Arrow *a2 = (Arrow *) getAnyPart("arrow2", TRUE );

    a1->isError.setValue( isError.getValue() );
    a2->isError.setValue( isError.getValue() );
}

DoubleArrow::~DoubleArrow()
{
}

////////////////////////////////////////////////////////////////////
//    Class: Bell
//
// A DoubleArrow that plays a sound file when the two arrows
// overlap
//
////////////////////////////////////////////////////////////////////
SO_KIT_SOURCE(Bell);

void
Bell::initClass()
{
    SO_KIT_INIT_CLASS(Bell, DoubleArrow, "DoubleArrow" );
}

Bell::Bell()
{
    SO_KIT_CONSTRUCTOR(Bell);

    isBuiltIn = TRUE;

    // Add the new fields.
    SO_KIT_ADD_FIELD(soundFileName,      ("sounds/bell.aiff"));
    SO_KIT_ADD_FIELD(overlapping,        (0));

    SO_KIT_INIT_INSTANCE();

    setPartAsDefault("material", "bellMaterial"); 

    bellEngine = new BellEngine;
    bellEngine->ref();

    overlappingSensor = new SoFieldSensor(&Bell::overlappingSensorCB, this );;
    overlappingSensor->setPriority(0);

    wasOverlapping = FALSE;

    setUpConnections( TRUE, TRUE );
}

void
Bell::overlappingSensorCB(  void *mePtr, SoSensor *)
{
    Bell *myself = (Bell *) mePtr;

    SbBool isIt = myself->overlapping.getValue();
    if (isIt != myself->wasOverlapping ) {

	if (isIt) {
	    char *fn = (char *) myself->soundFileName.getValue().getString();
	    if (fn != NULL) {
		Arrow *a2 = (Arrow *) myself->getAnyPart("arrow2", TRUE );
		float ln = a2->length.getValue();
		if (ln == 0.0) 
		    Sound::playOnce(Sound::BELL, fn);
		else
		    Sound::playOnce(Sound::BELL, fn, (1.0 / ln) );
	    }
	    else
		Sound::playOnce(Sound::BELL);
	}
	myself->wasOverlapping = isIt;
    }
}

SbBool
Bell::undoConnections(SbBool )
{
    // Disconnect the field sensors.
    overlappingSensor->detach();

    // Disconnect the bellEngine
    disconnectField( bellEngine->origin );
    disconnectField( bellEngine->endPoint );
    disconnectField( bellEngine->size1 );
    disconnectField( bellEngine->size2 );

    disconnectField( overlapping );

    DoubleArrow::undoConnections(FALSE);

    return !(connectionsSetUp = FALSE);
}

SbBool
Bell::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    DoubleArrow::setUpConnections(onOff, FALSE);

    // Remove the inert axles from the arrows. They won't really 
    // look right here.
    Arrow *a1 = (Arrow *) getAnyPart("arrow1", TRUE );
    Arrow *a2 = (Arrow *) getAnyPart("arrow2", TRUE );

    Axle *a1InAx = SO_CHECK_ANY_PART(this,"arrow1.inertAxle",Axle);
    if (a1InAx) a1InAx->setShowingParts(FALSE);
    Axle *a2InAx = SO_CHECK_ANY_PART(this,"arrow2.inertAxle",Axle);
    if (a2InAx) a2InAx->setShowingParts(FALSE);

    // Connect the bellengine
    connectFromField( &origin, &bellEngine->origin);
    connectFromField( &endPoint, &bellEngine->endPoint);
    connectFromField( &a1->length, &bellEngine->size1);
    connectFromField( &a2->length, &bellEngine->size2);

    connectFromEngine( &bellEngine->overlapping, &overlapping);

    // Set the parts that we can find as resources...
    a1->setPartAsDefault("originTranslateGeom","bell1OriginTranslateGeom"); 
    a2->setPartAsDefault("originTranslateGeom","bell2OriginTranslateGeom"); 
    a1->setPartAsDefault("angleRotateGeom", "bell1AngleRotateGeom");
    a2->setPartAsDefault("angleRotateGeom", "bell2AngleRotateGeom");
    a1->setPartAsDefault("endPointTranslateGeom", "bell1EndPointTranslateGeom");
    a2->setPartAsDefault("endPointTranslateGeom", "bell2EndPointTranslateGeom");
    a1->setPartAsDefault("oneDScaleGeom", "bell1OneDScaleGeom");
    a2->setPartAsDefault("oneDScaleGeom", "bell2OneDScaleGeom");
    a1->setPartAsDefault("twoDScaleGeom", "bell1TwoDScaleGeom");
    a2->setPartAsDefault("twoDScaleGeom", "bell2TwoDScaleGeom");
    a1->setPartAsDefault("threeDScaleGeom", "bell1ThreeDScaleGeom");
    a2->setPartAsDefault("threeDScaleGeom", "bell2ThreeDScaleGeom");
    a1->setPartAsDefault("material", "bell1Material");
    a2->setPartAsDefault("material", "bell2Material");

    // Set the parts for the sizing dragers in the arrows
    a1->setPartAsDefault("sizingDragger.translator", "bell1LengthTranslator");
    a2->setPartAsDefault("sizingDragger.translator", "bell2LengthTranslator");
    a1->setPartAsDefault("sizingDragger.translatorActive", "bell1LengthTranslatorActive");
    a2->setPartAsDefault("sizingDragger.translatorActive", "bell2LengthTranslatorActive");

    // Connect the field sensors
    overlappingSensor->attach(&overlapping);

    return !(connectionsSetUp = onOff);
}

Bell::~Bell()
{
    if (bellEngine)
	bellEngine->unref();
    if (overlappingSensor)
        delete overlappingSensor;
}
