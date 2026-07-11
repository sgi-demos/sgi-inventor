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
 |	WorldInfo
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoLists.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoPickedPoint.h>

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/errors/SoDebugError.h>

#include "TableTop.h"
#include "WorldInfo.h"
#include "MechPiece.h"
#include "Axle.h"
#include "LinkClasses.h"
#include "Shadow.h"
#include "Motors.h"

WorldInfo::WorldInfo()
{
    // Make worldRoot
	worldRoot = new SoSeparator;
	worldRoot->ref();

    // Add a light
	dirLight = new SoDirectionalLight;
	worldRoot->addChild( dirLight );
	dirLight->direction = SbVec3f( 2, -1, -3);

    // Add a shapeHints node. Unless otherwise specified, 
    // assume counterclockwise ordering, convex faces, and solid objects.
	shapeHints = new SoShapeHints;
	worldRoot->addChild( shapeHints );
	shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
	shapeHints->shapeType = SoShapeHints::SOLID;
	shapeHints->faceType = SoShapeHints::CONVEX;

    // Put tableTop, piecePicker,  and sceneAndShadow under worldRoot
	tableTopSwitch  = new SoSwitch;
	worldRoot->addChild( tableTopSwitch );
	tableTopSwitch->whichChild = SO_SWITCH_ALL;
	tableTop  = new TableTop;
	tableTopSwitch->addChild( tableTop );

	piecePicker = new SoEventCallback;
	worldRoot->addChild(piecePicker);

	sceneAndShadow = new SoSeparator;
	worldRoot->addChild( sceneAndShadow );

    // Put sceneRoot and shadowKit under sceneAndShadow
	sceneRoot = new SoSeparator;
	sceneAndShadow->addChild( sceneRoot );

	shadowKit = new ShadowKit;
	sceneAndShadow->addChild( shadowKit );

    // Create the axleList and deletedPieces list.

	// List of all axles in the scene. Used to find closest axle when 
	// connecting.
	axleList = new SbPList;

	// The piece in the axleOwnerList with the same index as the axle in
	// the axleList is the piece that owns that axle.
	axleOwnerList = new SbPList;

	// Store deleted pieces so they can be un-deleted.
	deletedPieces = new SoNodeList;
	
    // Initialize the shadowKit.
        shadowKit->setPart("stuff", sceneRoot );
        shadowKit->set("shadow", "projectionPlane 0 1 0 .04");
        shadowKit->set("shadow", "direction 2 -1 -3");

    // This piece will be dragged in z whenever the activePlane moves.
    currentPiece = NULL;

    SoDragger *dg;
    // Add a callback to the activePlaneDragger that
    // makes the currentPiece stay in the active plane.
	dg = (SoDragger *) tableTop->getPart("activePlaneDragger",TRUE);
	dg->addValueChangedCallback( &WorldInfo::activePlaneDraggerCB, this );
	
    // Add a callback to the bottomDragger of the tableTop that 
    // [a] Makes the shadow stay just above the surface.
    // [b] Changes the 'floor' field of every axle in the axleList.
	dg = (SoDragger *) tableTop->getPart("bottomDragger",TRUE);
	dg->addValueChangedCallback( &WorldInfo::bottomDraggerCB, this );

   // Add callback to the piecePicker
   piecePicker->addEventCallback( SoMouseButtonEvent::getClassTypeId(),
			  mousePressCB, this );
	
    minTableSize.setBounds(-2, 0, -2, 2, 4, 2 );
    sceneBounds = minTableSize;
}

WorldInfo::~WorldInfo()
{
    if ( worldRoot ) worldRoot->unref();
    if ( axleList ) delete axleList;
    if ( axleOwnerList ) delete axleOwnerList;
    if ( deletedPieces ) delete deletedPieces;
}

void
WorldInfo::sizeTableTop( SbBool includeShadow )
{
    if (tableTop == NULL)
	return;

    SbBox3f myBox = getSceneBounds( includeShadow );

    // Put active plane within bounds.
    // This may not work if near and/or far are set wrong, so we'll do it
    // again at the end.
    if (tableTop->activePlane.getValue() < myBox.getMin()[2])
        tableTop->activePlane = myBox.getMin()[2];
    else if (tableTop->activePlane.getValue() > myBox.getMax()[2])
        tableTop->activePlane = myBox.getMax()[2];

    tableTop->left   = myBox.getMin()[0];
    tableTop->right  = myBox.getMax()[0];
    tableTop->bottom    = myBox.getMin()[1];
    tableTop->top   = myBox.getMax()[1];
    tableTop->far = myBox.getMin()[2];
    tableTop->near    = myBox.getMax()[2];

    // Put active plane within bounds.
    // Need to do this again (see above in this routine).
    if (tableTop->activePlane.getValue() < myBox.getMin()[2])
        tableTop->activePlane = myBox.getMin()[2];
    else if (tableTop->activePlane.getValue() > myBox.getMax()[2])
        tableTop->activePlane = myBox.getMax()[2];

}

void
WorldInfo::expandTableTopToFitScene( SbBool includeShadow )
{
    if (tableTop == NULL)
	return;

    SbBox3f myBox( tableTop->smallestBoxMin.getValue(),
    		   tableTop->smallestBoxMax.getValue());

    myBox.extendBy( getSceneBounds( includeShadow ) );

    if (tableTop->smallestBoxMin.getValue() != myBox.getMin() )
        tableTop->smallestBoxMin = myBox.getMin();
    if (tableTop->smallestBoxMax.getValue() != myBox.getMax() )
        tableTop->smallestBoxMax = myBox.getMax();

    // Set the sides of the plane to actually fit these boundaries.
    sizeTableTop( includeShadow );
}

void
WorldInfo::constrainTableTopToFitScene( SbBool includeShadow )
{
    if (tableTop == NULL)
	return;

    SbBox3f myBox = minTableSize;

    myBox.extendBy( getSceneBounds( includeShadow ) );

    if (tableTop->smallestBoxMin.getValue() != myBox.getMin() )
        tableTop->smallestBoxMin = myBox.getMin();
    if (tableTop->smallestBoxMax.getValue() != myBox.getMax() )
        tableTop->smallestBoxMax = myBox.getMax();

    // Set the sides of the plane to actually fit these boundaries.
    sizeTableTop( includeShadow );
}

const SbBox3f &
WorldInfo::getSceneBounds( SbBool includeShadow )
{
    if (sceneAndShadow == NULL) {
#ifdef DEBUG
	SoDebugError::post("WorldInfo::getSceneBounds", 
			   "sceneAndShadow is NULL");
#endif
	sceneBounds = minTableSize;
    }
    else {
	SoGetBoundingBoxAction bba( SbViewportRegion(0,0));
	if (includeShadow)
	    bba.apply( sceneAndShadow );
	else
	    bba.apply( sceneRoot );
        sceneBounds = bba.getBoundingBox();
	if (sceneBounds.isEmpty())
	    sceneBounds = minTableSize;
    }

    return sceneBounds;
}

// setScene also recalculates the axleList.
void 
WorldInfo::setScene( SoSeparator *newScene )
{
    if (newScene == sceneRoot)
	return;

    // Always have at least an empty separator in the scene.

    if (newScene == NULL)
	newScene = new SoSeparator;

    if (sceneRoot == NULL) {
#ifdef DEBUG
	SoDebugError::post("WorldInfo::setScene", "old sceneRoot is NULL");
#endif
	sceneAndShadow->insertChild( newScene, 0 );
    }
    else
        sceneAndShadow->replaceChild( sceneRoot, newScene );

    sceneRoot = newScene;

    if (shadowKit != NULL)
        shadowKit->setPart("stuff", newScene );

    // Finds the axles in the scene and adds them to the list.
    calculateAxleList();

    // Make the tableTop a new size!
    if ( isTableOn() )
	sizeTableTop( isShadowOn() );
}

SbBool
WorldInfo::isSceneEmpty()
{
    if (sceneRoot == NULL) {
#ifdef DEBUG
	SoDebugError::post("WorldInfo::setScene", "old sceneRoot is NULL");
#endif
	return TRUE;
    }
    else 
	return ( sceneRoot->getNumChildren() == 0);
}

void 
WorldInfo::setTableTop( TableTop *newTableTop )
{
    if ( newTableTop == tableTop)
	return;

    if ( tableTop != NULL)
	tableTopSwitch->removeChild( tableTop );

    if ( newTableTop != NULL)
	tableTopSwitch->addChild( newTableTop );

    tableTop = newTableTop;

    SoDragger *dg;
    // Add a callback to the activePlaneDragger that
    // makes the currentPiece stay in the active plane.
	dg = (SoDragger *) tableTop->getPart("activePlaneDragger",TRUE);
	dg->addValueChangedCallback( &WorldInfo::activePlaneDraggerCB, this );
	
    // Add a callback to the bottomDragger of the tableTop that 
    // [a] Makes the shadow stay just above the surface.
    // [b] Changes the 'floor' field of every axle in the axleList.
	dg = (SoDragger *) tableTop->getPart("bottomDragger",TRUE);
	dg->addValueChangedCallback( &WorldInfo::bottomDraggerCB, this );
}

void
WorldInfo::setTableOn( SbBool onOff )
{
    if (onOff) {
	tableTopSwitch->whichChild = SO_SWITCH_ALL;
	sizeTableTop( isShadowOn() );
        if (tableTop && currentPiece)
	tableTop->activePlane = currentPiece->getZPlane();
    }
    else
	tableTopSwitch->whichChild = SO_SWITCH_NONE;
}

SbBool
WorldInfo::isTableOn()
{
    if (tableTopSwitch == NULL)
	return FALSE;
    if (tableTop == NULL)
	return FALSE;
    return ( tableTopSwitch->whichChild.getValue() == SO_SWITCH_ALL );
}

void
WorldInfo::setShadowOn( SbBool onOff )
{
    if (shadowKit == NULL)
	return;

    if (onOff) {
        shadowKit->setPart("stuff", sceneRoot );
	shadowKit->show();
    }
    else {
        shadowKit->setPart("stuff", NULL );
	shadowKit->hide();
    }
    sizeTableTop( isShadowOn() );
}

SbBool
WorldInfo::isShadowOn()
{
    if (shadowKit == NULL)
	return FALSE;
    return ( shadowKit->isShown() );
}

// A callback to the activePlaneDragger of the tableTop that 
// makes the currentPiece stay in the active plane.
void
WorldInfo::activePlaneDraggerCB( void *inWorld, SoDragger *)
{
    if (inWorld == NULL)
	return;
    WorldInfo *myself = (WorldInfo *) inWorld;

    if ( myself->tableTop == NULL || myself->getCurrentPiece() == NULL)
	return;

    float activePlane = myself->tableTop->activePlane.getValue();

    myself->getCurrentPiece()->setZPlane( activePlane );
}

// A callback to the bottomDragger of the tableTop that 
// [a] Makes the shadow stay just above the surface.
// [b] Changes the 'floor' field of every axle in the axleList.
void
WorldInfo::bottomDraggerCB( void *inWorld, SoDragger *)
{
    if (inWorld == NULL)
	return;
    WorldInfo *myself = (WorldInfo *) inWorld;

    if ( myself->tableTop == NULL )
	return;

    float tableHeight = myself->tableTop->bottom.getValue();
    float floorLevel = tableHeight + 0.05;

    if ( myself->shadowKit != NULL ) {
	Shadow *sh = SO_GET_PART(myself->shadowKit,"shadow",Shadow);
	if (sh)
	    sh->projectionPlane = SbPlane( SbVec3f(0, 1, 0), floorLevel);
    }
    for (int i = 0; i < myself->axleList->getLength(); i++ )
	((Axle *) (*myself->axleList)[i])->floorHeight = floorLevel;
}

// Finds all MechPieces under scene and adds their axles to the list.
void 
WorldInfo::calculateAxleList()
{
    // First, empty the axleList
    axleList->truncate(0);
    axleOwnerList->truncate(0);

    // Search for all MechPieces in the scene.
    SoSearchAction sa;
    sa.setType( MechPiece::getClassTypeId() );
    sa.setInterest( SoSearchAction::ALL );
    sa.apply( sceneRoot );

    SoPathList  pathList = sa.getPaths();
    for (int i = 0; i < pathList.getLength(); i++ ) {
	MechPiece *tail = (MechPiece *) pathList[i]->getTail();
	tail->setWorld( this );
	if (tail->isOfType( MechPiece::getClassTypeId() ))
	    addAxlesToList( ((MechPiece *)tail));
    }

    // To make sure that every axle gets set to the proper depths, 
    // we need to do this after all axles are registered:
    for (int i = 0; i < pathList.getLength(); i++ ) {
	MechPiece *tail = (MechPiece *) pathList[i]->getTail();
	tail->setWorld( this );
    }
}

// Finds all axles in the piece and adds them to the list.
void 
WorldInfo::addAxlesToList( MechPiece *thePiece )
{
    SbPList *axlesInPiece = ((MechPiece *)thePiece)->getAxlePtrs();
    for (int j = 0; j < axlesInPiece->getLength(); j++ ) {
	Axle *ax = (Axle *) (*axlesInPiece)[j];
	ax->setWorld( this );
	axleList->append( ax );
	axleOwnerList->append( thePiece );
    }
}

// Finds all axles in the piece and removes them from the list.
void 
WorldInfo::removeAxlesFromList( MechPiece *thePiece )
{
    SbPList *axlesInPiece = ((MechPiece *)thePiece)->getAxlePtrs();
    for (int j = 0; j < axlesInPiece->getLength(); j++ ) {
	int which = axleList->find( (*axlesInPiece)[j] );
	if (which != -1 ) {
	    axleList->remove( which );
	    axleOwnerList->remove(which);
	}
    }
}

// Pieces - these may add or delete from the axleList
void 
WorldInfo::addPiece( SoNode *newPiece )
{
    if (newPiece == NULL)
	return;
    sceneRoot->addChild( newPiece );
    if (newPiece->isOfType( MechPiece::getClassTypeId() ))
	addAxlesToList( (MechPiece *) newPiece );
}

void 
WorldInfo::deletePiece( SoNode *pieceToRemove )
{
    SoSearchAction sa;
    sa.setNode( pieceToRemove );
    sa.apply( sceneRoot );
    SoPath *p = sa.getPath();
    if ( !p || p->getLength() < 2 ||  p->getTail() != pieceToRemove)
	return;
    SoNode *parent = p->getNodeFromTail(1);
    if ( ! parent->isOfType( SoGroup::getClassTypeId() ))
	return;

    if (pieceToRemove->isOfType( MechPiece::getClassTypeId() )) {
        // Undo Axle dependencies.
	((MechPiece *)pieceToRemove)->undoAxleDependencies();

        // Remove axles from the list.
	removeAxlesFromList( (MechPiece *) pieceToRemove );
    }

    // Remove piece from parent.
    ((SoGroup *)parent)->removeChild( pieceToRemove );
}

void 
WorldInfo::addNewAxle()
{
    Axle *newOne = new Axle;
    newOne->ref();

    newOne->origin = SbVec3f( 0, 1, 0 );
    newOne->setWorld( this );
    if (tableTop)
	newOne->setZPlane( tableTop->activePlane.getValue() );

    addPiece( newOne );
    setCurrentPiece( newOne );
    newOne->unrefNoDelete();
}

void 
WorldInfo::addNewRigidStrut()
{
    RigidStrut *newOne = new RigidStrut;
    newOne->ref();

    newOne->setWorld( this );
    if (tableTop)
	newOne->setZPlane( tableTop->activePlane.getValue() );

    addPiece( newOne );
    setCurrentPiece( newOne );
    newOne->unrefNoDelete();
}

void 
WorldInfo::addNewDoubleStrut()
{
    DoubleStrut *newOne = new DoubleStrut;
    newOne->ref();

    newOne->set( "strut1.endPointAxle", "origin 0 2.732 0" );
    newOne->size1 = 2;
    newOne->size2 = 2;
    newOne->set("strut1.originAxle", "origin -1.5 1 0");
    newOne->set("strut2.endPointAxle", "origin 1.5 1 0");
    newOne->setWorld( this );
    if (tableTop)
	newOne->setZPlane( tableTop->activePlane.getValue() );

    addPiece( newOne );
    setCurrentPiece( newOne );
    newOne->unrefNoDelete();
}

void 
WorldInfo::addNewDoubleArrow()
{
    DoubleArrow *newOne = new DoubleArrow;
    newOne->ref();

    newOne->set("arrow1.originAxle", "origin 0 2 0");
    newOne->set("arrow2.originAxle", "origin 3 1 0");
    newOne->set("arrow1", "length 1.5");
    newOne->set("arrow2", "length 2.5");
    newOne->setWorld( this );
    if (tableTop)
	newOne->setZPlane( tableTop->activePlane.getValue() );

    addPiece( newOne );
    setCurrentPiece( newOne );
    newOne->unrefNoDelete();
}

void 
WorldInfo::addNewBell()
{
    Bell *newOne = new Bell;
    newOne->ref();

    newOne->set("arrow1.originAxle", "origin -1 2 0");
    newOne->set("arrow2.originAxle", "origin 3 1 0");
    newOne->set("arrow1", "length 1.5");
    newOne->set("arrow2", "length 1.0");
    newOne->soundFileName = "sounds/bell.aiff";
    newOne->setWorld( this );
    if (tableTop)
	newOne->setZPlane( tableTop->activePlane.getValue() );

    addPiece( newOne );
    setCurrentPiece( newOne );
    newOne->unrefNoDelete();
}

void 
WorldInfo::addNewArrow()
{
    Arrow *newOne = new Arrow;
    newOne->ref();

    newOne->set("originAxle", "origin 0 1 0");
    newOne->set("dirPointAxle", "origin 1 2 0");
    newOne->length = 3.0;
    newOne->setWorld( this );
    if (tableTop)
	newOne->setZPlane( tableTop->activePlane.getValue() );

    addPiece( newOne );
    setCurrentPiece( newOne );
    newOne->unrefNoDelete();
}

void 
WorldInfo::addNewWheel()
{
    Wheel *newOne = new Wheel;
    newOne->ref();

    newOne->set("originAxle", "origin -1 1.5 0");
    newOne->radius = 1.0;
    newOne->zRotation = 0;
    newOne->setWorld( this );
    if (tableTop)
	newOne->setZPlane( tableTop->activePlane.getValue() );

    addPiece( newOne );
    setCurrentPiece( newOne );
    newOne->unrefNoDelete();
}

void 
WorldInfo::addNewRotorMotor()
{
    RotorMotor *newOne = new RotorMotor;
    newOne->ref();

    newOne->on = FALSE;
    newOne->set("axle", "origin 0 1 0");
    newOne->setWorld( this );
    if (tableTop)
	newOne->setZPlane( tableTop->activePlane.getValue() );

    addPiece( newOne );
    setCurrentPiece( newOne );
    newOne->unrefNoDelete();
}

// This routine is called for every mouse button event.
void
WorldInfo::mousePressCB(void *userData, SoEventCallback *eventCB)
{
    // Check for mouse button being pressed
    const SoEvent *event = eventCB->getEvent();
    if (SO_MOUSE_PRESS_EVENT(event, ANY)) {

	// Was something picked?
	const SoPickedPoint *pp = eventCB->getPickedPoint();
	if (pp == NULL || pp->getPath() == NULL) 
	    return;
	const SoPath *pPath = pp->getPath();

	// Only care if a mechPiece was picked.
	if ( ! pPath->getTail()->isOfType(MechPiece::getClassTypeId()) )
	    return;

        WorldInfo *myself = (WorldInfo *) userData;

	MechPiece *newPiece = (MechPiece *) pPath->getTail();
	if (newPiece == myself->getCurrentPiece() )
	    return;

	// Path must be under sceneRoot, but not shadowKit
	if ( ! myself->sceneRoot ||
	     ! pPath->containsNode( myself->sceneRoot ) )
	    return;
	if ( myself->shadowKit &&  
	     pPath->containsNode( myself->shadowKit ) )
	    return;

	// We've got a new piece selected!
	myself->setCurrentPiece( newPiece );

	// Do NOT set the event as handled. We're just lookin'
    } 
}

void
WorldInfo::setCurrentPiece( MechPiece *newPiece )
{
    if (currentPiece == newPiece )
	return;

    currentPiece = newPiece;
    if (tableTop && isTableOn() )
	tableTop->activePlane = newPiece->getZPlane();
}

void 
WorldInfo::deleteCurrentPiece()
{
    if (currentPiece == NULL)
	return;

    currentPiece->ref();
    deletePiece( currentPiece );
    deletedPieces->append( currentPiece );
    currentPiece->unref();
}

void 
WorldInfo::undeletePiece()
{
    int num = deletedPieces->getLength();
    if ( num <= 0)
	return;

    MechPiece *thePiece = (MechPiece *) (*deletedPieces)[num - 1];

    thePiece->ref();

    thePiece->setWorld( this );
    if (tableTop)
	thePiece->setZPlane( tableTop->activePlane.getValue() );

    addPiece( thePiece );
    setCurrentPiece( thePiece );
    thePiece->unrefNoDelete();

    deletedPieces->remove( num - 1);
}
