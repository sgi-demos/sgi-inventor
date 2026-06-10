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
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include "Common.h"
#include "Mine.h"
#include "Motion.h"
#include "Panel.h"
#include "Scene.h"
#include "Sound.h"

SoNode	*Mine::sceneRoot = NULL;

////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
////////////////////////////////////////////////////////////////////////////

Mine::Mine(const SbVec3f &_position, float _radius, float _delay)
{
    position = _position;
    radius = _radius;
    delay = _delay;

    // Make sphere a little larger to make deactivation easier
    mineSphere.setValue(position, radius * MINE_DEACTIVATE_SCALE);

    state = DORMANT;

    // Build scene graph to represent mine
    buildGraph();
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Mine::~Mine()
{
    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Updates mine based on ship motion.
//
////////////////////////////////////////////////////////////////////////////

void
Mine::updateShip(Motion *shipMotion)
{
    // Note: the squares of distances are computed here to avoid sqrt().

    switch (state) {

      case DORMANT:
	{
	    // See if the ship hit the mine
	    if (isHitBy(shipMotion))
		explode(shipMotion);

	    // See if mine should be activated by ship getting close
	    else if (shipMotion->stealthTime == 0.0) {
		SbVec3f	diff = shipMotion->position0 - position;
		float	dist = diff.dot(diff);

		// Use square of distance to avoid sqrt
		if (dist < MINE_ACTIVATE_DISTANCE * MINE_ACTIVATE_DISTANCE)
		    activate();
	    }
	}
	break;

      case ACTIVE:
	// See if timer goes off or ship hits the mine
	delay -= shipMotion->numSeconds;
	if (delay <= 0.0 || isHitBy(shipMotion))
	    explode(shipMotion);
	break;

      case EXPLODING1:
	changeTime -= shipMotion->numSeconds;
	if (changeTime <= 0.0) {
	    mineSwitch->whichChild = state = EXPLODING2;
	    changeTime = 0.1;		// Until next change in state
	}
	break;

      case EXPLODING2:
	changeTime -= shipMotion->numSeconds;
	if (changeTime <= 0.0)
	    mineSwitch->whichChild = state = INACTIVE;
	break;

      case DEACTIVATING1:
	changeTime -= shipMotion->numSeconds;
	if (changeTime <= 0.0) {
	    mineSwitch->whichChild = state = DEACTIVATING2;
	    changeTime = 0.1;		// Until next change in state
	}
	break;

      case DEACTIVATING2:
	changeTime -= shipMotion->numSeconds;
	if (changeTime <= 0.0)
	    mineSwitch->whichChild = state = INACTIVE;
	break;

      case INACTIVE:
	break;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Updates mine based on blast motion.
//
////////////////////////////////////////////////////////////////////////////

void
Mine::updateBlast(Motion *blastMotion)
{
    if ((state == DORMANT || state == ACTIVE) && isHitBy(blastMotion)) {
	Panel::addScore(SCORE_DEACTIVATE_MINE);
	deactivate();
	blastMotion->hit = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph representing mine.
//
////////////////////////////////////////////////////////////////////////////

void
Mine::buildGraph()
{
    if (sceneRoot == NULL) {
	sceneRoot = Scene::readFile("Mine.iv");
	sceneRoot->ref();
    }

    // Create a new separator to hold mine
    SoSeparator *sep = new SoSeparator;
    sep->ref();
    sep->renderCaching = SoSeparator::OFF;

    // Create a transform node and set scale and translation
    SoTransform *xf = new SoTransform;
    xf->scaleFactor.setValue(radius, radius, radius);
    xf->translation = position;
    sep->addChild(xf);

    // Create a switch node to switch between mine states
    mineSwitch = new SoSwitch;
    mineSwitch->whichChild = DORMANT;
    sep->addChild(mineSwitch);

    // Add instances of mine graphs to switch
    mineSwitch->addChild(Scene::find("MineDormant"));
    mineSwitch->addChild(Scene::find("MineActive"));
    mineSwitch->addChild(Scene::find("MineExploding1"));
    mineSwitch->addChild(Scene::find("MineExploding2"));
    mineSwitch->addChild(Scene::find("MineDeactivating1"));
    mineSwitch->addChild(Scene::find("MineDeactivating2"));
    mineSwitch->addChild(Scene::find("MineInactive"));

    root = sep;
}

////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if given motion (ship or blast) hits mine.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Mine::isHitBy(Motion *motion)
{
    // The motion intersects the mine if it intersects the sphere
    // representing the mine and the intersection is between the start
    // and end positions of the motion

    // Check for intersection with sphere
    SbLine  ray(motion->position0, motion->position1);
    SbVec3f intersection;
    if (! mineSphere.intersect(ray, intersection))
	return FALSE;

    // See if intersection is past ending position by testing sign of
    // dot product
    return ((intersection - motion->position1).dot(motion->forward) <= 0);
}

////////////////////////////////////////////////////////////////////////////
//
// Activates mine.
//
////////////////////////////////////////////////////////////////////////////

void
Mine::activate()
{
    mineSwitch->whichChild = state = ACTIVE;

    Sound::playOnce("mineActivation");
}

////////////////////////////////////////////////////////////////////////////
//
// Deactivates mine.
//
////////////////////////////////////////////////////////////////////////////

void
Mine::deactivate()
{
    // Display first deactivation version of mine for the given amount
    // of time
    mineSwitch->whichChild = state = DEACTIVATING1;
    changeTime = .1;

    Sound::playOnce("mineDeactivation");
}

////////////////////////////////////////////////////////////////////////////
//
// Makes mine explode.
//
////////////////////////////////////////////////////////////////////////////

void
Mine::explode(Motion *shipMotion)
{
    // Don't bother with the fancy explosion graphics if the mine
    // cannot be seen - i.e., it's way behind the ship
    SbVec3f shipToMine = position - shipMotion->position1;
    if (shipToMine.dot(shipMotion->forward) < 0.0)
	mineSwitch->whichChild = state = INACTIVE;

    else {
	// Display first exploding version of mine for the given
	// amount of time
	mineSwitch->whichChild = state = EXPLODING1;
	changeTime = .1;
    }

    // Compute force of explosion based on distance of ship from mine.
    // The maximum force is 1.0 (at close distance).
    float distance = shipToMine.length();

    shipMotion->explosionForce += MINE_EXPLOSION_FORCE(distance);

    Panel::startExplode();

    Sound::playOnce("mineExplosion");
}
