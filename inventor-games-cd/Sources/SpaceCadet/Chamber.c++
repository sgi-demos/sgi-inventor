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
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/engines/SoInterpolate.h>
#include <Inventor/engines/SoOneShot.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText3.h>
#include "Common.h"
#include "Chamber.h"
#include "Scene.h"
#include "Sound.h"

// These are the vertices of an icosahedron on which each edge has
// been subdivided once. The first 12 points are the originals, and
// the other 30 are the midpoints of the edges.
#define NUM_VERTS 42
static float icosaVerts[NUM_VERTS][3] = {
    { 1.1547, 0, -1.51152 },
    { -0.57735, 1, -1.51152 },
    { -0.57735, -1, -1.51152 },
    { 0.934172, 1.61803, -0.356822 },
    { -1.86835, 0, -0.356822 },
    { 0.934172, -1.61803, -0.356822 },
    { -0.934172, 1.61803, 0.356822 },
    { 1.86835, 0, 0.356822 },
    { -0.934172, -1.61803, 0.356822 },
    { 0.57735, 1, 1.51152 },
    { -1.1547, 0, 1.51152 },
    { 0.57735, -1, 1.51152 },
    { 0.178411, 0.309017, -0.934172 },
    { 0.178411, -0.309017, -0.934172 },
    { 0.645498, 0.499999, -0.57735 },
    { 0.645498, -0.499999, -0.57735 },
    { 0.934173, 0, -0.356821 },
    { -0.356822, 0, -0.934172 },
    { 0.110264, 0.809017, -0.57735 },
    { -0.755762, 0.309017, -0.577349 },
    { -0.467087, 0.809017, -0.356822 },
    { -0.755762, -0.309017, -0.577349 },
    { 0.110264, -0.809017, -0.57735 },
    { -0.467087, -0.809017, -0.356822 },
    { 0, 1, 0 },
    { 0.866026, 0.499998, 0 },
    { 0.467087, 0.809017, 0.356822 },
    { -0.866026, 0.499998, 0 },
    { -0.866026, -0.499998, 0 },
    { -0.934173, 0, 0.356821 },
    { 0.866026, -0.499998, 0 },
    { 0, -1, 0 },
    { 0.467087, -0.809017, 0.356822 },
    { -0.110264, 0.809017, 0.57735 },
    { -0.645498, 0.499999, 0.57735 },
    { 0.755762, 0.309017, 0.577349 },
    { 0.755762, -0.309017, 0.577349 },
    { -0.645498, -0.499999, 0.57735 },
    { -0.110264, -0.809017, 0.57735 },
    { -0.178411, 0.309017, 0.934172 },
    { 0.356822, 0, 0.934172 },
    { -0.178411, -0.309017, 0.934172 },
};

// A random door number
#define RANDOM_DOOR_NUMBER(startLevel)					      \
	((int) RANDOM((startLevel) + 1, 2 * (NUM_VERTS + (startLevel))))

SoNode		*Chamber::root;
SoSeparator	*Chamber::doors;

////////////////////////////////////////////////////////////////////////////
//
// Initializes class.
//
////////////////////////////////////////////////////////////////////////////

void
Chamber::init(int startLevel)
{
    // Build scene graph to display status
    buildGraph(startLevel);
}

////////////////////////////////////////////////////////////////////////////
//
// Cleans up.
//
////////////////////////////////////////////////////////////////////////////

void
Chamber::cleanUp()
{
    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph.
//
////////////////////////////////////////////////////////////////////////////

void
Chamber::buildGraph(int startLevel)
{
    root = Scene::readFile("Chamber.iv");
    root->ref();

    // Add N chamber doors at the vertices of the subdivided
    // icosahedron. Each door has a static closed version and an
    // animating open version. The text label on the door has to be
    // copied so each instance can have a different number label. The
    // subgraphs that contain the motion to open the door are
    // instanced.

    doors = (SoSeparator *) Scene::find("ChamberDoors");
    SoNode *textRoot = Scene::find("ChamberDoorTextSep");

    for (int i = 0; i < NUM_VERTS; i++) {

	// Separator at top of door graph. Contains rotation to position
	// door and text
	SoSeparator *topSep = new SoSeparator;
	doors->addChild(topSep);

	// Rotation to bring door to correct spot
	SbVec3f	v(icosaVerts[i][0], icosaVerts[i][1], icosaVerts[i][2]);
	SoRotation *rot = new SoRotation;
	rot->setName("ChamberDoorRotation");
	rot->rotation = SbRotation(SbVec3f(0.0, 0.0, -1.0), v);
	topSep->addChild(rot);

	// Closed door subgraph
	SoNode *closed = Scene::find("ChamberDoorClosed");
	topSep->addChild(closed);

	// Create numeric label for door
	SoNode *textCopy = textRoot->copy();
	// Add first before searching - make sure it's ref'ed
	topSep->addChild(textCopy);
	SoText3 *text = (SoText3 *) Scene::findUnder("ChamberDoorText",
						     textCopy);
	text->string = SbString(RANDOM_DOOR_NUMBER(startLevel));
    }

    // Turn off depth-buffering when drawing the chamber spheres
    SoCallback *cb1 = (SoCallback *) Scene::find("ChamberCallback1");
    SoCallback *cb2 = (SoCallback *) Scene::find("ChamberCallback2");
    cb1->setCallback(&Chamber::depthCB, (void *) FALSE);
    cb2->setCallback(&Chamber::depthCB, (void *) TRUE);
}

////////////////////////////////////////////////////////////////////////////
//
// Returns duration (in seconds) of main chamber animation.
//
////////////////////////////////////////////////////////////////////////////

float
Chamber::getMainAnimationTime()
{
    SoOneShot *shot = (SoOneShot *) Scene::findEngine("ChamberOneShot");

    // Add a half-second for initial pause
    return (float) shot->duration.getValue().getValue() + .5;
}

////////////////////////////////////////////////////////////////////////////
//
// Returns duration (in seconds) of door chamber animation.
//
////////////////////////////////////////////////////////////////////////////

float
Chamber::getDoorAnimationTime()
{
    SoOneShot *shot = (SoOneShot *) Scene::findEngine("ChamberDoorOneShot");
    return (float) shot->duration.getValue().getValue();
}

////////////////////////////////////////////////////////////////////////////
//
// Begins animation that brings ship through chamber
//
////////////////////////////////////////////////////////////////////////////

void
Chamber::animateMain(int level)
{
    // Turn on the switch to show the chamber
    SoSwitch *chSwitch = (SoSwitch *) Scene::find("ChamberSwitch");
    chSwitch->whichChild = SO_SWITCH_ALL;

    // Display the closed doors
    SoSwitch *doorSwitch = (SoSwitch *) Scene::find("ChamberDoorSwitch");
    doorSwitch->whichChild = 0;

    // Pick one of the doors at random
    int door = (int) RANDOM(0.0, (NUM_VERTS - .00001));
    SbVec3f v(icosaVerts[door][0], icosaVerts[door][1], icosaVerts[door][2]);
    SbRotation r(SbVec3f(0.0, 0.0, -1.0), v);

    SbString doorNumberString(level);

    // Make sure all other doors have different numbers and simple text
    for (int i = 0; i < NUM_VERTS; i++) {
	if (i != door) {
	    SoSeparator *sep = (SoSeparator *) doors->getChild(i);
	    SoText3 *txt = (SoText3 *)Scene::findUnder("ChamberDoorText", sep);
	    while (txt->string[0] == doorNumberString)
		txt->string = SbString(RANDOM_DOOR_NUMBER(level));
	    txt->parts = SoText3::FRONT;
	}
    }

    // Set up the interpolation to go to that door
    SoInterpolateRotation *interp =
	(SoInterpolateRotation *) Scene::findEngine("ChamberRotate");
    interp->input1 = r;

    // Get root of chosen door's subgraph
    SoSeparator *doorRoot = (SoSeparator *) doors->getChild(door);

    // Set the text on the door with the correct level number
    SoText3 *doorText =
	(SoText3 *) Scene::findUnder("ChamberDoorText", doorRoot);
    doorText->string = SbString(level);
    doorText->parts = (SoText3::FRONT | SoText3::SIDES);

    // Set the opening door to use the chosen door's rotation...
    SoGroup *openRot = (SoGroup *) Scene::find("ChamberOpeningDoorRotGroup");
    openRot->removeAllChildren();
    openRot->addChild(Scene::findUnder("ChamberDoorRotation", doorRoot));

    // ... and its geometry
    SoSeparator *openSep = (SoSeparator *)Scene::find("ChamberOpeningDoorSep");
    openSep->removeAllChildren();
    openSep->addChild(Scene::find("ChamberDoorClosed"));
    openSep->addChild(Scene::findUnder("ChamberDoorTextSep", doorRoot));

    // Activate the one-shot engine
    SoOneShot *oneShot = (SoOneShot *) Scene::findEngine("ChamberOneShot");
    oneShot->trigger.touch();

    // Play the sounds for the motion on time delays
    Sound::playOnce("chamberMotion", 2.0);
    Sound::playOnce("chamberMotion", 6.0);
}

////////////////////////////////////////////////////////////////////////////
//
// Begins animation that opens chamber door to tunnel.
//
////////////////////////////////////////////////////////////////////////////

void
Chamber::animateDoor()
{
    // Turn off the switch to show the chamber
    SoSwitch *chSwitch = (SoSwitch *) Scene::find("ChamberSwitch");
    chSwitch->whichChild = SO_SWITCH_NONE;

    // Display just the opening door
    SoSwitch *doorSwitch = (SoSwitch *) Scene::find("ChamberDoorSwitch");
    doorSwitch->whichChild = 1;

    // Activate the one-shot engine
    SoOneShot *oneShot = (SoOneShot *)Scene::findEngine("ChamberDoorOneShot");
    oneShot->trigger.touch();

    // Play the sound
    Sound::playOnce("chamberDoorMotion");
}

////////////////////////////////////////////////////////////////////////////
//
// Callback to enable or disable depth test. The userData pointer is
// actually the Boolean flag.
//
////////////////////////////////////////////////////////////////////////////

void
Chamber::depthCB(void *userData, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
	if ((SbBool) userData)
	    glEnable(GL_DEPTH_TEST);
	else
	    glDisable(GL_DEPTH_TEST);
    }
}
