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
 * Copyright (C) 1990,91,92,93,94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.11 $
 |
 |   Classes:
 |	RaceCar
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbColor.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include "RaceCar.h"

#include <math.h>

#define SCREECH_PER_FRAME 5.0

//
// List of car geometries.
//
static SoNodeList carList;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//

RaceCar::RaceCar(const CarPlayer &player, const SbString &name,
		 SbBool _controlled) :
	Car(player, name, _controlled)
//
////////////////////////////////////////////////////////////////////////
{
    if (carList.getLength() == 0) {
	// Read in the scene graphs for the 8 different cars used in this
	// game.  Keep them on a nodelist from which cars will instance.
	for (int i=0; i<8; i++) {
	    char fname[16];
	    SoInput reader;

	    sprintf(fname, "car%d.iv", i+1);
	    reader.openFile(fname);
	    SoNode *carNode = SoDB::readAll(&reader);
	    reader.closeFile();
	    if (carNode == NULL) {
		fprintf(stderr, "Couldn't read car geometry\n");
		exit(1);
	    }
	    carList.append(carNode);
	}
    }

    buildCar((SoSeparator *)carList[(getId().getCarNum()-1)%8]);
    screechAmount = SCREECH_CUTOFF;
    screechDiff   = 0.0;
    screechSign   = 1.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

RaceCar::~RaceCar()
//
////////////////////////////////////////////////////////////////////////
{
    carGraph->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return scene graph representing car
//

SoSeparator *
RaceCar::getCarRoot()
//
////////////////////////////////////////////////////////////////////////
{
    return carGraph;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Build a scene graph representing the car.
//

void
RaceCar::buildCar(SoSeparator *geom)

//
////////////////////////////////////////////////////////////////////////
{
    carGeom = geom;

    // Create the scene graph for the car
    SoTranslation *annotXform = new SoTranslation;
    SoBaseColor   *annotColor = new SoBaseColor;
    SoFont        *annotFont  = new SoFont;
    SoText2       *annotText  = new SoText2;
    carGraph   = new SoSeparator;
    annotation = new SoSeparator;
    transform  = new SoTransform;
    screechTransform = new SoTransform;
    annotXform->translation.setValue(SbVec3f(0.0, 8.0, 0.0));
    annotColor->rgb.setValue(SbColor(1.0, 0.0, 0.0));
    annotFont->size = 18.0;
    annotText->string.setValue(carName);
    annotText->justification = SoText2::CENTER;
    annotation->addChild(annotXform);
    annotation->addChild(annotColor);
    annotation->addChild(annotFont);
    annotation->addChild(annotText);
    carGraph->addChild(transform);
    carGraph->addChild(screechTransform);
    carGraph->addChild(carGeom);
    carGraph->ref();
    annotation->ref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add the annotation text above the car.
//

void
RaceCar::addAnnotation()

//
////////////////////////////////////////////////////////////////////////
{
    carGraph->insertChild(annotation, 1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove the annotation text above the car.
//

void
RaceCar::removeAnnotation()

//
////////////////////////////////////////////////////////////////////////
{
    carGraph->removeChild(annotation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Update the car's position, given the new simulation time and a
//    pointer to the track.
//

void
RaceCar::positionCar(const WorldPosition &position)
//
////////////////////////////////////////////////////////////////////////
{
    transform->translation = position.translation;
    transform->rotation = position.rotation;

    // Tilt the car over based on the screech amount.  To keep the
    // tilt from happening instantaneously, move it gradually.

    if (fabs(screechDiff) < SCREECH_PER_FRAME) {
        screechAmount += screechDiff;
        screechDiff = 0.0;
    }
    else {
        if (screechDiff > 0.0) {
            screechDiff -= SCREECH_PER_FRAME;
            screechAmount += SCREECH_PER_FRAME;
        }
        else {
            screechDiff += SCREECH_PER_FRAME;
            screechAmount -= SCREECH_PER_FRAME;
        }
    }

    float screechAngle;
    if (!getIsCrashing()) 
        screechAngle = -screechSign *
                  (screechAmount-SCREECH_CUTOFF) * M_PI_2/SCREECH_CUTOFF;
    else
        screechAngle = screechSign * M_PI_2;
    screechTransform->rotation.setValue(SbVec3f(0, 0, 1), screechAngle);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Screech the car by the given amount.
//

void
RaceCar::setScreechAmount(float _screech, float curvature)
//
////////////////////////////////////////////////////////////////////////
{
    float newScreech = (_screech < SCREECH_CUTOFF) ? SCREECH_CUTOFF : _screech;
    if (!getIsCrashing())
        newScreech -= (newScreech-SCREECH_CUTOFF) * 0.7;
    if (newScreech == screechAmount)
        return;

    screechDiff   = newScreech - screechAmount;
    screechSign   = (curvature > 0.0) ? -1.0 : 1.0;

    // Depending on the curvature, set the rotation center to be on one
    // side of the car or another.
    if (curvature < 0.0) {
        screechTransform->center.setValue(-CAR_WIDTH*0.5, 0.0, 0.0);
    }
    else if (curvature > 0.0) {
        screechTransform->center.setValue(CAR_WIDTH*0.5, 0.0, 0.0);
    }
}
