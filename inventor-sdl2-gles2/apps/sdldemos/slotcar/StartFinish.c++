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
 |   $Revision: 1.3 $
 |
 |   Classes:
 |	StartFinish
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "StartFinish.h"

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

#ifndef DEBUG
#define NDEBUG
#endif
#include <assert.h>

SO_NODE_SOURCE(StartFinish);

SoSeparator *StartFinish::sharedStuff;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Init StartFinish class
//

void
StartFinish::initClass()

//
////////////////////////////////////////////////////////////////////////
{
    // Init parent class
    if (TrackSegment::getClassTypeId() == SoType::badType()) {
	TrackSegment::initClass();
    }
    if (Straight::getClassTypeId() == SoType::badType()) {
	Straight::initClass();
    }

    SO_NODE_INIT_CLASS(StartFinish, Straight, "Straight");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Public constructor
//

StartFinish::StartFinish(float l) : Straight(l)

//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(StartFinish);
    
    renderCaching = SoSeparator::OFF;

    constructSceneGraph();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    StartFinish constructor; called by DB when reading.
//

StartFinish::StartFinish()

//
////////////////////////////////////////////////////////////////////////
{
    renderCaching = SoSeparator::OFF;

    SO_NODE_CONSTRUCTOR(StartFinish);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    StartFinish destructor
//

StartFinish::~StartFinish()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create the scene graph, based on the length field.
//

void
StartFinish::constructSceneGraph()
//
////////////////////////////////////////////////////////////////////////
{
    if (sharedStuff == NULL) {
	SoInput in;
	if (!in.openFile("StartFinish.iv")) {
	    fprintf(stderr, "Couldn't open StartFinish.iv\n");
	    sharedStuff = new SoSeparator;
	} else {
	    sharedStuff = SoDB::readAll(&in);
	    sharedStuff->renderCaching = SoSeparator::OFF;
	    in.closeFile();
	}
    }
    transform = new SoTransform;
    transform->scaleFactor = SbVec3f(1.0, 1.0, length.getValue());
    addChild(transform);
    addChild(sharedStuff);
}
