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
 |	TableTop
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/nodes/SoArray.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>

#include "GraphPaper.h"
#include "TableTop.h"
#include "MechPiece.h"

SO_KIT_SOURCE(TableTop);

void
TableTop::initClass()
{
    SO_KIT_INIT_CLASS(TableTop, SoBaseKit, "BaseKit");
}

TableTop::TableTop()
{
    SO_KIT_CONSTRUCTOR(TableTop);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(topSeparator, SoSeparator, TRUE,
				this, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(nearLeftSep, SoSeparator, FALSE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(nearLeftDraggerRot, SoRotationXYZ, FALSE,
				nearLeftSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(nearLeftDragger, SoTranslate2Dragger, TRUE,
				nearLeftSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(farLeftSep, SoSeparator, FALSE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(farLeftDraggerRot, SoRotationXYZ, FALSE,
				farLeftSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(farLeftDragger, SoTranslate2Dragger, TRUE,
				farLeftSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(nearRightSep, SoSeparator, FALSE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(nearRightDraggerRot, SoRotationXYZ, FALSE,
				nearRightSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(nearRightDragger, SoTranslate2Dragger, TRUE,
				nearRightSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(farRightSep, SoSeparator, FALSE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(farRightDraggerRot, SoRotationXYZ, FALSE,
				farRightSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(farRightDragger, SoTranslate2Dragger, TRUE,
				farRightSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(surfaceSep, SoSeparator, FALSE,
				topSeparator, ,FALSE);

    // This will draw a graph of one color over a square of another.
    SO_KIT_ADD_CATALOG_ENTRY(graphPaper, GraphPaper, FALSE,
				surfaceSep, ,FALSE);

    // The fieldSensorCB insures that this translates the table to
    // the center of (near,far,left,right). It doesn't translate in Y.
    SO_KIT_ADD_CATALOG_ENTRY(bottomDraggerPlacer, SoTranslation, FALSE,
				surfaceSep, ,FALSE);

    // The fieldSensorCB insures that this scales a 2x2 (in x,z) square 
    // to the size of (right-left) by (near-far). It leave the y-scale alone
    SO_KIT_ADD_CATALOG_ENTRY(bottomDraggerScale, SoScale, FALSE,
				surfaceSep, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(bottomDraggerRot, SoRotationXYZ, FALSE,
				surfaceSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(bottomDragger, SoTranslate1Dragger, TRUE,
				surfaceSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(activePlaneDraggerSep, SoSeparator, FALSE,
				topSeparator, ,FALSE);
    // Places the dragger at the left edge of the tabletop.
    SO_KIT_ADD_CATALOG_ENTRY(activePlaneDraggerPlacer, SoTransform, FALSE,
				activePlaneDraggerSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(activePlaneDragger, SoTranslate1Dragger, FALSE,
				activePlaneDraggerSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(activePlaneGeomSep, SoSeparator, FALSE,
				topSeparator, ,FALSE);
    // This transform makes a 2x2 in (x,y) plane fit in left,right,bottom,top
    // It also places it in z at the active plane.
    SO_KIT_ADD_CATALOG_ENTRY(activePlaneGeomFitter, SoTransform, FALSE,
				activePlaneGeomSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(activePlaneGeom, SoSeparator, TRUE,
				activePlaneGeomSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(topSep, SoSeparator, FALSE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(topDraggerPlacer, SoTransform, FALSE,
				topSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(topDragger, SoTranslate1Dragger, TRUE,
				topSep, ,TRUE);

    SO_KIT_ADD_FIELD(activePlane, (0.0));
    SO_KIT_ADD_FIELD(left, (-5.0));
    SO_KIT_ADD_FIELD(right, (5.0));
    SO_KIT_ADD_FIELD(near, (5.0));
    SO_KIT_ADD_FIELD(far, (-5.0));
    SO_KIT_ADD_FIELD(bottom, (0.0));
    SO_KIT_ADD_FIELD(top, (10.0));

    SO_KIT_ADD_FIELD(smallestBoxMin, (-2,  0, -2));
    SO_KIT_ADD_FIELD(smallestBoxMax, ( 2,  4,  2));

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    MechPiece::makeNodeFieldStopNotifying( topSeparator);
    MechPiece::makeNodeFieldStopNotifying( nearLeftSep);
    MechPiece::makeNodeFieldStopNotifying( nearLeftDraggerRot);
    MechPiece::makeNodeFieldStopNotifying( nearLeftDragger);
    MechPiece::makeNodeFieldStopNotifying( farLeftSep);
    MechPiece::makeNodeFieldStopNotifying( farLeftDraggerRot);
    MechPiece::makeNodeFieldStopNotifying( farLeftDragger);
    MechPiece::makeNodeFieldStopNotifying( nearRightSep);
    MechPiece::makeNodeFieldStopNotifying( nearRightDraggerRot);
    MechPiece::makeNodeFieldStopNotifying( nearRightDragger);
    MechPiece::makeNodeFieldStopNotifying( farRightSep);
    MechPiece::makeNodeFieldStopNotifying( farRightDraggerRot);
    MechPiece::makeNodeFieldStopNotifying( farRightDragger);
    MechPiece::makeNodeFieldStopNotifying( surfaceSep);
    MechPiece::makeNodeFieldStopNotifying( graphPaper);  
    MechPiece::makeNodeFieldStopNotifying( bottomDraggerPlacer);
    MechPiece::makeNodeFieldStopNotifying( bottomDraggerScale);
    MechPiece::makeNodeFieldStopNotifying( bottomDraggerRot);
    MechPiece::makeNodeFieldStopNotifying( bottomDragger);
    MechPiece::makeNodeFieldStopNotifying( activePlaneDraggerSep);
    MechPiece::makeNodeFieldStopNotifying( activePlaneDraggerPlacer);
    MechPiece::makeNodeFieldStopNotifying( activePlaneDragger); 
    MechPiece::makeNodeFieldStopNotifying( activePlaneGeomSep);
    MechPiece::makeNodeFieldStopNotifying( activePlaneGeomFitter);
    MechPiece::makeNodeFieldStopNotifying( topSep);
    MechPiece::makeNodeFieldStopNotifying( topDraggerPlacer);
    MechPiece::makeNodeFieldStopNotifying( topDragger);

    // Orient the draggers correctly
    set(  "nearLeftDraggerRot", "axis X angle 1.57079");
    set(   "farLeftDraggerRot", "axis X angle 1.57079");
    set( "nearRightDraggerRot", "axis X angle 1.57079");
    set(  "farRightDraggerRot", "axis X angle 1.57079");
    set(    "bottomDraggerRot", "axis Z angle 1.57079");
    set(       "topDraggerPlacer", "rotation 0 0 1 1.57079");
    set(       "activePlaneDraggerPlacer", "rotation 0 1 0 -1.57079");

    // Get the resource for the activePlaneGeom.
    // This should be 2x2 in (x,y), so it can be scaled into place.
    setAnyPart("activePlaneGeom", SoNode::getByName("tableActivePlaneGeom"));

    // Updates the motionMatrix when the scaleFactor field is set.
    activePlaneFieldSensor = new SoFieldSensor(&TableTop::fieldSensorCB, this);
    leftFieldSensor = new SoFieldSensor(&TableTop::fieldSensorCB, this);
    rightFieldSensor = new SoFieldSensor(&TableTop::fieldSensorCB, this);
    nearFieldSensor = new SoFieldSensor(&TableTop::fieldSensorCB, this);
    farFieldSensor = new SoFieldSensor(&TableTop::fieldSensorCB, this);
    bottomFieldSensor = new SoFieldSensor(&TableTop::fieldSensorCB, this);
    topFieldSensor = new SoFieldSensor(&TableTop::fieldSensorCB, this);
    smallestBoxMinFieldSensor =new SoFieldSensor(&TableTop::fieldSensorCB,this);
    smallestBoxMaxFieldSensor =new SoFieldSensor(&TableTop::fieldSensorCB,this);

    activePlaneFieldSensor->setPriority( 0 );
    leftFieldSensor->setPriority( 0 );
    rightFieldSensor->setPriority( 0 );
    nearFieldSensor->setPriority( 0 );
    farFieldSensor->setPriority( 0 );
    bottomFieldSensor->setPriority( 0 );
    topFieldSensor->setPriority( 0 );
    smallestBoxMinFieldSensor->setPriority( 0 );
    smallestBoxMaxFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

SbBool
TableTop::setUpConnections(SbBool onOff, SbBool doItAlways)
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    SoTranslate2Dragger *nldrag, *fldrag, *nrdrag, *frdrag;
    nldrag = SO_GET_ANY_PART(this, "nearLeftDragger",  SoTranslate2Dragger);
    fldrag = SO_GET_ANY_PART(this, "farLeftDragger",  SoTranslate2Dragger);
    nrdrag = SO_GET_ANY_PART(this, "nearRightDragger", SoTranslate2Dragger);
    frdrag = SO_GET_ANY_PART(this, "farRightDragger", SoTranslate2Dragger);

    SoTranslate1Dragger *botdrag, *plndrag, *topdrag;
    botdrag = SO_GET_ANY_PART( this, "bottomDragger", SoTranslate1Dragger);
    plndrag = SO_GET_ANY_PART( this, "activePlaneDragger", SoTranslate1Dragger);
    topdrag = SO_GET_ANY_PART( this, "topDragger", SoTranslate1Dragger);

    if ( !onOff ) {

	// We disconnect BEFORE base class
	nldrag->removeValueChangedCallback( &TableTop::nearLeftVCCB, this );
	fldrag->removeValueChangedCallback( &TableTop::farLeftVCCB, this );
	nrdrag->removeValueChangedCallback( &TableTop::nearRightVCCB, this );
	frdrag->removeValueChangedCallback( &TableTop::farRightVCCB, this );
	botdrag->removeValueChangedCallback( &TableTop::bottomVCCB, this );
	plndrag->removeValueChangedCallback( &TableTop::activePlaneVCCB, this );
	topdrag->removeValueChangedCallback( &TableTop::topVCCB, this );

	activePlaneFieldSensor->detach();
	leftFieldSensor->detach();
	rightFieldSensor->detach();
	nearFieldSensor->detach();
	farFieldSensor->detach();
	bottomFieldSensor->detach();
	topFieldSensor->detach();
	smallestBoxMinFieldSensor->detach();
	smallestBoxMaxFieldSensor->detach();

        return SoBaseKit::setUpConnections( FALSE, FALSE );
    }
    else {

	// We connect AFTER base class.
	SoBaseKit::setUpConnections( TRUE, FALSE );

	// Create the draggers and change their geometries.

	SoNode *part1, *part2;
	part1 = SoNode::getByName("tableNearLeftTranslator");
	part2 = SoNode::getByName("tableNearLeftTranslatorActive");
	if (part1) nldrag->setPart("translator", part1 );
	if (part2) nldrag->setPart("translatorActive", part2);

	part1 = SoNode::getByName("tableFarLeftTranslator");
	part2 = SoNode::getByName("tableFarLeftTranslatorActive");
	if (part1) fldrag->setPart("translator", part1);
	if (part2) fldrag->setPart("translatorActive", part2);

	part1 = SoNode::getByName("tableNearRightTranslator");
	part2 = SoNode::getByName("tableNearRightTranslatorActive");
	if (part1) nrdrag->setPart("translator", part1);
	if (part2) nrdrag->setPart("translatorActive", part2);

	part1 = SoNode::getByName("tableFarRightTranslator");
	part2 = SoNode::getByName("tableFarRightTranslatorActive");
	if (part1) frdrag->setPart("translator", part1);
	if (part2) frdrag->setPart("translatorActive", part2);

	part1 = SoNode::getByName("tableBottomTranslator");
	part2 = SoNode::getByName("tableBottomTranslatorActive");
	if (part1) botdrag->setPart("translator", part1);
	if (part2) botdrag->setPart("translatorActive", part2);

	part1 = SoNode::getByName("tableActivePlaneTranslator");
	part2 = SoNode::getByName("tableActivePlaneTranslatorActive");
	if (part1) plndrag->setPart("translator", part1);
	if (part2) plndrag->setPart("translatorActive", part2);

	part1 = SoNode::getByName("tableTopTranslator");
	part2 = SoNode::getByName("tableTopTranslatorActive");
	if (part1) topdrag->setPart("translator", part1);
	if (part2) topdrag->setPart("translatorActive", part2);

	nldrag->addValueChangedCallback( &TableTop::nearLeftVCCB, this );
	fldrag->addValueChangedCallback( &TableTop::farLeftVCCB, this );
	nrdrag->addValueChangedCallback( &TableTop::nearRightVCCB, this );
	frdrag->addValueChangedCallback( &TableTop::farRightVCCB, this );
	botdrag->addValueChangedCallback( &TableTop::bottomVCCB, this );
	plndrag->addValueChangedCallback( &TableTop::activePlaneVCCB, this );
	topdrag->addValueChangedCallback( &TableTop::topVCCB, this );

	activePlaneFieldSensor->attach( &activePlane );
	leftFieldSensor->attach( &left );
	rightFieldSensor->attach( &right );
	nearFieldSensor->attach( &near );
	farFieldSensor->attach( &far );
	bottomFieldSensor->attach( &bottom );
	topFieldSensor->attach( &top );
	smallestBoxMinFieldSensor->attach( &smallestBoxMin );
	smallestBoxMaxFieldSensor->attach( &smallestBoxMax );

	// Call the fieldSensorCB. This will place all the draggers
	// according to current values (which are the defaults at this point).
	fieldSensorCB( this, NULL );
    }

    return !(connectionsSetUp = onOff);
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
TableTop::~TableTop()
//
////////////////////////////////////////////////////////////////////////
{
    if (activePlaneFieldSensor ) delete activePlaneFieldSensor;
    if (leftFieldSensor ) delete leftFieldSensor;
    if (rightFieldSensor ) delete rightFieldSensor;
    if (nearFieldSensor ) delete nearFieldSensor;
    if (farFieldSensor ) delete farFieldSensor;
    if (bottomFieldSensor ) delete bottomFieldSensor;
    if (topFieldSensor ) delete topFieldSensor;
    if (smallestBoxMinFieldSensor ) delete smallestBoxMinFieldSensor;
    if (smallestBoxMaxFieldSensor ) delete smallestBoxMaxFieldSensor;
}

void
TableTop::nearLeftVCCB( void *inTable, SoDragger *inDragger )
{
    TableTop *myself = (TableTop *) inTable;
    SbVec3f trans = ((SoTranslate2Dragger *) inDragger)->translation.getValue();

    if ( myself->near.getValue() != trans[1] )
	 myself->near = trans[1];
    if ( myself->left.getValue() != trans[0] )
	 myself->left = trans[0];
}

void
TableTop::farLeftVCCB( void *inTable, SoDragger *inDragger )
{
    TableTop *myself = (TableTop *) inTable;
    SbVec3f trans = ((SoTranslate2Dragger *) inDragger)->translation.getValue();

    if ( myself->far.getValue() != trans[1] )
	 myself->far = trans[1];
    if ( myself->left.getValue() != trans[0] )
	 myself->left = trans[0];
}

void
TableTop::nearRightVCCB( void *inTable, SoDragger *inDragger )
{
    TableTop *myself = (TableTop *) inTable;
    SbVec3f trans = ((SoTranslate2Dragger *) inDragger)->translation.getValue();

    if ( myself->near.getValue() != trans[1] )
	 myself->near = trans[1];
    if ( myself->right.getValue() != trans[0] )
	 myself->right = trans[0];
}

void
TableTop::farRightVCCB( void *inTable, SoDragger *inDragger )
{
    TableTop *myself = (TableTop *) inTable;
    SbVec3f trans = ((SoTranslate2Dragger *) inDragger)->translation.getValue();

    if ( myself->far.getValue() != trans[1] )
	 myself->far = trans[1];
    if ( myself->right.getValue() != trans[0] )
	 myself->right = trans[0];
}

void
TableTop::bottomVCCB( void *inTable, SoDragger *inDragger )
{
    TableTop *myself = (TableTop *) inTable;
    SbVec3f trans = ((SoTranslate1Dragger *) inDragger)->translation.getValue();

    if ( myself->bottom.getValue() != trans[0] )
	 myself->bottom = trans[0];
}

void
TableTop::activePlaneVCCB( void *inTable, SoDragger *inDragger )
{
    TableTop *myself = (TableTop *) inTable;
    SbVec3f trans = ((SoTranslate1Dragger *) inDragger)->translation.getValue();

    if ( myself->activePlane.getValue() != trans[0] )
	 myself->activePlane = trans[0];
}

void
TableTop::topVCCB( void *inTable, SoDragger *inDragger )
{
    TableTop *myself = (TableTop *) inTable;
    SbVec3f trans = ((SoTranslate1Dragger *) inDragger)->translation.getValue();

    if ( myself->top.getValue() != trans[0] )
	 myself->top = trans[0];
}

void
TableTop::fieldSensorCB( void *inTable, SoSensor * )
{
    TableTop *myself = (TableTop *) inTable;

    myself->constrainToLimits();

    SoTranslate2Dragger *d2;
    SoTranslate1Dragger *d1;
    SbVec3f xlate, shouldBe;
    float left, right, near, far, bottom, top, activePlane;
    left = myself->left.getValue();
    right = myself->right.getValue();
    near = myself->near.getValue();
    far = myself->far.getValue();
    bottom = myself->bottom.getValue();
    top = myself->top.getValue();
    activePlane = myself->activePlane.getValue();

    d2 = (SoTranslate2Dragger *) myself->nearLeftDragger.getValue();
    if (d2) {
	xlate = d2->translation.getValue();
	shouldBe.setValue( left, near, -bottom );
	if (xlate != shouldBe)
	    d2->translation = shouldBe;
    }

    d2 = (SoTranslate2Dragger *) myself->farLeftDragger.getValue();
    if (d2) {
	xlate = d2->translation.getValue();
	shouldBe.setValue( left, far, -bottom );
	if (xlate != shouldBe)
	    d2->translation = shouldBe;
    }

    d2 = (SoTranslate2Dragger *) myself->nearRightDragger.getValue();
    if (d2) {
	xlate = d2->translation.getValue();
	shouldBe.setValue( right, near, -bottom );
	if (xlate != shouldBe)
	    d2->translation = shouldBe;
    }

    d2 = (SoTranslate2Dragger *) myself->farRightDragger.getValue();
    if (d2) {
	xlate = d2->translation.getValue();
	shouldBe.setValue( right, far, -bottom );
	if (xlate != shouldBe)
	    d2->translation = shouldBe;
    }

    d1 = (SoTranslate1Dragger *) myself->bottomDragger.getValue();
    if (d1) {
	xlate = d1->translation.getValue();
	shouldBe.setValue( bottom, 0, 0 );
	if (xlate != shouldBe)
	    d1->translation = shouldBe;
    }

    d1 = (SoTranslate1Dragger *) myself->topDragger.getValue();
    if (d1) {
	xlate = d1->translation.getValue();
	shouldBe.setValue( top, 0, 0 );
	if (xlate != shouldBe)
	    d1->translation = shouldBe;
    }

    d1 = (SoTranslate1Dragger *) myself->activePlaneDragger.getValue();
    if (d1) {
	xlate = d1->translation.getValue();
	shouldBe.setValue( activePlane, 0, 0 );
	if (xlate != shouldBe)
	    d1->translation = shouldBe;
    }

    // The fieldSensorCB insures that this translates the table to
    // the center of (near,far,left,right). It doesn't translate in Y.
    SoTranslation *placer 
	= (SoTranslation *) myself->bottomDraggerPlacer.getValue();
    if (placer)
	placer->translation.setValue((right + left)/2.0, 0.0,(near + far)/2.0 );

    // Scales a 2x2 (in x,z) square 
    // to the size of (right-left) by (near-far). Leave the y-scale alone
    SoScale *scl = (SoScale *) myself->bottomDraggerScale.getValue();
    if (scl)
	scl->scaleFactor.setValue( (right - left)/2.0, 1.0, (near - far)/2.0 );

    // Set up parameters for graph paper.
    GraphPaper *gp = (GraphPaper *) myself->graphPaper.getValue();
    if (gp) {
	if ( gp->left.getValue() != left)
	     gp->left = left + 1;
	if ( gp->right.getValue() != right)
	     gp->right = right - 1;
	if ( gp->near.getValue() != near)
	     gp->near = near - 1;
	if ( gp->far.getValue() != far)
	     gp->far = far + 1;
	if ( gp->paperHeight.getValue() != bottom + .01)
	     gp->paperHeight = bottom + .01;
	if ( gp->lineHeight.getValue() != bottom + .03)
	     gp->lineHeight = bottom + .03;
     }

    // Move and scale the active plane geometry 
    SoTransform *fit = (SoTransform *) myself->activePlaneGeomFitter.getValue();
    if (fit) {
	fit->scaleFactor.setValue( (right - left)/2.0, (top-bottom)/2.0, 1.0);
	fit->translation.setValue( (right + left)/2.0, (top+bottom)/2.0, 
				    activePlane);
    }

    // Move the active plane dragger to the left edge.
    fit = (SoTransform *) myself->activePlaneDraggerPlacer.getValue();
    if (fit)
	fit->translation.setValue( left, bottom, 0 );

    // Move the topDragger in z to the active plane.
    // Move it in x to (right+left)/2.0
    // Scale it in y to (right-left)/2.0
    fit = (SoTransform *) myself->topDraggerPlacer.getValue();
    if (fit) {
	fit->translation.setValue( (right+left)/2.0, 0, activePlane);
	fit->scaleFactor.setValue( 1, (right-left)/2.0, 1);
    }
}

void
TableTop::constrainToLimits()
{
    float maxLeft, minRight, maxFar, minNear, maxBottom, minTop;
    getLimits(maxLeft, minRight, maxFar, minNear, maxBottom, minTop);

    if ( left.getValue() > maxLeft )
	left = maxLeft;
    if ( right.getValue() < minRight )
	right = minRight;

    if ( far.getValue() > maxFar )
	far = maxFar;
    if ( near.getValue() < minNear )
	near = minNear;

    if ( bottom.getValue() > maxBottom )
	bottom = maxBottom;
    if ( top.getValue() < minTop )
	top = minTop;
}

void
TableTop::getLimits( float &maxLeft,   float &minRight, 
		     float &maxFar,    float &minNear, 
		     float &maxBottom, float &minTop  )
{
    // Get values for constraining box.
    maxLeft    = smallestBoxMin.getValue()[0];
    minRight   = smallestBoxMax.getValue()[0];
    maxBottom  = smallestBoxMin.getValue()[1];
    minTop     = smallestBoxMax.getValue()[1];
    maxFar     = smallestBoxMin.getValue()[2];
    minNear    = smallestBoxMax.getValue()[2];
    float activeP    = activePlane.getValue();
    minNear = (activeP > minNear) ? activeP : minNear;
    maxFar  = (activeP < maxFar)  ? activeP : maxFar;
}
