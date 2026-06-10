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
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTransform.h>
#include "Common.h"
#include "Motion.h"
#include "Panel.h"
#include "Ring.h"
#include "Scene.h"
#include "Sound.h"

//--------------------------------------------------------------------------
//
// Class: Ring
//
//--------------------------------------------------------------------------

SoNode	*Ring::sceneRoot = NULL;

////////////////////////////////////////////////////////////////////////////
//
// Constructor. The constructor is passed the center, orientation, and
// radius of the ring.
//
////////////////////////////////////////////////////////////////////////////

Ring::Ring(const SbVec3f &_center, const SbRotation &_orientation,
	   float _radius)
{
    center		= _center;
    orientation		= _orientation;
    radius		= _radius;
    isActive		= FALSE;
    wasApplied		= FALSE;

    // Set up plane containing ring to make intersection tests faster
    SbVec3f axis;
    orientation.multVec(SbVec3f(0.0, 0.0, -1.0), axis);
    plane = SbPlane(axis, center);

    // Find random center for button within ring (assuming initial
    // ring is in xy-plane), making sure entire button will fit
    float x, y, maxRad = radius - RING_BUTTON_SIZE;
    do {
	x = RANDOM(-maxRad, maxRad);
	y = RANDOM(-maxRad, maxRad);
    } while (x*x + y*y > maxRad*maxRad);
    
    buttonOffset.setValue(x, y, 0.0);
    orientation.multVec(buttonOffset, buttonCenter);
    buttonCenter += center;
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Ring::~Ring()
{
    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Updates ring based on ship motion.
//
////////////////////////////////////////////////////////////////////////////

void
Ring::updateShip(Motion *shipMotion)
{
    if (! isActive || wasApplied)
	return;

    // Find intersection point of ship's motion across the plane
    // containing the ring, if any.
    SbVec3f intersection;
    if (! intersect(shipMotion, intersection))
	return;

    // Make sure the intersection is within the radius of the ring
    SbVec3f diff = intersection - center;
    if (diff.dot(diff) <= radius * radius) {
	apply(shipMotion);
	Panel::startRingApply();
	Sound::playOnce("ringApply");
	activate(FALSE);
	wasApplied = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Updates ring based on blast motion.
//
////////////////////////////////////////////////////////////////////////////

void
Ring::updateBlast(Motion *blastMotion)
{
    // If already active, nothing to do
    if (isActive || wasApplied)
	return;

    // Find intersection point of blast's motion across the plane
    // containing the ring, if any.
    SbVec3f intersection;
    if (! intersect(blastMotion, intersection))
	return;

    // Make sure the intersection is within the radius of the button
    SbVec3f diff = intersection - buttonCenter;
    if (diff.dot(diff) <= RING_BUTTON_SIZE * RING_BUTTON_SIZE) {
	Sound::playOnce("ringActivate");
	activate(TRUE);
	blastMotion->hit = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph representing ring.
//
////////////////////////////////////////////////////////////////////////////

void
Ring::buildGraph()
{
    if (sceneRoot == NULL) {
	sceneRoot = Scene::readFile("Ring.iv");
	sceneRoot->ref();
    }

    // Create a new separator to hold ring
    SoSeparator *topSep = new SoSeparator;
    topSep->renderCaching = SoSeparator::OFF;
    topSep->ref();

    // Add an instance of the shared properties
    topSep->addChild(Scene::find("RingProps"));

    // Add a copy of the main ring graph
    topSep->addChild(Scene::find("RingSep")->copy());
    topSep->addChild(Scene::find("RingSep")->copy());

    // Set the correct color
    SoBaseColor *color = (SoBaseColor *) Scene::findUnder("RingColor", topSep);
    color->rgb = getColor();

    // Set scale and translation
    SoTransform *xf = (SoTransform *) Scene::findUnder("RingXform", topSep);
    xf->scaleFactor.setValue(radius, radius, 1.0);
    xf->rotation = orientation;
    xf->translation = center;

    // Find switch node to allow us to change ring's state
    ringSwitch = (SoSwitch *) Scene::findUnder("RingSwitch", topSep);

    // Set button position
    SoTransform *butXf =
	(SoTransform *) Scene::findUnder("RingButtonXform", topSep);
    butXf->translation = buttonOffset;

    // Change text to correct letter
    SoText3 *text = (SoText3 *) Scene::findUnder("RingText", ringSwitch);
    text->string = getLabel();

    root = topSep;
}

////////////////////////////////////////////////////////////////////////////
//
// Activates or deactivates ring.
//
////////////////////////////////////////////////////////////////////////////

void
Ring::activate(SbBool on)
{
    isActive = on;

    // Turn the ring graph on or off
    ringSwitch->whichChild = (on ? 1 : 2);
}

////////////////////////////////////////////////////////////////////////////
//
// Computes intersection of motion with plane of ring. Returns FALSE
// if the motion does not intersect within its interval. Otherwise,
// fills in intersection and returns TRUE.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Ring::intersect(Motion *motion, SbVec3f &intersection)
{
    // First, check if endpoints of motion are on opposite sides of
    // the plane. If not, there's no intersection.
    if (plane.isInHalfSpace(motion->position0) ==
	plane.isInHalfSpace(motion->position1))
	return FALSE;

    // Find the actual intersection point
    return plane.intersect(SbLine(motion->position0, motion->position1),
			   intersection);
}

//--------------------------------------------------------------------------
//
// Class: EnergizerRing
//
//--------------------------------------------------------------------------

EnergizerRing::EnergizerRing(const SbVec3f &center,
			     const SbRotation &orientation,
			     float radius, float _energy) :

			     Ring(center, orientation, radius)
{
    energy = _energy;
    buildGraph();
}

SbColor
EnergizerRing::getColor() const
{
    return SbColor(.2, 1.0, .2);
}

const char *
EnergizerRing::getLabel() const
{
    return "E";
}

void
EnergizerRing::apply(Motion *shipMotion)
{
    shipMotion->addedEnergy += energy;
}

//--------------------------------------------------------------------------
//
// Class: StealthRing
//
//--------------------------------------------------------------------------

StealthRing::StealthRing(const SbVec3f &center, const SbRotation &orientation,
			 float radius, float _stealthTime) :

			 Ring(center, orientation, radius)
{
    stealthTime = _stealthTime;
    buildGraph();
}

SbColor
StealthRing::getColor() const
{
    return SbColor(.3, .6, 1.0);
}

const char *
StealthRing::getLabel() const
{
    return "S";
}

void
StealthRing::apply(Motion *shipMotion)
{
    shipMotion->stealthTime += stealthTime;
}

//--------------------------------------------------------------------------
//
// Class: WarpRing
//
//--------------------------------------------------------------------------

WarpRing::WarpRing(const SbVec3f &center, const SbRotation &orientation,
		   float radius, float _warpDistance) :

		   Ring(center, orientation, radius)
{
    warpDistance = _warpDistance;
    buildGraph();
}

SbColor
WarpRing::getColor() const
{
    return SbColor(.8, .8, .2);
}

const char *
WarpRing::getLabel() const
{
    return "W";
}

void
WarpRing::apply(Motion *shipMotion)
{
    shipMotion->warpDistance += warpDistance;
}
