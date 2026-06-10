//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.5 $
 |
 |   Description:
 |      This file contains the definition of the Straight class.
 |
 |   Classes:
 |      Straight
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _STRAIGHT_
#define  _STRAIGHT_

#include "Track.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Straight
//
//  A straight section of track
//
//////////////////////////////////////////////////////////////////////////////

class Straight : public TrackSegment
{
    SO_NODE_HEADER(Straight);

  public:
    static void		initClass();

    // Must pass length to constructor.
    Straight(float length);

    // This is used to link pieces together.  Called by Track.
    // This modifies lastPosition/lastOrientation.
    virtual void link(SbVec3f &lastPosition, SbRotation &lastOrientation,
		      TrackSegment *nextSeg, TrackSegment *prevSeg,
		      int trackID);

    // Given a starting position, a velocity (expressed in
    // track-length units), and a time delta, calculate a new position
    // on the track and the grip at that position.
    void getNewPosition(const TrackPosition &start,
			const TrackPosition &velocity,
			const SbTime &deltaTime,
			TrackPosition &result,
			float &grip);

    // Given a track position, return a world-space position:
    void getWorldPosition(const TrackPosition &position,
			  WorldPosition &result);

    // Given a track position, return the curvature or length at that position
    float getCurvature(const TrackPosition &position);
    float getLength(const TrackPosition &position);

  protected:
    Straight();
    virtual ~Straight();

    // Override read() to setup scene graph:
    virtual SbBool      readInstance(SoInput *in);

    // This is called by readInstance or the public constructor to
    // create an appropriate scene graph.
    virtual void	constructSceneGraph();

    // Length of track is stored as a field.  Default value of 1.0
    // corresponds to 100 world units long.
    SoSFFloat length;
    
    int			ID;
    TrackSegment *	next;
    TrackSegment *	prev;
    SbVec3f		startPosition;
    SbRotation  	orientation;
    
    SoTransform *transform;
  private:
    static SoSeparator *sharedStuff;
};

#endif /* _STRAIGHT_ */
