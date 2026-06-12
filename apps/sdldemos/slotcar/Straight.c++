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
 |   $Revision: 1.13 $
 |
 |   Classes:
 |	Straight
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "Straight.h"

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

SO_NODE_SOURCE(Straight);

SoSeparator *Straight::sharedStuff;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Init Straight class
//

void
Straight::initClass()

//
////////////////////////////////////////////////////////////////////////
{
    // Init parent class
    if (TrackSegment::getClassTypeId() == SoType::badType()) {
	TrackSegment::initClass();
    }

    SO_NODE_INIT_CLASS(Straight, TrackSegment, "TrackSegment");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Public constructor
//

Straight::Straight(float l)

//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(Straight);
    
    SO_NODE_ADD_FIELD(length, (1.0));

    ID = -1;
    next = NULL;

    if (l != 1.0) length = l;

    renderCaching = SoSeparator::OFF;

    constructSceneGraph();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Straight constructor; called by DB when reading.
//

Straight::Straight()

//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(Straight);
    
    SO_NODE_ADD_FIELD(length, (1.0));

    ID = -1;
    next = NULL;

    renderCaching = SoSeparator::OFF;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Straight destructor
//

Straight::~Straight()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override readInstance to also setup scene graph:
//

SbBool
Straight::readInstance(SoInput *in, unsigned short flags)

//
////////////////////////////////////////////////////////////////////////
{
    // Call base class:
    SbBool result = TrackSegment::readInstance(in, flags);
    constructSceneGraph();
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create the scene graph, based on the length field.
//

void
Straight::constructSceneGraph()
//
////////////////////////////////////////////////////////////////////////
{
    if (sharedStuff == NULL) {
	SoInput in;
	if (!in.openFile("Straight.iv")) {
	    fprintf(stderr, "Couldn't open Straight.iv\n");
	    sharedStuff = new SoSeparator;
	} else {
	    sharedStuff = SoDB::readAll(&in);
	    sharedStuff->renderCaching = SoSeparator::OFF;
	    in.closeFile();
	}
    }
    transform = new SoTransform;
    transform->scaleFactor = SbVec3f(1.0, 1.0, length.getValue());
    insertChild(transform, 0);
    addChild(sharedStuff);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Link a straight piece onto the end of previous piece
//

void
Straight::link(SbVec3f &lastPosition, SbRotation &lastOrientation,
	       TrackSegment *nextSeg, TrackSegment *prevSeg,
	       int segmentID)

//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (next != NULL) {
	SoDebugError::post("Straight::link", "May not be multiply "
			   "instanced in a track");
	return;
    }
#endif

    ID = segmentID;
    next = nextSeg;
    prev = prevSeg;
    startPosition = lastPosition;
    orientation = lastOrientation;
    
    transform->translation = startPosition;
    transform->rotation = orientation;

    // Update lastPosition to be end of track:
    SbVec3f end(0, 0, -100.0*length.getValue());
    
    // rotate end based on orientation:
    SbVec3f xformedEnd;
    orientation.multVec(end, xformedEnd);

    // And add on to lastPosition:
    lastPosition += xformedEnd;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Move something down the track.
//

void
Straight::getNewPosition(const TrackPosition &start,
			 const TrackPosition &velocity,
			 const SbTime &deltaTime,
			 TrackPosition &result,
			 float &grip)

//
////////////////////////////////////////////////////////////////////////
{
    float t = deltaTime.getValue();

    // Figure out how far down the track we'll travel:
    float distAlong = velocity.along * t / length.getValue(); 
    float distAcross = velocity.across * t;
    
    // Break up start position into integer/fraction parts:
    int seg = (int)(start.along);
    float frac = start.along - seg;

    // If we go off the end of this track segment:
    if (frac + distAlong > 1.0) {
	// Set result to the very end of this track, let next track do
	// the rest:
	float fractionThisTrack = (1.0 - frac) / distAlong;
	
	TrackPosition newStart;
	newStart.along = start.along + distAlong * fractionThisTrack;
	newStart.across = start.across + distAcross * fractionThisTrack;
	result.along = newStart.along;
	result.across = newStart.across;

	SbTime newTime = deltaTime*(1.0-fractionThisTrack);

	next->getNewPosition(newStart, velocity, newTime, result, grip);

    }
    else if (frac + distAlong < 0.0) {
	// Set result to the beginning of this track, let prev track do
	// the rest:
	float fractionThisTrack = frac / -distAlong;
	
	TrackPosition newStart;
	newStart.along = start.along + distAlong * fractionThisTrack-.01;
	newStart.across = start.across + distAcross * fractionThisTrack;
	result.along = newStart.along;
	result.across = newStart.across;

	SbTime newTime = deltaTime*(1.0-fractionThisTrack);

	prev->getNewPosition(newStart, velocity, newTime, result, grip);

    } else {
	result.along = start.along + distAlong;
	result.across = start.across + distAcross;

	// Don't need to do anything with grip; it is initialized to
	// MAX_GRIP by the Track class.
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translate from track coordinates to world coordinates.
//

void
Straight::getWorldPosition(const TrackPosition &position,
			   WorldPosition &result)

//
////////////////////////////////////////////////////////////////////////
{
    // Break up start position into integer/fraction parts:
    int seg = (int)(position.along);
    float frac = position.along - seg;

    // Position relative to start of this track:
    SbVec3f p(position.across*10.0 - (NUM_LANES*5.0-5.0), 0.0,
	      -(frac*100.0*length.getValue()));

    // rotate/translate p into world space:
    SbVec3f xformedP;
    orientation.multVec(p, xformedP);

    result.translation = xformedP + startPosition;
    result.rotation = orientation;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a position on the track, return the curvature of the track
//    at that position.
//

float
Straight::getCurvature(const TrackPosition &)

//
////////////////////////////////////////////////////////////////////////
{
    return MAX_GRIP;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a position on the track, return the length of the track
//    at that position.
//

float
Straight::getLength(const TrackPosition &)

//
////////////////////////////////////////////////////////////////////////
{
    return length.getValue();
}
