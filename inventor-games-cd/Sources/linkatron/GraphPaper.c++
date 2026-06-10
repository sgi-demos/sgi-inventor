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
 |	GraphPaper
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
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodekits/SoAppearanceKit.h>

#include "GraphPaper.h"
#include "MechPiece.h"

SO_KIT_SOURCE(GraphPaper);

void
GraphPaper::initClass()
{
    SO_KIT_INIT_CLASS(GraphPaper, SoBaseKit, "BaseKit");
}

GraphPaper::GraphPaper()
{
    SO_KIT_CONSTRUCTOR(GraphPaper);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(paperSeparator, SoSeparator, TRUE,
				this, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(paperHeightPlacer, SoTranslation, FALSE,
				paperSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(paperAppearance, SoAppearanceKit, TRUE,
				paperSeparator, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(paperCoordinate3, SoCoordinate3, FALSE,
				paperSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(paperFaceSet, SoFaceSet, FALSE,
				paperSeparator, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(lineSeparator, SoSeparator, TRUE,
				this, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(lineHeightPlacer, SoTranslation, FALSE,
				lineSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(lineAppearance, SoAppearanceKit, TRUE,
				lineSeparator, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(lineNormalBinding, SoNormalBinding, FALSE,
				lineSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(lineNormal, SoNormal, FALSE,
				lineSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(lineCoordinate3, SoCoordinate3, FALSE,
				lineSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(lineLineSet, SoLineSet, FALSE,
				lineSeparator, ,FALSE);

    SO_KIT_ADD_FIELD(left, (-4.0));
    SO_KIT_ADD_FIELD(right, (4.0));
    SO_KIT_ADD_FIELD(near, (4.0));
    SO_KIT_ADD_FIELD(far, (-4.0));
    SO_KIT_ADD_FIELD(paperHeight, (0.0));
    SO_KIT_ADD_FIELD(lineHeight, (0.01));

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    MechPiece::makeNodeFieldStopNotifying( paperSeparator);
    MechPiece::makeNodeFieldStopNotifying( paperHeightPlacer);
    MechPiece::makeNodeFieldStopNotifying( paperCoordinate3);
    MechPiece::makeNodeFieldStopNotifying( paperFaceSet);
    MechPiece::makeNodeFieldStopNotifying( lineSeparator);
    MechPiece::makeNodeFieldStopNotifying( lineHeightPlacer);
    MechPiece::makeNodeFieldStopNotifying( lineNormalBinding);
    MechPiece::makeNodeFieldStopNotifying( lineNormal);
    MechPiece::makeNodeFieldStopNotifying( lineCoordinate3);
    MechPiece::makeNodeFieldStopNotifying( lineLineSet);

    // Get the appearances from the global dictionary.
    setAnyPart("paperAppearance",SoNode::getByName("graphPaperPaperAppearance"));
    setAnyPart("lineAppearance", SoNode::getByName("graphPaperLineAppearance" ));

    SoNormalBinding *nb = SO_GET_ANY_PART(this,"lineNormalBinding",SoNormalBinding);
    nb->value = SoNormalBinding::OVERALL;
    SoNormal *nn = SO_GET_ANY_PART(this,"lineNormal",SoNormal);
    nn->vector.set1Value(0,SbVec3f(0,1,0));

    // Create the field Sensors
    leftFieldSensor = new SoFieldSensor(&GraphPaper::fieldSensorCB, this);
    rightFieldSensor = new SoFieldSensor(&GraphPaper::fieldSensorCB, this);
    nearFieldSensor = new SoFieldSensor(&GraphPaper::fieldSensorCB, this);
    farFieldSensor = new SoFieldSensor(&GraphPaper::fieldSensorCB, this);
    paperHeightFieldSensor = new SoFieldSensor(&GraphPaper::fieldSensorCB,this);
    lineHeightFieldSensor = new SoFieldSensor(&GraphPaper::fieldSensorCB, this);

    leftFieldSensor->setPriority( 0 );
    rightFieldSensor->setPriority( 0 );
    nearFieldSensor->setPriority( 0 );
    farFieldSensor->setPriority( 0 );
    paperHeightFieldSensor->setPriority( 0 );
    lineHeightFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

SbBool
GraphPaper::setUpConnections(SbBool onOff, SbBool doItAlways)
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff ) {
	// We disconnect BEFORE base class
	leftFieldSensor->detach();
	rightFieldSensor->detach();
	nearFieldSensor->detach();
	farFieldSensor->detach();
	paperHeightFieldSensor->detach();
	lineHeightFieldSensor->detach();

        return SoBaseKit::setUpConnections( FALSE, FALSE );
    }
    else {

	// We connect AFTER base class.
	SoBaseKit::setUpConnections( TRUE, FALSE );

	// Call field Sensor callback to set up the translations, coordinates,
	// lineSet and FaceSet.
	fieldSensorCB( this, NULL );

	// We disconnect BEFORE base class
	leftFieldSensor->attach( &left );
	rightFieldSensor->attach( &right );
	nearFieldSensor->attach( &near );
	farFieldSensor->attach( &far);
	paperHeightFieldSensor->attach( &paperHeight );
	lineHeightFieldSensor->attach( &lineHeight );

    }

    return !(connectionsSetUp = onOff);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
GraphPaper::~GraphPaper()
//
////////////////////////////////////////////////////////////////////////
{
    if (leftFieldSensor ) delete leftFieldSensor;
    if (rightFieldSensor ) delete rightFieldSensor;
    if (nearFieldSensor ) delete nearFieldSensor;
    if (farFieldSensor ) delete farFieldSensor;
    if (paperHeightFieldSensor ) delete paperHeightFieldSensor;
    if (lineHeightFieldSensor ) delete lineHeightFieldSensor;
}

void
GraphPaper::fieldSensorCB( void *inGraphPaper, SoSensor * )
{
    GraphPaper *myself = (GraphPaper *) inGraphPaper;

    // Set the two height translations.
	SoTranslation *htNode;
	SbVec3f        htVec;

	htNode = (SoTranslation *) myself->paperHeightPlacer.getValue();
	if (htNode) {
	    htVec.setValue( 0, myself->paperHeight.getValue(), 0);
	    if (htNode->translation.getValue() != htVec )
		htNode->translation = htVec;
	}

	htNode = (SoTranslation *) myself->lineHeightPlacer.getValue();
	if (htNode) {
	    htVec.setValue( 0, myself->lineHeight.getValue(), 0);
	    if (htNode->translation.getValue() != htVec )
		htNode->translation = htVec;
	}

    // Get handy variables.
	float left = myself->left.getValue();
	float right = myself->right.getValue();
	float near = myself->near.getValue();
	float far = myself->far.getValue();

    // Set up the paper.
	SoCoordinate3 *pC3Node = (SoCoordinate3 *)myself->paperCoordinate3.getValue();
	if (pC3Node) {
	    if (pC3Node->point.getNum() != 4)
		pC3Node->point.setNum(4);
	    SbVec3f *pC3 = pC3Node->point.startEditing();
	    pC3[0].setValue(  left, 0, far );
	    pC3[1].setValue(  left, 0, near );
	    pC3[2].setValue( right, 0, near );
	    pC3[3].setValue( right, 0, far );
	    pC3Node->point.finishEditing();
	}

	SoFaceSet     *pFS = (SoFaceSet *)myself->paperFaceSet.getValue();
	if ( pFS ) {
	    if ( pFS->startIndex.getValue() != 0)
		 pFS->startIndex = 0;
	    if ( pFS->numVertices.getNum() != 1)
		 pFS->numVertices.setNum(1);
	    if ( pFS->numVertices[0] != -1)
		 pFS->numVertices.set1Value(0,-1);
	}

    // Set up the lines.
	int leftInt  = (int) (( left < 0.0) ? -floor(  -left ) : floor(left));
	int rightInt = (int) ((right < 0.0) ? -floor( -right ) : floor(right));
	int nearInt  = (int) (( near < 0.0) ? -floor(  -near ) : floor(near));
	int farInt   = (int) ((  far < 0.0) ? -floor(   -far ) : floor(far));

	int numRows = nearInt - farInt + 1;
	int numCols = rightInt - leftInt + 1;

	numRows = (numRows < 0) ? 0 : numRows;
	numCols = (numCols < 0) ? 0 : numCols;

	SoCoordinate3 *lC3Node = (SoCoordinate3 *)myself->lineCoordinate3.getValue();
	if (lC3Node) {
	    if ( lC3Node->point.getNum() != 2 * (numRows + numCols))
		 lC3Node->point.setNum( 2 * (numRows + numCols));
	    SbVec3f *lC3 = lC3Node->point.startEditing();
	    int whichCoord = 0;
	    for ( int row = farInt; row <= nearInt; row++ ) {
		lC3[whichCoord++].setValue( leftInt, 0, row ); 
		lC3[whichCoord++].setValue( rightInt, 0, row ); 
	    }

	    for ( int col = leftInt; col <= rightInt; col++ ) {
		lC3[whichCoord++].setValue( col, 0, farInt); 
		lC3[whichCoord++].setValue( col, 0, nearInt); 
	    }

	    lC3Node->point.finishEditing();
	}


	SoLineSet     *lLSNode = (SoLineSet *)myself->lineLineSet.getValue();
	if (lLSNode) {
	    if ( lLSNode->startIndex.getValue() != 0)
		 lLSNode->startIndex = 0;
	    if ( lLSNode->numVertices.getNum() != numRows + numCols )
		 lLSNode->numVertices.setNum( numRows + numCols );
	    long *lLS = lLSNode->numVertices.startEditing();
	    for ( int nv = 0; nv < lLSNode->numVertices.getNum(); nv++ )
		if (lLS[nv] != 2)
		    lLS[nv] = 2;
	    lLSNode->numVertices.finishEditing();
        }
}
