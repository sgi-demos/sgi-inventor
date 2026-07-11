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
 * Copyright (C) 1992   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Shadow.c++
 |
 |   Author(s): Howard Look, Paul Isaacs
 |
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMatrixTransform.h>

#include <Inventor/errors/SoDebugError.h>

#include "Shadow.h"
#include "MechPiece.h"

SO_NODE_SOURCE(Shadow);

void
Shadow::initClass()
{
    SO_NODE_INIT_CLASS(Shadow, SoMatrixTransform, "MatrixTransform" );
}

Shadow::Shadow()
{
    SO_NODE_CONSTRUCTOR(Shadow);
    
    SO_NODE_ADD_FIELD(direction, (-1,-1,-1));
    SO_NODE_ADD_FIELD(projectionPlane, (SbPlane(SbVec3f(0, 1, 0), 0)));

    // Sensor to find out when our fields change
    directionFieldSensor = new SoFieldSensor( &Shadow::fieldSensorCB, this );
    directionFieldSensor->setPriority(0);
    directionFieldSensor->attach( &direction );

    planeFieldSensor = new SoFieldSensor( &Shadow::fieldSensorCB, this );
    planeFieldSensor->setPriority(0);
    planeFieldSensor->attach( &projectionPlane );

    calculateXform();
}

Shadow::~Shadow()
{
    delete directionFieldSensor;
    delete planeFieldSensor;
}

void
Shadow::fieldSensorCB(void *userData, SoSensor * )
{
    ((Shadow *) userData)->calculateXform();
}

//
// Icky math that can only be understood by seeing the derivation
// in my Yellow Book.
// ??? Make a Frame doc describing this.
//

void
Shadow::calculateXform()
{
    // Light Direction
    SbVec3f D = direction.getValue();

    // Plane normal and distance from origin
    const SbVec3f &N = projectionPlane.getValue().getNormal();
    float d = projectionPlane.getValue().getDistanceFromOrigin();

    // Check to make sure the light direction isn't perpendicular (dot = 0.0)
    // or below the projection plane
    float NdotD = N.dot(D);

    if (NdotD < 0) {
	D *= -1.0;
	NdotD *= -1.0;
    }

    if (NdotD < 0.03) {
#ifdef DEBUG
	SoDebugError::post("Shadow::calculateXform",
			   "direction and plane are perpendicular");
#endif
	SbMatrix answer;
	answer.setScale( .01 );
        matrix = answer;
	return;
    }

//??? Fudge to make this work
    float factor = 1 / NdotD;
    D *= factor;
    NdotD *= factor;
//??? Fudge to make this work

//???printf("dot = %f, angle = %f\n", NdotD, facos(NdotD));

    SbMatrix M;

    for (int r = 0; r <= 3; r++)
	for (int c = 0; c <= 2; c++) {
	    float val;

	    if (r == 3)
		val = d * D[c];
	    else
		val = - (D[c] * N[r]);

	    if (r == c)
		val += NdotD;

	    M[r][c] = val;
	}

    M[0][3] = M[1][3] = M[2][3] = 0;
    M[3][3] = NdotD;

    matrix.setValue( M );
}



SO_KIT_SOURCE(ShadowKit);

void
ShadowKit::initClass()
{
    SO_KIT_INIT_CLASS(ShadowKit, SoBaseKit, "BaseKit" );
}

ShadowKit::ShadowKit()
{
    SO_KIT_CONSTRUCTOR(ShadowKit);
    
    SO_KIT_ADD_CATALOG_ENTRY(topSwitch,SoSwitch,FALSE,this, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(shadow, Shadow, FALSE,
			     topSwitch, , TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(separator, SoSeparator, TRUE,
			     topSwitch, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(lightModel, SoLightModel, FALSE,
			     separator, , FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(material, SoMaterial, FALSE,
			     separator, , TRUE);
    SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(stuff, SoNode, SoGroup, FALSE,
			     separator, , TRUE);

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    MechPiece::makeNodeFieldStopNotifying( topSwitch);
    MechPiece::makeNodeFieldStopNotifying( shadow);
    MechPiece::makeNodeFieldStopNotifying( separator);
    MechPiece::makeNodeFieldStopNotifying( lightModel);
    MechPiece::makeNodeFieldStopNotifying( material);
    MechPiece::makeNodeFieldStopNotifying( stuff);

    Shadow *shad = (Shadow *) getPart("shadow", TRUE);
    shad->direction = SbVec3f(-1,-1,-1);
    shad->projectionPlane = SbPlane(SbVec3f(0, 1, 0), 0);

    SoMaterial *mtl = (SoMaterial *) getPart("material", TRUE);
    mtl->diffuseColor = SbVec3f( .1, .1, .1);
    mtl->transparency = 0;
    mtl->setOverride(TRUE);

    SoLightModel *lm = (SoLightModel *) getAnyPart("lightModel", TRUE);
    lm->model = SoLightModel::BASE_COLOR;
    lm->setOverride(TRUE);

    show();
}

ShadowKit::~ShadowKit()
{
}

SbBool 
ShadowKit::isShown()
{
    SoSwitch *sw = (SoSwitch *) getAnyPart("topSwitch", FALSE); 
    if (sw && sw->whichChild.getValue() == SO_SWITCH_ALL)
	return TRUE;
    else
	return FALSE;
}

void 
ShadowKit::show()
{
    SoSwitch *sw = (SoSwitch *) getAnyPart("topSwitch", TRUE); 
    if (sw && sw->whichChild.getValue() != SO_SWITCH_ALL)
	sw->whichChild = SO_SWITCH_ALL;
}

void 
ShadowKit::hide()
{
    SoSwitch *sw = (SoSwitch *) getAnyPart("topSwitch", FALSE); 
    if (sw && sw->whichChild.getValue() != SO_SWITCH_NONE)
	sw->whichChild = SO_SWITCH_NONE;
}
