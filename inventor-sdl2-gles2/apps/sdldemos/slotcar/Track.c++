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
 |   $Revision: 1.18 $
 |
 |   Classes:
 |	Track, TrackSegment
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "Car.h"
#include "Track.h"

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/misc/SoChildList.h>

#ifndef DEBUG
#define NDEBUG
#endif
#include <assert.h>

#define CAR_OFFSET 0.01

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
Track::Track()

//
////////////////////////////////////////////////////////////////////////
{
    trackData = NULL;
    clearStartingPositions();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
Track::~Track()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Read in the track, and return a scene graph representing it
//

SoSeparator *
Track::getTrack(int whichTrack, SbBool useAbbrev)
//
////////////////////////////////////////////////////////////////////////
{
    // If we've already read in, just return geometry:
    if (trackData != NULL) return trackData;

    // Read the track scenery geometry from a file and add to the scene
    SoInput myInput;
    if (whichTrack) {
        if (useAbbrev) {
            if (!myInput.openFile("ShortTrackAbbrev.iv")) 
                exit (1);
        }
        else {
            if (!myInput.openFile("ShortTrack.iv")) 
                exit (1);
        }
    }
    else {
        if (useAbbrev) {
            if (!myInput.openFile("LongTrackAbbrev.iv")) 
                exit (1);
        }
        else {
            if (!myInput.openFile("LongTrack.iv")) 
                exit (1);
        }
    }
    trackData = SoDB::readAll(&myInput);
    if (trackData == NULL) 
        exit (1);

    trackData->ref();

    // Find the pieces of track in the scene read in:

    // WARNING!  There is an incredibly subtle bug that happens here
    // if we declare the SearchAction on the stack instead of creating
    // it with new.  If declared on the stack, the
    // trackData->unrefNoDelete() will happen before the search action
    // is destroyed; when the search action is destroyed (at the end
    // of this routine), trackData's reference count will go to zero
    // and it will be deleted.
    // Very insidious, indeed.
    SoSearchAction *sa = new SoSearchAction;
    sa->setInterest(SoSearchAction::ALL);
    sa->setType(TrackSegment::getClassTypeId());
    sa->apply(trackData);
    
    const SoPathList &paths = sa->getPaths();
    int i;	// used after the loop (pre-ANSI scoping)
    for (i = 0; i < paths.getLength(); i++) {
	segments.append(paths[i]->getTail());
    }

    // Link up the track:
    int numSegs = segments.getLength();
    SbVec3f segPosition(0,0,0);
    SbRotation segOrientation(SbVec3f(0,1,0),0);
    for (i = 0; i < numSegs; i++) {
	TrackSegment *seg = (TrackSegment *)segments[i];
	seg->link(segPosition, segOrientation,
		  (TrackSegment *)segments[(i+1)%numSegs], 
		  (TrackSegment *)segments[(i-1+numSegs)%numSegs],
		  i);
    }

    delete sa;


    // Create root of all cars and properties for all cars
    carData = new SoSeparator;
    trackData->addChild( carData );
    SoLightModel *carLm = new SoLightModel;
    carLm->model.setValue( SoLightModel::PHONG );
    carData->addChild( carLm );


    trackData->unrefNoDelete();

    return trackData;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Move something down the track:
//

void
Track::getNewPosition(const TrackPosition &start,
		      const TrackPosition &velocity,
		      const SbTime &deltaTime,
		      TrackPosition &result,
                      float &grip)
//
////////////////////////////////////////////////////////////////////////
{
    int numSegments = segments.getLength();
    int startID = ((int)start.along)%numSegments;
    
    TrackSegment *startSegment = (TrackSegment *)segments[startID];
    
    result.along = start.along;
    result.across = start.across;

    // Initialize grip to a large value; the segments that we travel
    // across will reset it if their grip is smaller:
    grip = MAX_GRIP;
    startSegment->getNewPosition(start, velocity, deltaTime, result, grip);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a position on the track, figure out the world space
//    orientation/translation.
//

void
Track::getWorldPosition(const TrackPosition &position,
			  WorldPosition &result)
//
////////////////////////////////////////////////////////////////////////
{
    int numSegments = segments.getLength();
    int startID = ((int)position.along)%numSegments;
    
    TrackSegment *startSegment = (TrackSegment *)segments[startID];
    
    startSegment->getWorldPosition(position, result);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a position on the track, figure out the curvature of the 
//    track at that position.
//

float
Track::getCurvature(const TrackPosition &position)
//
////////////////////////////////////////////////////////////////////////
{
    int numSegments = segments.getLength();
    int startID = ((int)position.along)%numSegments;
    
    TrackSegment *startSegment = (TrackSegment *)segments[startID];
    
    return startSegment->getCurvature(position);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a position on the track, figure out the length of the 
//    segment of track at that position.
//

float
Track::getSegmentLength(const TrackPosition &position)
//
////////////////////////////////////////////////////////////////////////
{
    int numSegments = segments.getLength();
    int startID = ((int)position.along)%numSegments;
    
    TrackSegment *startSegment = (TrackSegment *)segments[startID];
    
    return startSegment->getLength(position);
}

/////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out if a car is sitting at/near the start/finish line.
//

SbBool
Track::atFinishLine(const TrackPosition &position)
//
////////////////////////////////////////////////////////////////////////
{
    if (position.along < getLength()*STARTING_LINE_LAP)
        return FALSE;

    int lane = (int)position.across;
    assert(startingPositions.getLength() == NUM_LANES);

    // Just in case we're crashing...
    if (lane < 0) lane = 0;
    if (lane >= NUM_LANES) lane = NUM_LANES-1;

    float p = fmod(position.along, getLength());

    // Figure out if we are within 1 unit of possible starting
    // position:
    float sp = (float)(getLength()) - 
	(float)(startingPositions[lane]*START_CAR_SEPARATION);
    
    if ((p < sp) && (p > (sp-1.0))) return TRUE;

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assign a starting position.  Depends on positions already
//    assigned.
//

void
Track::assignStartingPosition(const TrackPosition &current,
			      TrackPosition &result)
//
////////////////////////////////////////////////////////////////////////
{
    int lane = (int)current.across;

    // Just in case we're crashing...
    if (lane < 0) lane = 0;
    if (lane >= NUM_LANES) lane = NUM_LANES-1;
    
    result.across = lane;
    result.along = (float)(segments.getLength()*(STARTING_LINE_LAP+1))
        - CAR_OFFSET - (float)(startingPositions[lane]*START_CAR_SEPARATION);

    ++startingPositions[lane];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Clear out the starting positions.
//

void
Track::clearStartingPositions()
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < NUM_LANES; i++) {
	startingPositions[i] = 0;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add a car to the track.
//

void
Track::addCar( Car *car )
//
////////////////////////////////////////////////////////////////////////
{
    carData->addChild( car->getCarRoot() );
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove a car from the track.
//

void
Track::removeCar( Car *car )
//
////////////////////////////////////////////////////////////////////////
{
    carData->removeChild( car->getCarRoot() );
}




//------------------------- TrackSegment ---------------------------

SO_NODE_ABSTRACT_SOURCE(TrackSegment);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Init TrackSegment class
//

void
TrackSegment::initClass()

//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_INIT_ABSTRACT_CLASS(TrackSegment, SoSeparator,
				"Separator");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    TrackSegment constructor
//

TrackSegment::TrackSegment()

//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(TrackSegment);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    TrackSegment destructor
//

TrackSegment::~TrackSegment()

//
////////////////////////////////////////////////////////////////////////
{
}
