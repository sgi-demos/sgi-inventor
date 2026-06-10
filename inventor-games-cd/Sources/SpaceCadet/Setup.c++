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
#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTranslation.h>
#include "Common.h"
#include "HighScore.h"
#include "Panel.h"
#include "Scene.h"
#include "Setup.h"

// Maximum starting level
#define MAX_START_LEVEL 50

const char	*Setup::playerName;
SbBool		Setup::soundOn;
SbBool		Setup::airplaneMode;
SbBool		Setup::darkMode;
int		Setup::startLevel;
SetupStartCB	*Setup::startCB;
SbBool		Setup::texEnabled;
SoNode		*Setup::root;
Setup::Hit	Setup::saveHit;

////////////////////////////////////////////////////////////////////////////
//
// Initializes class.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::init()
{
    // Build scene graph to display status
    buildGraph();

    saveHit = NONE;

    // Assume for now that textures are fast enough to be enabled. We
    // will be informed if otherwise.
    texEnabled = TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Cleans up.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::cleanUp()
{
    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::buildGraph()
{
    root = Scene::readFile("Setup.iv");
    root->ref();

    // Set up event callback node
    SoEventCallback *ecb = (SoEventCallback *) Scene::find("SetupECB");
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
			  &Setup::buttonCB);
    ecb->addEventCallback(SoKeyboardEvent::getClassTypeId(), keyboardCB);

    // Turn on regular graphics
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("SetupMainSwitch");
    mainSwitch->whichChild = SO_SWITCH_ALL;
}

////////////////////////////////////////////////////////////////////////////
//
// Returns scene graph that displays setup panel.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Setup::getGraph()
{
    // Customize and update graph first
    SoText3 *nameText = (SoText3 *) Scene::find("SetupNameText");
    nameText->string.set1Value(1, playerName);

    updateGraph();

    return root;
}

////////////////////////////////////////////////////////////////////////////
//
// Activates display of high score list.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::displayHighScores()
{
    // Turn off regular graphics
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("SetupMainSwitch");
    mainSwitch->whichChild = SO_SWITCH_NONE;

    // Turn on high score graphics
    SoSwitch *hsSwitch = (SoSwitch *) Scene::find("SetupHighScoreSwitch");
    hsSwitch->whichChild = SO_SWITCH_ALL;

    // Add ending score to list
    int newScore = HighScore::addScore(getPlayerName(),
				       Panel::getLevel(), Panel::getScore());

    // Set up text nodes with high score info. Each text node already
    // has 1 string in it, which is the heading.
    SoText3 *nameText  = (SoText3 *) Scene::find("SetupHighScoreNames");
    SoText3 *rankText  = (SoText3 *) Scene::find("SetupHighScoreRanks");
    SoText3 *levelText = (SoText3 *) Scene::find("SetupHighScoreLevels");
    SoText3 *scoreText = (SoText3 *) Scene::find("SetupHighScores");

    int numScores = HighScore::getNumScores();
    for (int i = 0; i < numScores; i++) {

	const char	*name;
	int		level, score;

	HighScore::getScore(i, name, level, score);
	
	nameText->string.set1Value(i + 1, name);
	rankText->string.set1Value(i + 1, SbString(i + 1));
	levelText->string.set1Value(i + 1, SbString(level));
	scoreText->string.set1Value(i + 1, SbString(score));
    }

    // If current score is now on the list, highlight it and write out
    // the new scores
    if (newScore >= 0) {

	// Translate highlight to correct score
	SoTranslation *xl = (SoTranslation *)
	    Scene::find("SetupHighScoreNewTranslation");
	xl->translation.setValue(0.0, -13.1 * newScore, 0.0);

	// Turn it on
	SoSwitch *scoreSwitch = (SoSwitch *)
	    Scene::find("SetupHighScoreNewSwitch");
	scoreSwitch->whichChild = SO_SWITCH_ALL;

	HighScore::write();
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for a keyboard button press or release.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::keyboardCB(void *, SoEventCallback *ecb)
{
    const SoKeyboardEvent *kbe = (const SoKeyboardEvent *) ecb->getEvent();

    if (kbe->getKey() == SoKeyboardEvent::ESCAPE ||
	kbe->getKey() == SoKeyboardEvent::Q)
	exit(0);

    ecb->setHandled();
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for a mouse button press or release.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::buttonCB(void *, SoEventCallback *ecb)
{
    // If the mouse is not over something interesting, do nothing
    const SoPickedPoint *pp = ecb->getPickedPoint();
    if (pp == NULL) {
	saveHit = NONE;
	return;
    }

    Hit hit = getHit(pp);

    // On mouse-down, see what object was hit. Wait until the mouse
    // comes up. If it's the same object, perform the correct action.

    const SoMouseButtonEvent *mbe =
	(const SoMouseButtonEvent *) ecb->getEvent();

    if (SO_MOUSE_PRESS_EVENT(mbe, ANY))
	saveHit = hit;

    else {
	if (saveHit == hit)
	    processHit(hit);

	saveHit = NONE;
    }

    ecb->setHandled();
}

////////////////////////////////////////////////////////////////////////////
//
// Finds what was hit, returning code.
//
////////////////////////////////////////////////////////////////////////////

Setup::Hit
Setup::getHit(const SoPickedPoint *pp)
{
    const SoPath *path = pp->getPath();

    const SoNode *node;

    // Look through path, from bottom to top, for a node with a name
    // that does not begin with an underscore
    for (int i = path->getLength() - 1; i >= 0; --i) {

	node = path->getNode(i);
	const SbName &nodeName = node->getName();

	if (! ! nodeName && nodeName.getString()[0] != '_')
	    break;
    }

    // If no such node was found, give up
    if (i < 0)
	return NONE;

    // Test against known names, using this array. NOTE: this array
    // has to be in the exact order of the Hit enum.
    static const char *hitNames[] = {
	"__None__",
	"SetupSound",
	"SetupAirplaneMode",
	"SetupDarkMode",
	"SetupLevelUp1",
	"SetupLevelUp5",
	"SetupLevelDown1",
	"SetupLevelDown5",
	"SetupHelp",
	"SetupStart",
	"SetupQuit",
	"SetupRestart",
    };
#define NUM_NAMES (sizeof(hitNames) / sizeof(hitNames[0]))

    const SbName &nodeName = node->getName();

    for (i = 0; i < NUM_NAMES; i++)
	if (nodeName == hitNames[i])
	    return (Hit) i;

    return NONE;

#undef NUM_NAMES
}

////////////////////////////////////////////////////////////////////////////
//
// Restarts after game has completed.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::restart()
{
    // Turn off high score graphics
    SoSwitch *hsSwitch = (SoSwitch *) Scene::find("SetupHighScoreSwitch");
    hsSwitch->whichChild = SO_SWITCH_NONE;

    // Bring up the original panel
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("SetupMainSwitch");
    mainSwitch->whichChild = SO_SWITCH_ALL;
}

////////////////////////////////////////////////////////////////////////////
//
// Processes a hit.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::processHit(Hit hit)
{
    switch (hit) {

      case NONE:
	break;

      case SOUND:
	setSound(! isSound());
	updateGraph();
	break;

      case AIRPLANE_MODE:
	setAirplaneMode(! isAirplaneMode());
	updateGraph();
	break;

      case DARK_MODE:
	setDarkMode(! isDarkMode());
	updateGraph();
	break;

      case LEVEL_UP_1:
	if (getStartLevel() < MAX_START_LEVEL) {
	    setStartLevel(getStartLevel() + 1);
	    updateGraph();
	}
	break;

      case LEVEL_UP_5:
	if (getStartLevel() < MAX_START_LEVEL) {
	    int newStartLevel = getStartLevel() + 5;
	    if (newStartLevel > MAX_START_LEVEL)
		newStartLevel = MAX_START_LEVEL;
	    setStartLevel(newStartLevel);
	    updateGraph();
	}
	break;

      case LEVEL_DOWN_1:
	if (getStartLevel() > 1) {
	    setStartLevel(getStartLevel() - 1);
	    updateGraph();
	}
	break;

      case LEVEL_DOWN_5:
	if (getStartLevel() > 1) {
	    int newStartLevel = getStartLevel() - 5;
	    if (newStartLevel < 1)
		newStartLevel = 1;
	    setStartLevel(newStartLevel);
	    updateGraph();
	}
	break;

      case HELP:
	displayHelp();
	break;

      case START:
	(*startCB)();
	break;

      case QUIT:
	exit(0);

      case RESTART:
	restart();
	break;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Updates scene graph based on current settings.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::updateGraph()
{
    SoSwitch *soundSw = (SoSwitch *) Scene::find("SetupSoundSwitch");
    soundSw->whichChild = (isSound() ? SO_SWITCH_ALL : SO_SWITCH_NONE);

    SoSwitch *airSw = (SoSwitch *) Scene::find("SetupAirplaneModeSwitch");
    airSw->whichChild = (isAirplaneMode() ? SO_SWITCH_ALL : SO_SWITCH_NONE);

    SoSwitch *darkSw = (SoSwitch *) Scene::find("SetupDarkModeSwitch");
    darkSw->whichChild = (isDarkMode() ? SO_SWITCH_ALL : SO_SWITCH_NONE);

    SoText3 *levelText = (SoText3 *) Scene::find("SetupStartLevelText");
    levelText->string = SbString(getStartLevel());
}

////////////////////////////////////////////////////////////////////////////
//
// Displays help screen.
//
////////////////////////////////////////////////////////////////////////////

void
Setup::displayHelp()
{
    if (system("showcase -v SpaceCadetHelp.sho"))
	fprintf(stderr, "Can't display help file\n");
}
