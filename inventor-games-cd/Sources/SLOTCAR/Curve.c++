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
 |   $Revision: 1.20 $
 |
 |   Classes:
 |	Curve
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "Curve.h"
#include "LODD.h"

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoInfo.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTriangleStripSet.h>

#ifndef DEBUG
#define NDEBUG
#endif
#include <assert.h>


SO_NODE_SOURCE(Curve);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Init Curve class
//

void
Curve::initClass()

//
////////////////////////////////////////////////////////////////////////
{
    // Init parent class
    if (TrackSegment::getClassTypeId() == SoType::badType()) {
	TrackSegment::initClass();
    }
    if (LODD::getClassTypeId() == SoType::badType()) {
	LODD::initClass();
    }

    SO_NODE_INIT_CLASS(Curve, TrackSegment, "TrackSegment");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Public curve constructor, must be passed length/radius so it can
//    construct scene graph.
//

Curve::Curve(float r, float a)

//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(Curve);
    
    SO_NODE_ADD_FIELD(radius, (1.0));
    SO_NODE_ADD_FIELD(angle, (.25));

    ID = -1;
    next = NULL;

    if (r != 1.0) radius = r;
    if (a != .25) angle = a;

    renderCaching = SoSeparator::OFF;

    constructSceneGraph();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Curve constructor, used when reading from file:
//

Curve::Curve()

//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(Curve);
    
    SO_NODE_ADD_FIELD(radius, (1.0));
    SO_NODE_ADD_FIELD(angle, (.25));

    ID = -1;
    next = NULL;

    renderCaching = SoSeparator::OFF;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Curve destructor
//

Curve::~Curve()

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
Curve::readInstance(SoInput *in)

//
////////////////////////////////////////////////////////////////////////
{
    // Call base class:
    SbBool result = TrackSegment::readInstance(in);
    constructSceneGraph();
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create the scene graph, based on the radius/angle fields.
//    Note:  We should optimize this and keep a list of scene graphs
//    indexed by radius/angle, and instance graphs for curves that have
//    the same radius/angle.  That will result in a lot less memory
//    being used.
//

void
Curve::constructSceneGraph()

//
////////////////////////////////////////////////////////////////////////
{
    // First, the transform node that is modified by the link()
    // routine to place this curve in the right spot:
    transform = new SoTransform;
    insertChild(transform, 0);

    // Create a Level-of-Detail node.
    // Generate the Curve in 3 LODs (max-med-min).
    // Instance all the shareable parts.
    LODD *lod = new LODD;
    addChild(lod);
    lod->distance.set1Value( 0, 500 );
    lod->distance.set1Value( 1, 800 );
    lod->distance.set1Value( 2, 1200 );


    //////////////////////////////////////
    // LOD = 0 - Maximum Level of Detail
    //////////////////////////////////////
    SoSeparator *lod0 = new SoSeparator;
    lod->addChild( lod0 );

    // We want to break up angle into an equal number of segments ,
    // each of which is a max of 15 deg (1/NUM_SEGS_CIRCLE) diff from
    // the previous:
    int numSegs = (int)(NUM_SEGS_CIRCLE*fabsf(angle.getValue()) + .99);

    float r = radius.getValue()*100.0;

    // The origin is at the center of the start of this segment of
    // track.  The center of the turn is therefore:
    SbVec3f center(-r, 0.0, 0.0);

    // Compute the vertices for the road ( (numSegs+1)*2 of them):
    SoCoordinate3 *c3 = new SoCoordinate3;
    float r1 = r - (NUM_LANES*5.0+2.5);  // left-hand edge
    float r2 = r + (NUM_LANES*5.0+2.5);  // right-hand edge

    for (int i = 0; i < numSegs+1; i++) {
	float a = 2*M_PI*angle.getValue()*i / (float)numSegs;
	float x = r1*cosf(a);
	float z = -r1*sinf(a);
	
	c3->point.set1Value(i*2+0, center+SbVec3f(x, 0.0, z));
	
	x = r2*cosf(a);
	z = -r2*sinf(a);
	c3->point.set1Value(i*2+1, center+SbVec3f(x, 0.0, z));
    }

    // Compute the vertices for the shoulders and striped curb.

    float r3 = r1 - SHOULDER_WIDTH;
    float r4 = r1 - CURB_WIDTH;
    float r5 = r2 + CURB_WIDTH;
    float r6 = r2 + SHOULDER_WIDTH;
    for (i = 0; i < numSegs+1; i++) {
	float a  = 2*M_PI*angle.getValue()*i / (float)numSegs;

	// outside of LEFT shoulder
	float x  = r3*cosf(a);
	float z  = -r3*sinf(a);
	c3->point.set1Value(2*(numSegs+1)+i*4+0, center+SbVec3f(x, -1, z));
	
	// outside of LEFT curb
	x = r4*cosf(a);
	z = -r4*sinf(a);
	c3->point.set1Value(2*(numSegs+1)+i*4+1, center+SbVec3f(x, -.5, z));
	
	// outside of RIGHT curb
	x = r5*cosf(a);
	z = -r5*sinf(a);
	c3->point.set1Value(2*(numSegs+1)+i*4+2, center+SbVec3f(x, -.5, z));

	// outside of RIGHT shoulder
	x = r6*cosf(a);
	z = -r6*sinf(a);
	c3->point.set1Value(2*(numSegs+1)+i*4+3, center+SbVec3f(x, -1, z));
    }
    lod0->addChild(c3);

    // Create TriStripSet for road:
    SoBaseColor *trackColor = new SoBaseColor;
    trackColor->rgb.setValue( TRACK_COLOR );
    lod0->addChild(trackColor);
    SoTriangleStripSet *tris = new SoTriangleStripSet;
    tris->numVertices.setValue((numSegs+1)*2);
    lod0->addChild(tris);

    // Create an index list for re-use later...
    long *indList = new long[(numSegs+1)*4+2];
    long *indList2 = new long[(numSegs+1)*4+2];

    // Create left and right shoulders
    // Note that the shoulders have a color ramp from in to out...
    SoBaseColor *shoulderColor = new SoBaseColor;
    shoulderColor->rgb.setValue( SHOULDER_COLOR );
    lod0->addChild(shoulderColor);

    SoMaterialBinding *bind = new SoMaterialBinding;
    bind->value.setValue( SoMaterialBinding::PER_VERTEX_INDEXED );

    SoIndexedTriangleStripSet *shoulder = new SoIndexedTriangleStripSet;

    long *ind = indList2;
    for (i = 0; i < numSegs+1; i++) {
	// alternate OUT-IN color on vertices of left shoulder
	*ind++ = 0; *ind++ = 1;
    }
//    for (i = numSegs+1; i < (numSegs+1)*2; i++) { 
    for (i = 0; i < numSegs+1; i++) { 
	// alternate IN-OUT color on vertices of right shoulder
	*ind++ = 1; *ind++ = 0;
    }
    shoulder->materialIndex.setValues(0, (numSegs+1)*2, indList2);

    ind = indList;
    for (i = 0; i < numSegs+1; i++) {
	// left shoulder
	*ind++ = 2*(numSegs+1) + i*4;
	*ind++ = 2*(numSegs+1) + i*4 + 1;
    }
    *ind++ = SO_END_STRIP_INDEX;
    for (i = 0; i < numSegs+1; i++) {
	// right shoulder
	*ind++ = 2*(numSegs+1) + i*4 + 2;
	*ind++ = 2*(numSegs+1) + i*4 + 3;
    }
    *ind++ = SO_END_STRIP_INDEX;
    shoulder->coordIndex.setValues(0, (numSegs+1)*4+2, indList);
    lod0->addChild(shoulder);

    // Create red-white stripes along INSIDE curve.
    // (The r>0 test inside the loops checks which side is inside...)
    //
    // Create red stripes along INSIDE curb
    SoBaseColor *curbColor1 = new SoBaseColor;
    curbColor1->rgb.setValue( 1.0, 0.0, 0.0 );
    lod0->addChild(curbColor1);
    SoIndexedFaceSet *curb1 = new SoIndexedFaceSet;
    ind = indList;
    for (i = 0; i < numSegs+1; i++ ) {
	*ind++ = (r > 0) ? (2*(numSegs+1) + i*4 + 1) : (i*2 + 1);
	*ind++ = (r > 0) ? (i*2) : (2*(numSegs+1) + i*4 + 2);
	i++;
	*ind++ = (r > 0) ? (i*2) : (2*(numSegs+1) + i*4 + 2);
	*ind++ = (r > 0) ? (2*(numSegs+1) + i*4 + 1) : (i*2 + 1);
	*ind++ = SO_END_FACE_INDEX;
    }
    curb1->coordIndex.setValues(0, ((numSegs+1)/2)*5, indList);
    lod0->addChild(curb1);

    // Create white stripes along INSIDE curb
    SoBaseColor *curbColor2 = new SoBaseColor;
    curbColor2->rgb.setValue( 1.0, 1.0, 1.0 );
    lod0->addChild(curbColor2);
    SoIndexedFaceSet *curb2 = new SoIndexedFaceSet;
    ind = indList;
    for (i = 1; i < numSegs+1; i++ ) {
	*ind++ = (r > 0) ? (2*(numSegs+1) + i*4 + 1) : (i*2 + 1);
	*ind++ = (r > 0) ? (i*2) : (2*(numSegs+1) + i*4 + 2);
	i++;
	*ind++ = (r > 0) ? (i*2) : (2*(numSegs+1) + i*4 + 2);
	*ind++ = (r > 0) ? (2*(numSegs+1) + i*4 + 1) : (i*2 + 1);
	*ind++ = SO_END_FACE_INDEX;
    }
    curb2->coordIndex.setValues(0, ((numSegs)/2)*5, indList);
    lod0->addChild(curb2);

    // Create white stripe along OUTSIDE curve
    SoIndexedTriangleStripSet *outsideCurb = new SoIndexedTriangleStripSet;
    ind = indList;
    for (i = 0; i < numSegs+1; i++) {
	*ind++ = ( r < 0 ? (2*(numSegs+1) + i*4 + 1) : (i*2 + 1) );
	*ind++ = ( r < 0 ? (i*2) : (2*(numSegs+1) + i*4 + 2) );
    }
    *ind++ = SO_END_STRIP_INDEX;
    outsideCurb->coordIndex.setValues(0, (numSegs+1)*2 + 1, indList);
    lod0->addChild(outsideCurb);

    // Now, create coordinates for slot lines:
#define REPEAT_VALUE 3
    SoBaseColor *slotColor = new SoBaseColor;
    slotColor->rgb.setValue( 0.0, 0.0, 0.0 );
    lod0->addChild(slotColor);

    SoCoordinate3 *slotc3 = new SoCoordinate3;
    float slotr[NUM_LANES];
    for (i=0; i<NUM_LANES; i++) {
	slotr[i] = r - (NUM_LANES*5.0 - 5.0) + 10*i;
    }

    float x;
    float z;
    for (i = 0; i < numSegs*REPEAT_VALUE+1; i++) {
	float a = 2*M_PI*angle.getValue()*i / ((float)numSegs*REPEAT_VALUE);
        if (i == 0) {
            a = -.015 / radius.getValue();
        }
	for (int j = 0; j<NUM_LANES; j++) {
	    x = slotr[j]*cosf(a);
	    z = -slotr[j]*sinf(a);
	    slotc3->point.set1Value(i+j*(numSegs*REPEAT_VALUE+1),
		    center+SbVec3f(x, 0.7, z));
	}
    }
    lod0->addChild(slotc3);
    SoLineSet *lines = new SoLineSet;
    for (int j=0; j<NUM_LANES; j++) {
	lines->numVertices.set1Value(j, numSegs*REPEAT_VALUE+1);
    }
    lod0->addChild(lines);

    //////////////////////////////////////
    // LOD = 1 - Medium Level of Detail
    //////////////////////////////////////
    SoSeparator *lod1 = new SoSeparator;
    lod->addChild( lod1 );

    // Instance nodes from LOD 0 for the track and stuff...
    lod1->addChild(c3);
    lod1->addChild(trackColor);
    lod1->addChild(tris);
    lod1->addChild(shoulderColor);
    lod1->addChild(shoulder);

    // Create white stripes along INSIDE and OUTSIDE curbs
    curbColor1 = new SoBaseColor;
    curbColor1->rgb.setValue( 1.0, 0.5, 0.5 );
    lod1->addChild(curbColor1);

    SoIndexedTriangleStripSet *curb3 = new SoIndexedTriangleStripSet;
    ind = indList;
    for (i = 0; i < numSegs+1; i++ ) {
	*ind++ = ( r > 0 ? (2*(numSegs+1) + i*4 + 1) : (i*2 + 1) );
	*ind++ = ( r > 0 ? (i*2) : (2*(numSegs+1) + i*4 + 2) );
    }
    *ind++ = SO_END_STRIP_INDEX;
    curb3->coordIndex.setValues(0, (numSegs+1)*2+1, indList);
    lod1->addChild(curb3);
    lod1->addChild(outsideCurb);

    // Create slot lines...
    slotColor = new SoBaseColor;
    slotColor->rgb.setValue( 0.2, 0.2, 0.2 );
    lod1->addChild(slotColor);
    slotc3 = new SoCoordinate3;
    for (i = 0; i < numSegs+1; i++) {
	float a = 2*M_PI*angle.getValue()*i / ((float)numSegs);
        if (i == 0) {
            if (angle.getValue() < 0.0)
                a = .02;
            else
                a = -.02;
        }
	for (int j = 0; j<NUM_LANES; j++ ) {
	    x = slotr[j]*cosf(a);
	    z = -slotr[j]*sinf(a);
	    slotc3->point.set1Value(i+j*(numSegs+1),
                    center+SbVec3f(x, 0.7, z));
	}
    }
    lod1->addChild(slotc3);
    lines = new SoLineSet;
    for (j=0; j<NUM_LANES; j++) {
	lines->numVertices.set1Value(j, numSegs+1);
    }
    lod1->addChild(lines);


    //////////////////////////////////////
    // LOD = 2 - Minimum Level of Detail
    //////////////////////////////////////
    SoSeparator *lod2 = new SoSeparator;
    lod->addChild( lod2 );
    trackColor = new SoBaseColor;
    trackColor->rgb.setValue( TRACK_COLOR_LO_RES );
    lod2->addChild(trackColor);
    lod2->addChild(c3);
    lod2->addChild(tris);
    lod2->addChild(shoulderColor);
    lod2->addChild(shoulder);

    //////////////////////////////////////
    // LOD = 3 - Nothing at all
    //////////////////////////////////////
    lod->addChild(new SoInfo);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Link a curved piece onto the end of previous piece.
//

void
Curve::link(SbVec3f &lastPosition, SbRotation &lastOrientation,
	       TrackSegment *nextSeg, TrackSegment *prevSeg,
	       int segmentID)

//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (next != NULL) {
	SoDebugError::post("Curve::link", "May not be multiply "
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

    float angleRad = 2*M_PI*angle.getValue();
    float worldRadius = radius.getValue()*100.0;

    // Update startPosition to be end of this segment of track:
    float deltaX, deltaZ;
    deltaX = worldRadius*cosf(angleRad) - worldRadius;
    deltaZ = -worldRadius*sinf(angleRad);

    // The end of this segment, in our local space:
    SbVec3f end(deltaX, 0, deltaZ);
    
    // rotate end based on orientation:
    SbVec3f xformedEnd;
    orientation.multVec(end, xformedEnd);

    // And add on to lastPosition:
    lastPosition += xformedEnd;
    
    // Update orientation:
    lastOrientation *= SbRotation(SbVec3f(0,1,0), angleRad);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Move something down the track.
//

void
Curve::getNewPosition(const TrackPosition &start,
			 const TrackPosition &velocity,
			 const SbTime &deltaTime,
			 TrackPosition &result,
			 float &grip)

//
////////////////////////////////////////////////////////////////////////
{
    // Lane position, as an offset from the center of the track:
    float laneDelta = (start.across*10.0 - (NUM_LANES*5.0-5.0))/100.0;
    float r = (radius.getValue()+laneDelta)*100.0;

    // Length of this track (depends on what lane we're in):
    float trackLength =
	fabsf(2*M_PI*(radius.getValue()+laneDelta)*angle.getValue());
    
    float t = deltaTime.getValue();

    // Figure out how far down the track we'll travel:
    float distAlong = velocity.along * t / trackLength;
    float distAcross = velocity.across * t;
    
    // Break up start position into integer/fraction parts:
    int seg = (int)(start.along);
    float frac = start.along - seg;


    // Calculate the grip of the track at the current position
    // The grip will be calculated as some function of the radius
    // of the curve.
    float newGrip = fabs(r) * 1.5;
    if (newGrip < grip) grip = newGrip;

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
    } else if (frac + distAlong < 0.0) {
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
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translate from track coordinates to world coordinates.
//

void
Curve::getWorldPosition(const TrackPosition &position,
			   WorldPosition &result)

//
////////////////////////////////////////////////////////////////////////
{
    // Lane position, as an offset from the center of the track:
    float laneDelta = (position.across*10.0 - (NUM_LANES*5.0-5.0))/100.0;

    float r = (radius.getValue()+laneDelta)*100.0;

    // Break up start position into integer/fraction parts:
    int seg = (int)(position.along);
    float frac = position.along - seg;

    float angleRad = 2*M_PI*angle.getValue()*frac;

    float x = r*cosf(angleRad) - radius.getValue()*100.0;
    float z = -r*sinf(angleRad);

    // Position relative to start of this track:
    SbVec3f p(x, 0.0, z);

    // rotate/translate p into world space:
    SbVec3f xformedP;
    orientation.multVec(p, xformedP);

    result.translation = xformedP + startPosition;
    result.rotation = orientation*SbRotation(SbVec3f(0,1,0), angleRad);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a position on the track, return the curvature of the track
//    at that position.
//

float
Curve::getCurvature(const TrackPosition &position)

//
////////////////////////////////////////////////////////////////////////
{
    // Lane position, as an offset from the center of the track:
    float laneDelta = (position.across*10.0 - (NUM_LANES*5.0-5.0))/100.0;
    float r = (radius.getValue()+laneDelta)*100.0;

    return r;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a position on the track, return the length of the track
//    at that radius.
//

float
Curve::getLength(const TrackPosition &position)

//
////////////////////////////////////////////////////////////////////////
{
    // Lane position, as an offset from the center of the track:
    float laneDelta = (position.across*10.0 - (NUM_LANES*5.0-5.0))/100.0;
    float r = (radius.getValue()+laneDelta);

    return (r * 2 * M_PI * angle.getValue());
}
