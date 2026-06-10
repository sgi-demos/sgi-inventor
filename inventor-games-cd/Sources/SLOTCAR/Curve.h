//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Description:
 |      A curved piece of track.
 |
 |   Classes:
 |      Curve
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _CURVE_
#define  _CURVE_

#include "Track.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Curve
//
//  A curved section of track
//
//////////////////////////////////////////////////////////////////////////////

class Curve : public TrackSegment
{
    SO_NODE_HEADER(Curve);

  public:
    // Must pass radius/angle to constructor.  See comments before
    // protected radius/angle fields for more info.
    Curve(float r, float a);

    static void		initClass();

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
    Curve();
    virtual ~Curve();

    // Override read() to setup scene graph:
    virtual SbBool      readInstance(SoInput *in);

    // This is called by readInstance or the public constructor to
    // create an appropriate scene graph.
    virtual void	constructSceneGraph();

    // Fields are protected; they must not be changed once a node is
    // read in or constructed.

    // Radius of turn.  The default value of 1.0 actually corresponds
    // to a radius of 100 world-space units.
    // Positive radii correspond to left-hand turns, negative to
    // right-hand turns; if the radius is negative, the turn angle had
    // better be negative, too!
    SoSFFloat radius;

    // Angle of turn, in fractions of 360 degrees.
    // The default value of .25 is a 90 degree turn.
    SoSFFloat angle;
    
  private:
    int			ID;
    TrackSegment *	next;
    TrackSegment *	prev;
    SbVec3f		startPosition;
    SbRotation  	orientation;
    SoTransform *	transform;
};

#endif /* _CURVE_ */
