//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.18 $
 |
 |   Description:
 |      This file contains the definition of the Track
 |   and TrackSegment classes.
 |
 |   Classes:
 |      Track - The complete race track class that connects the segments
 |      TrackSegment - Base class for track sections (e.g., Curve, Straight)
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _TRACK_
#define  _TRACK_

#include <Inventor/SbLinear.h>
#include <Inventor/SbPList.h>
#include <Inventor/SbTime.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoSeparator.h>


class SoTransform;
class TrackPosition;
class WorldPosition;
class Car;

#define MAX_GRIP 1000.0
#define NUM_LANES 6
#define START_CAR_SEPARATION 0.3


// Constants for track segements
#define CURB_WIDTH      	4.0
#define GUARD_RAIL_OFFSET	8.0
#define NUM_SEGS_CIRCLE 	24.0
#define SHOULDER_WIDTH  	18.0

#define SHOULDER_COLOR		SbVec3f(0.3,  0.1,  0.0)
#define SLOT_LINE_COLOR		SbVec3f(0.0,  0.0,  0.0)
#define SLOT_LINE_COLOR_LO_RES	SbVec3f(0.2,  0.2,  0.2)
#define TRACK_COLOR		SbVec3f(0.6,  0.6,  0.6)
#define TRACK_COLOR_LO_RES	SbVec3f(0.45, 0.45, 0.42)

// Number of laps in each race.  The first lap of the track is slop to
// avoid nasty cases where the track position might go negative.
#define NUM_LAPS 5
#define STARTING_LINE_LAP 1
#define VICTORY_LAP (NUM_LAPS+STARTING_LINE_LAP+1)

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Track
//
//  This class stores a list of all track segments, and has routines
//  for dealing with the track as a whole.
//
//  A word on track space:
//  Each track segment goes from a paramaterized 0.0 to 1.0.  So, a
//  length of one lap is paramaterized from 0.0 to the number of track
//  segments+1.  Notice that a track-space distance of 1.0 might be
//  very long or very short in world space; internally, the track
//  segments compensate for this; velocities in track space are stated
//  in 'unit-length' track space; a unit-length track is 100
//  world-space units long.
//
//////////////////////////////////////////////////////////////////////////////

class Track {
 public:

    // Constructor and Destructor
    Track();
    virtual ~Track();

    // Returns the scene graph of the whole track
    virtual SoSeparator *getTrack(int whichTrack, SbBool useAbbrev);

    // Given a starting position, a velocity (expressed in
    // track-length units), and a time delta, calculate a new position
    // on the track and the grip of the track at that position.
    void getNewPosition(const TrackPosition &start,
			const TrackPosition &velocity,
			const SbTime &deltaTime,
			TrackPosition &result,
                        float &grip);

    // Given a track position, return a world-space position:
    void getWorldPosition(const TrackPosition &position,
			  WorldPosition &result);

    // Given a track position, return the curvature at that position
    float getCurvature(const TrackPosition &position);

    // Returns TRUE if a car is near the start/finish line.
    SbBool atFinishLine(const TrackPosition &position);

    // Figures out starting positions of cars.  You MUST sort the cars
    // by their distance along the track to get accurate starting
    // positions (this routine uses the order in which it is called to
    // hand out starting positions).
    void assignStartingPosition(const TrackPosition &current,
				TrackPosition &result);

    // Clear the starting position info.  Must be called before
    // first call to assignStartingPosition.
    void clearStartingPositions();
    
    // Returns the length of the track, or of a segment located at the
    // given track position.
    float getLength() { return (float)segments.getLength(); }
    float getSegmentLength(const TrackPosition &pos);

    // Add and remove car methods
    void addCar(Car *car);
    void removeCar(Car *car);

  private:
    // List of track segments...
    SoNodeList	segments;

    SbIntList startingPositions;

    // Scene graph for track
    SoSeparator *trackData;

    // Root node of cars
    SoSeparator *carData;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TrackSegment
//
//  Pure virtual class that defines the interface to different pieces
//  of track (straight, curved, etc).
//
//////////////////////////////////////////////////////////////////////////////

class TrackSegment : public SoSeparator
{
    SO_NODE_HEADER(TrackSegment);

  public:
    static void		initClass();

    TrackSegment();

    // This is used to link pieces together.  Called by Track.
    // This modifies lastPosition/lastDirection.
    virtual void link(SbVec3f &lastPosition, SbRotation &lastOrientation,
		      TrackSegment *nextSeg, TrackSegment *prevSeg,
		      int trackID) = 0;
    
    // Given a starting position, a velocity (expressed in
    // track-length units), and a time delta, calculate a new position
    // on the track.
    virtual void getNewPosition(const TrackPosition &start,
				const TrackPosition &velocity,
				const SbTime &deltaTime,
				TrackPosition &result,
				float &grip) = 0;

    // Given a track position, return a world-space position:
    virtual void getWorldPosition(const TrackPosition &position,
				  WorldPosition &result) = 0;

    // Given a track position, return the curvature or length at that position
    virtual float getCurvature(const TrackPosition &position) = 0;
    virtual float getLength(const TrackPosition &position) = 0;

  protected:
    virtual ~TrackSegment();

};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TrackPosition
//
//  Really simple structure containing a position on the track.
//
//////////////////////////////////////////////////////////////////////////////

class TrackPosition 
{
  public:
    TrackPosition()	{ along = 0.0; across = 0.0; }
    float		along;
    float		across;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: WorldPosition
//
//  Another simple class, this time containing a world-space
//  translation/rotation.  It is possible to convert from a
//  CarPosition to a WorldPosition, but not vice/versa.
//
//////////////////////////////////////////////////////////////////////////////

class WorldPosition 
{
  public:
    WorldPosition()	{ translation.setValue(0,0,0);
			  rotation.setValue(SbVec3f(0,1,0), 0);
		      }
    SbVec3f		translation;
    SbRotation		rotation;
};

#endif /* _TRACK_ */
