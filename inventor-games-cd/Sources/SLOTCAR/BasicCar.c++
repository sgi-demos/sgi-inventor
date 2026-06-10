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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	BasicCar
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbColor.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include "BasicCar.h"

#include <math.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//

BasicCar::BasicCar()
//
////////////////////////////////////////////////////////////////////////
{
    buildCar();
    carGraph->ref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

BasicCar::~BasicCar()
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
BasicCar::getCar()
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
BasicCar::buildCar()

//
////////////////////////////////////////////////////////////////////////
{
    // This car will consist of a colored cube.
    carGraph = new SoSeparator;

    transform = new SoTransform;

    SoTranslation *xl = new SoTranslation;
    SoMaterial  *mtl = new SoMaterial;
    SoCube      *cube = new SoCube;

    // Pick a random color for this car:
    mtl->diffuseColor.setValue(drand48(), drand48(), drand48());

    cube->height = CAR_HEIGHT;
    cube->depth  = CAR_LENGTH;
    cube->width  = CAR_WIDTH;
    xl->translation.setValue(SbVec3f(0.0, CAR_HEIGHT/2 + 1.0, 0.0));

    carGraph->addChild(transform);
    carGraph->addChild(xl);
    carGraph->addChild(mtl);
    carGraph->addChild(cube);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Update the car's position, given the new simulation time and a
//    pointer to the track.
//

void
BasicCar::positionCar(const WorldPosition &position)
//
////////////////////////////////////////////////////////////////////////
{
    transform->translation = position.translation;
    transform->rotation = position.rotation;
}

