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
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include "Blaster.h"
#include "Common.h"
#include "Panel.h"
#include "Scene.h"
#include "Ship.h"
#include "Sound.h"
#include "Tunnel.h"

Tunnel		*Ship::tunnel;		// Tunnel flying through
Blaster		*Ship::blaster;		// Blaster to shoot things
Ship::State	Ship::state;		// Current motion state
SbBool		Ship::airplaneMode;	// Whether to use airplane mode
SbBool		Ship::darkMode;		// Whether to fly in the dark
float		Ship::speed;		// Current speed
int		Ship::shipSound;	// Id of continuous sound
SbVec3f		Ship::position;		// Current 3D position
SbRotation	Ship::orientation;	// Orientation wrt default orientation
SbVec2s		Ship::initCursor;	// Initial cursor position
SbVec2s		Ship::curCursor;	// Current cursor position
SbVec3f		Ship::headLightDir;	// Initial direction of headlight 1
Motion		Ship::motion;		// Motion parameters

SoNode			*Ship::root;		// Root of graph
SoPerspectiveCamera	*Ship::camera;		// Camera node
SoDirectionalLight	*Ship::headLight;	// Headlight node

// Converts ship's speed into pitch of continuous sound. Defined so
// that speed 0 results in a pitch of .5, and max speed is pitch 4.
#define SHIP_PITCH(speed)	(0.5 + 3.5 * (speed) / MAX_SPEED)

////////////////////////////////////////////////////////////////////////////
//
// Initializes class.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::init()
{
    tunnel = NULL;

    blaster = new Blaster;

    // Build scene graph containing camera and lights
    buildGraph();

    headLightDir = headLight->direction.getValue();
    headLightDir.normalize();
}

////////////////////////////////////////////////////////////////////////////
//
// Cleans up.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::cleanUp()
{
    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::buildGraph()
{
    // Scene::readFile() returns a separator over the group in
    // Ship.iv. We want the group under it, so use the first child.

    SoSeparator *sep = (SoSeparator *) Scene::readFile("Ship.iv");
    sep->ref();
    root = sep->getChild(0);
    root->ref();
    sep->unref();

    camera    = (SoPerspectiveCamera *) Scene::find("ShipCamera");
    headLight = (SoDirectionalLight  *) Scene::find("ShipHeadLight");

    // Initialize the camera's fields
    camera->heightAngle  = .5;
    camera->nearDistance = 0.1;
    camera->farDistance  = 200.0;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets dark mode - if TRUE, lights are out in tunnels.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::setDarkMode(SbBool flag)
{
    darkMode = flag;

    // Turn the lights on or off
    SoDirectionalLight *fixedLight =
	(SoDirectionalLight  *) Scene::find("ShipFixedLight");

    headLight->on  = ! flag;
    fixedLight->on = ! flag;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets base cursor position to given one
//
////////////////////////////////////////////////////////////////////////////

void
Ship::setBaseCursor(const SbVec2s &pos)
{
    initCursor = pos;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets current tunnel to fly through. Resets to start of tunnel.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::setTunnel(Tunnel *_tunnel)
{
    tunnel = _tunnel;

    state  = COAST;
    speed  = 0.0;

    position.setValue(0.0, 0.0, 0.0);
    orientation = SbRotation::identity();

    motion.forward.setValue(0.0, 0.0, -1.0);
    motion.up.setValue(0.0, 1.0, 0.0);
    motion.exited = FALSE;
    motion.hit    = FALSE;
    motion.explosionForce = 0.0;
    motion.addedEnergy    = 0.0;
    motion.warpDistance   = 0.0;
    motion.stealthTime    = 0.0;

    setView();

    blaster->setTunnel(tunnel);
}

////////////////////////////////////////////////////////////////////////////
//
// Sets whether ship's blaster is currently firing.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::setFiring(SbBool flag)
{
    blaster->setFiring(flag);
}

////////////////////////////////////////////////////////////////////////////
//
// Sets new cursor position to use to update ship location and direction.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::setCursor(const SbVec2s &newPosition)
{
    curCursor = newPosition;
}

////////////////////////////////////////////////////////////////////////////
//
// Starts the ship.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::start()
{
    // Set up continuous sound
    shipSound = Sound::initContinuous("whoosh", SHIP_PITCH(speed));
}

////////////////////////////////////////////////////////////////////////////
//
// Stops the ship.
//
////////////////////////////////////////////////////////////////////////////

void
Ship::stop()
{
    Sound::endContinuous(shipSound);
}

////////////////////////////////////////////////////////////////////////////
//
// Moves over given time interval (in seconds). Returns TRUE if ship
// has exited tunnel.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Ship::update(float numSeconds)
{
    // If in warp mode, just move ship around
    if (motion.warpDistance > 0.0) {

	// Move at warp speed for the given length of time
	speed = WARP_SPEED;
	Sound::setContinuousPitch(shipSound, SHIP_PITCH(speed));
	float distance = speed * numSeconds / SENSOR_INTERVAL;
	motion.warpDistance -= distance;
	if (motion.warpDistance <= 0.0) {
	    distance += motion.warpDistance;
	    motion.warpDistance = 0.0;
	    speed = 0.0;
	    Panel::setSpeed(0.0);
	    Sound::setContinuousPitch(shipSound, SHIP_PITCH(0.0));
	}
	motion.position0 = position;
	tunnel->moveSafely(&motion, distance);
	if (motion.exited) {
	    Panel::setSpeed(0.0);
	    Sound::setContinuousPitch(shipSound, SHIP_PITCH(0.0));
	    return TRUE;
	}

	// Set new position and orientation for ship
	position = motion.positionAfterHit;
	SbRotation rot(motion.forward, motion.forwardAfterHit);
	motion.forward = motion.forwardAfterHit;
	rot.multVec(motion.up, motion.up);
	orientation *= rot;

	setView();
	return FALSE;
    }

    // Change speed based on current state
    switch (state) {
      case ACCELERATE:
	speed += SPEED_INCREMENT;
	if (speed > getMaxSpeed())
	    speed = getMaxSpeed();
	// Use a little energy for acceleration
	Panel::setEnergy(Panel::getEnergy() - ACCEL_ENERGY_LOSS);
	break;
      case DECELERATE:
	speed -= SPEED_DECREMENT;
	if (speed < 0.0)
	    speed = 0.0;
	break;
      case COAST:
	break;
    }

    // Compute distance traveled over time interval. The basic speed
    // is designed for SENSOR_INTERVAL.
    float distance = speed * numSeconds / SENSOR_INTERVAL;

    // Just rotate around the forward direction
    SbVec2s delta = curCursor - initCursor;

    // Horizontal motion rotates "ship" around forward direction
    float dx = delta[0] * HORIZ_SPEED_MULT;
    SbRotation hrot(motion.forward, dx);
    hrot.multVec(motion.up, motion.up);

    // Vertical motion tilts "ship" up and down 
    float dy = delta[1] * VERT_SPEED_MULT;
    if (airplaneMode)
	dy = -dy;
    SbRotation vrot(motion.forward.cross(motion.up), dy);
    vrot.multVec(motion.forward, motion.forward);
    vrot.multVec(motion.up, motion.up);

    // Rotate orientation
    orientation *= hrot * vrot;

    // Compute motion interval based on current direction and speed
    motion.position0  = position;
    motion.position1  = position + distance * motion.forward;
    motion.numSeconds = numSeconds;

    // Move and see what happens in tunnel
    tunnel->updateShip(&motion);

    // Exited tunnel
    if (motion.exited) {
	Panel::setSpeed(0.0);
	Sound::setContinuousPitch(shipSound, SHIP_PITCH(0.0));
	return TRUE;
    }

    // If we hit something, bounce and lose energy
    if (motion.hit) {

	Panel::startCrash();
    
	// Loss of energy is proportional to current speed
	Panel::setEnergy(Panel::getEnergy() -
			 CRASH_ENERGY_LOSS(speed, getMaxSpeed()));
	
	// Speed gets cut
	speed *= CRASH_SPEED_MULT;
	
	// Set to new position
	position = motion.positionAfterHit;

	// Rotate ship so that forward direction is along new direction
	SbRotation rot(motion.forward, motion.forwardAfterHit);
	motion.forward = motion.forwardAfterHit;
	rot.multVec(motion.up, motion.up);
	orientation *= rot;
	
	// Play sound
	Sound::playOnce("crash");

	// Reset
	motion.hit = FALSE;
    }
    else
	position = motion.position1;
    
    // If anything exploded near us, give the ship a jolt
    if (motion.explosionForce > 0.0) {
	SbVec3f randAxis(RANDOM(-1.0, 1.0),
			 RANDOM(-1.0, 1.0),
			 RANDOM(-1.0, 1.0));
	randAxis.normalize();
	
	// An explosionForce of 1 can jolt us by the maximum
	// rotation. (Larger forces are possible if more than 1
	// mine explodes simultaneously.)
	float maxAngle = motion.explosionForce * MAX_MINE_EXPLOSION_ROTATION;
	SbRotation rot(randAxis, RANDOM(0.0, RADIANS(maxAngle)));
	rot.multVec(motion.forward, motion.forward);
	rot.multVec(motion.up, motion.up);
	orientation *= rot;
	
	// Lose energy
	Panel::setEnergy(Panel::getEnergy() -
			 MINE_ENERGY_LOSS(motion.explosionForce));
	
	// Reset
	motion.explosionForce = 0.0;
    }
    
    // If we get extra energy, add it in
    if (motion.addedEnergy > 0.0) {
	float newEnergy = Panel::getEnergy() + motion.addedEnergy;
	if (newEnergy > 1.0) {
	    float surplus = newEnergy - 1.0;
	    Panel::addScore(SCORE_ABSORBED_ENERGY(surplus));
	    newEnergy = 1.0;
	}
	Panel::setEnergy(newEnergy);
	
	// Reset
	motion.addedEnergy = 0.0;
    }
    
    // Decrement stealth time if any
    motion.stealthTime -= numSeconds;
    if (motion.stealthTime < 0.0)
	motion.stealthTime = 0.0;

    // Move the camera and lights
    setView();

    // Update blaster state
    blaster->update(&motion);

    // Update current display
    Panel::setSpeed(speed / MAX_SPEED);
    Sound::setContinuousPitch(shipSound, SHIP_PITCH(speed));

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets camera and lights based on current position and orientation
//
////////////////////////////////////////////////////////////////////////////

void
Ship::setView()
{
    // Update camera
    camera->position    = position;
    camera->orientation = orientation;
    
    // Update headlight
    SbVec3f direction;
    orientation.multVec(headLightDir, direction);
    headLight->direction = direction;
}
