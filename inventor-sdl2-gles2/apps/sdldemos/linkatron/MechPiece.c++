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
 |	MechPiece
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText3.h>

#include "MechPiece.h"
#include "Axle.h"

SO_KIT_SOURCE(MechPiece);

void
MechPiece::initClass()
{
    SO_KIT_INIT_CLASS(MechPiece, SoInteractionKit, "InteractionKit");
}

MechPiece::MechPiece()
{
    SO_KIT_CONSTRUCTOR(MechPiece);

    isBuiltIn = TRUE;

    SO_KIT_ADD_FIELD(origin, ( 0, 0, 0));

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    // From SoInteractionKit class
    makeNodeFieldStopNotifying( topSeparator );
    makeNodeFieldStopNotifying( geomSeparator );

    // Upon creation, the axle does not know what world it's in
    myWorld = NULL;

    axlePtrs = new SbPList;

    renderCaching = OFF;
    boundingBoxCaching = OFF;
    renderCulling = OFF;
    pickCulling = OFF;

    SoSeparator *sep;
    sep = SO_GET_ANY_PART(this,"topSeparator",SoSeparator);
    sep->renderCaching = OFF;
    sep->boundingBoxCaching = OFF;
    sep->renderCulling = OFF;
    sep->pickCulling = OFF;

    setUpConnections( TRUE, TRUE );
}

SbBool
MechPiece::undoConnections(SbBool )
{
    // We disconnect base class LAST.
    // Usually this is BaseClass::undoConnections(FALSE);
    SoInteractionKit::setUpConnections( FALSE, FALSE );

    return !(connectionsSetUp = FALSE);
}

SbBool
MechPiece::setUpConnections(SbBool onOff, SbBool doItAlways)
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections(FALSE);

    // We connect AFTER base class.
    SoInteractionKit::setUpConnections( onOff, FALSE );

    return !(connectionsSetUp = onOff);
}

////////////////////////////////////////////////////////////////////////
//
void   
MechPiece::setDefaultOnNonWritingFields()
{
    SoInteractionKit::setDefaultOnNonWritingFields();

    renderCaching.setDefault(TRUE);
    boundingBoxCaching.setDefault(TRUE);
    renderCulling.setDefault(TRUE);
    pickCulling.setDefault(TRUE);
}

void
MechPiece::write( SoWriteAction *action )
{
    if (action->getOutput()->getStage()  == SoOutput::COUNT_REFS) {
	// Undo all the connections before counting
	setUpConnections(FALSE);
	SoInteractionKit::write( action );
    }
    else {
	// Put back all the connections afterwards.
	SoInteractionKit::write( action );
	setUpConnections(TRUE);
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
MechPiece::~MechPiece()
//
////////////////////////////////////////////////////////////////////////
{
    delete axlePtrs;
}

void 
MechPiece::setZPlane( float newZ )
{
    // Don't notify that the origin is changing.
    // We're just sliding the zPlane, that's all.
    SbBool wasEn = origin.enableNotify(FALSE);

    // Set z planes of all axles first:
    SbPList *al = getAxlePtrs();
    for (int i = 0; i < al->getLength(); i++ ) {
	if (  this != (*al)[i] )
	    ((Axle *) (*al)[i])->setZPlane( newZ );
    }
    // Set our own origin if the z value differs from the one given.
#define CLOSE_IN_Z .005
    SbVec3f v = origin.getValue();
    if ( fabs(newZ - v[2]) > CLOSE_IN_Z )
	origin = SbVec3f( v[0], v[1], newZ );
#undef CLOSE_IN_Z

    origin.enableNotify(wasEn);
}

void 
MechPiece::setWorld( WorldInfo *newWorld )
{
    SbPList *al = getAxlePtrs();
    for (int i = 0; i < al->getLength(); i++ ) {
	if (  this != (*al)[i] )
	    ((Axle *) (*al)[i])->setWorld( newWorld );
    }
    myWorld = newWorld;
}

float 
MechPiece::getZPlane()
{
    SbVec3f v = origin.getValue();
    return v[2];
}

void 
MechPiece::undoAxleDependencies()
{
    // Do axles first:
    SbPList *al = getAxlePtrs();
    for (int i = 0; i < al->getLength(); i++ ) {
	if (  this != (*al)[i] )
	    ((Axle *) (*al)[i])->undoAxleDependencies();
    }
}

SbPList *
MechPiece::getAxlePtrs()
{
    return axlePtrs;
}

void 
MechPiece::connectFromField( SoField *fromField, SoField *toField )
{
    SoField *testF;
    if ( toField->getConnectedField( testF ) && testF == fromField ) {
    }
    else {
	toField->connectFrom( fromField );
    }
    if ( !toField->isConnectionEnabled() )
	toField->enableConnection(TRUE);
}

void 
MechPiece::connectFromEngine( SoEngineOutput *fromEngine, SoField *toField )
{
    SoEngineOutput *testE;
    if ( toField->getConnectedEngine( testE ) && testE == fromEngine ) {
    }
    else {
        toField->connectFrom( fromEngine );
    }
    if ( !toField->isConnectionEnabled() )
	toField->enableConnection(TRUE);
}

void 
MechPiece::disconnectField( SoField &theField, SoField *ifConnectedFrom )
{
    if (ifConnectedFrom == NULL)
	theField.disconnect();
    else {
	SoField *testF;
	if (theField.getConnectedField( testF ) && testF == ifConnectedFrom )
	    theField.disconnect();
    }
}

void
MechPiece::makeNodeFieldStopNotifying( SoSFNode &theField )
{
    // Nodekits store their part-pointers in SoSFNode fields.
    // Unfortunately, this causes much undesired notification.
    // What we want to do is:
    // theField.enableNotify(FALSE);
    // However, due to a bug in SoField::notify(), the flag set by this
    // method has no effect. But calling enableConnection(FALSE)
    // will have the desired effect. Unfortunately, you can't call that
    // method unless the field is connected. So we connect, disable, then
    // disconnect.

    theField.enableNotify(FALSE);

    // Just connect the field from itself temporarily. This should not 
    // screw us up.
    theField.connectFrom(&theField);
    theField.enableConnection(FALSE);
    theField.disconnect();

    theField.setDefault(TRUE);
}

// This disables notification, sets the field, then returns 
// notification to original state.
void 
MechPiece::setVec3fDisabled( SoSFVec3f &theField, const SbVec3f &theVal )
{
    SbBool wasEn = theField.enableNotify(FALSE);
    theField = theVal;
    theField.enableNotify(wasEn);
}
