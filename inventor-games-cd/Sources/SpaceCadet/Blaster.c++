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
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSeparator.h>
#include "Blaster.h"
#include "Common.h"
#include "Panel.h"
#include "Scene.h"
#include "Sound.h"
#include "Tunnel.h"

struct Blast {
  public:
    SbVec3f		position;	// Current position of blast
    SbVec3f		direction;	// Direction of blast
    float		distance;	// Distance traveled
    SoTranslation	*translation;	// Translation node that moves blast
};    

////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
////////////////////////////////////////////////////////////////////////////

Blaster::Blaster()
{
    blasts = new Blast [MAX_BLASTS];

    // Build scene graph to represent blasts
    buildGraph();

    // Initialize things
    setTunnel(NULL);
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Blaster::~Blaster()
{
    delete blasts;

    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Returns scene graph representing blasts.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Blaster::getGraph()
{
    return root;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph representing blasts.
//
////////////////////////////////////////////////////////////////////////////

void
Blaster::buildGraph()
{
    root = Scene::readFile("Blaster.iv");
    root->ref();

    blastObj = Scene::find("BlastObj");
    blastSep = (SoSeparator *) Scene::find("BlastSep");
}

////////////////////////////////////////////////////////////////////////////
//
// Sets current tunnel to shoot into.
//
////////////////////////////////////////////////////////////////////////////

void
Blaster::setTunnel(Tunnel *_tunnel)
{
    tunnel = _tunnel;
    firing = FALSE;
    needToFire = FALSE;
    sinceLastBlast = 1000.0;	// Make sure first blast goes right away
    numBlasts = 0;

    // No blasts initially
    blastSep->removeAllChildren();
}

////////////////////////////////////////////////////////////////////////////
//
// Sets whether currently firing.
//
////////////////////////////////////////////////////////////////////////////

void
Blaster::setFiring(SbBool flag)
{
    // Don't miss any shots that occurred between updates
    if (flag != firing) {
	firing = flag;

	if (firing)
	    needToFire = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Updates all current blasts for given ship motion interval. Fires a
// new blast if appropriate.
//
////////////////////////////////////////////////////////////////////////////

void
Blaster::update(Motion *shipMotion)
{
    // Increment time since last blast
    sinceLastBlast += shipMotion->numSeconds;

    // Move all current blasts. Go backwards through the list in case
    // we remove any.
    for (int i = numBlasts - 1; i >= 0; i--) {
	Blast	*blast = &blasts[i];
	float	dist = BLAST_SPEED * shipMotion->numSeconds;

	blast->distance += dist;

	// Remove any blast that has moved too far
	if (blast->distance > MAX_BLAST_DISTANCE)
	    removeBlast(i);
	else {
	    // See if blast hits anything
	    motion.position0  = blast->position;
	    motion.position1  = blast->position + blast->direction * dist;
	    motion.forward    = (motion.position1 - motion.position0);
	    motion.forward.normalize();
	    motion.numSeconds = shipMotion->numSeconds;
	    motion.hit        = FALSE;
	    tunnel->updateBlast(&motion);
	    if (motion.hit) {
		removeBlast(i);
		motion.hit = FALSE;
	    }
	    else {
		blast->position = motion.position1;

		// Update blast's rendered location
		blast->translation->translation = blast->position;
	    }
	}
    }

    // See if we should fire off a new blast
    if ((firing || needToFire) &&
	numBlasts < MAX_BLASTS &&
	sinceLastBlast > MIN_BLAST_INTERVAL) {

	// Move blast position down a little to look like it's coming
	// from the bottom of the ship
	addBlast(shipMotion->position1 - (.1 * shipMotion->up),
		 shipMotion->forward);
	Sound::playOnce("blast");
	sinceLastBlast = 0.0;

	// Firing a blast uses a teeny bit of energy
	Panel::setEnergy(Panel::getEnergy() - BLAST_ENERGY_LOSS);
    }

    needToFire = FALSE;
}

////////////////////////////////////////////////////////////////////////////
//
// Adds a new active blast.
//
////////////////////////////////////////////////////////////////////////////

void
Blaster::addBlast(const SbVec3f &position, const SbVec3f &direction)
{
    Blast *blast = &blasts[numBlasts++];

    blast->position  = position;
    blast->direction = direction;
    blast->distance  = 0.0;
    blast->translation = new SoTranslation;

    // Create a new subgraph to show blast
    SoSeparator *sep = new SoSeparator;
    sep->renderCaching = SoSeparator::OFF;
    blast->translation->translation = position;
    sep->addChild(blast->translation);
    sep->addChild(blastObj);
    blastSep->addChild(sep);
}

////////////////////////////////////////////////////////////////////////////
//
// Removes a blast that is no longer active.
//
////////////////////////////////////////////////////////////////////////////

void
Blaster::removeBlast(int index)
{
    // If this is not the last one in the array, move all the ones
    // after it up by 1
    for (int i = index; i < numBlasts - 1; i++)
	blasts[i] = blasts[i + 1];

    numBlasts--;

    // Remove the blast's graph
    blastSep->removeChild(index);
}
