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
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoText3.h>
#include "Common.h"
#include "Meter.h"
#include "Scene.h"
#include "Panel.h"

int		Panel::level;
SbTime		Panel::elapsedTime;
int		Panel::score;
float		Panel::scoreMult;
Panel::Effect	Panel::crash;
Panel::Effect	Panel::ring;
Panel::Effect	Panel::explode;
Meter		*Panel::energyMeter;
Meter		*Panel::distanceMeter;
Meter		*Panel::speedMeter;
Meter		*Panel::centerMeter;
SoNode		*Panel::root;		// Root of scene graph
SoText2		*Panel::levelText;
SoText2		*Panel::scoreText;
SoText2		*Panel::timeText;

////////////////////////////////////////////////////////////////////////////
//
// Initializes class.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::init()
{
    scoreMult = 1.0;

    // Turn all effects off
    crash.isDisplayed = ring.isDisplayed = explode.isDisplayed = FALSE;

    // Set durations
    crash.duration	= CRASH_DISPLAY_TIME;
    ring.duration	= RING_DISPLAY_TIME;
    explode.duration	= EXPLODE_DISPLAY_TIME;

    // Build speed indicator
    energyMeter   = new Meter(1.0,  0.1, 2.0);
    distanceMeter = new Meter(0.0, -1.0, 2.0);
    speedMeter    = new Meter(0.0, -1.0, 2.0);
    centerMeter   = new Meter(0.0, -1.0, 0.8);

    // Build scene graph to display status
    buildGraph();
}

////////////////////////////////////////////////////////////////////////////
//
// Cleans up.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::cleanUp()
{
    delete energyMeter;
    delete distanceMeter;
    delete speedMeter;
    delete centerMeter;

    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::buildGraph()
{
    SoSeparator *sep;

    root = Scene::readFile("Panel.iv");
    root->ref();

    levelText = (SoText2 *) Scene::find("PanelLevelText");
    scoreText = (SoText2 *) Scene::find("PanelScoreText");
    timeText  = (SoText2 *) Scene::find("PanelTimeText");

    crash.displaySwitch   = (SoSwitch *) Scene::find("PanelCrashSwitch");
    ring.displaySwitch    = (SoSwitch *) Scene::find("PanelRingSwitch");
    explode.displaySwitch = (SoSwitch *) Scene::find("PanelExplodeSwitch");

    // Add the meter graphs
    sep = (SoSeparator *) Scene::find("PanelEnergySep");
    sep->addChild(energyMeter->getGraph());
    sep = (SoSeparator *) Scene::find("PanelDistanceSep");
    sep->addChild(distanceMeter->getGraph());
    sep = (SoSeparator *) Scene::find("PanelSpeedSep");
    sep->addChild(speedMeter->getGraph());
    sep = (SoSeparator *) Scene::find("PanelCenterSep");
    sep->addChild(centerMeter->getGraph());
}

////////////////////////////////////////////////////////////////////////////
//
// Resets all displays to initial values, except level and score,
// which remain as is.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::reset()
{
    setElapsedTime(SbTime::zero());

    speedMeter->reset();
    distanceMeter->reset();
    centerMeter->reset();
    energyMeter->reset();
}

////////////////////////////////////////////////////////////////////////////
//
// Sets current level.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::setLevel(int _level)
{
    level = _level;
    levelText->string = SbString(level);
}

////////////////////////////////////////////////////////////////////////////
//
// Sets/adds to/returns elapsed time.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::setElapsedTime(const SbTime &time)
{
    elapsedTime = time;
    timeText->string = elapsedTime.format("%m:%s");
}

void
Panel::addElapsedTime(const SbTime &timeDiff)
{
    setElapsedTime(elapsedTime + timeDiff);

    // Make sure effects are up to date
    float numSeconds = (float) timeDiff.getValue();
    updateEffect(&crash,   numSeconds);
    updateEffect(&ring,    numSeconds);
    updateEffect(&explode, numSeconds);
}

SbTime
Panel::getElapsedTime()
{
    return elapsedTime;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets/adds to current score.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::setScore(int _score)
{
    char buf[32];
    score = _score;
    sprintf(buf, "%9d", score);
    scoreText->string = buf;
}

void
Panel::addScore(int scoreDiff)
{
    int adjustedDiff = (int) (scoreDiff * scoreMult);

    setScore(score + adjustedDiff);
}

////////////////////////////////////////////////////////////////////////////
//
// Each of these sets some metered quantity to a given amount.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::setSpeed(float speed)
{
    speedMeter->setValue(speed);
}

void
Panel::setDistance(float distance)
{
    distanceMeter->setValue(distance);
}

void
Panel::setCentering(float centering)
{
    centerMeter->setValue(centering);
}

void
Panel::setEnergy(float energy)
{
    energyMeter->setValue(energy);
}

////////////////////////////////////////////////////////////////////////////
//
// Each of these returns some metered quantity.
//
////////////////////////////////////////////////////////////////////////////

float
Panel::getSpeed()
{
    return speedMeter->getValue();
}

float
Panel::getDistance()
{
    return distanceMeter->getValue();
}

float
Panel::getCentering()
{
    return centerMeter->getValue();
}

float
Panel::getEnergy()
{
    return energyMeter->getValue();
}

////////////////////////////////////////////////////////////////////////////
//
// Activates/deactivates main panel display.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::display(SbBool on)
{
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("PanelMainSwitch");

    mainSwitch->whichChild = (on ? SO_SWITCH_ALL : SO_SWITCH_NONE);
}

////////////////////////////////////////////////////////////////////////////
//
// Activates/deactivates between-level display.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::displayEndLevel(SbBool on)
{
    SoSwitch *endSwitch = (SoSwitch *) Scene::find("PanelEndLevelSwitch");

    endSwitch->whichChild = (on ? SO_SWITCH_ALL : SO_SWITCH_NONE);

    if (on) {
	// Compute scores
	int levelScore = SCORE_LEVEL_COMPLETE(level);
	int timeBonus  = SCORE_TIME_BONUS(elapsedTime.getValue(), level);

	// Set up text with the correct info
	char buf[256];
	SoText3 *endText = (SoText3 *) Scene::find("PanelEndLevelText");
	sprintf(buf, "Bonus for completing level: %d", levelScore);
	endText->string.setValue(buf);
	if (timeBonus > 0) {
	    sprintf(buf, "Bonus for time left over: %d", timeBonus);
	    endText->string.set1Value(1, buf);
	}

	addScore(levelScore + timeBonus);
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Activates display of ending graphics.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::displayEnd()
{
    // Turn off regular graphics
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("PanelMainSwitch");
    mainSwitch->whichChild = SO_SWITCH_NONE;

    // Turn on ending graphics
    SoSwitch *endSwitch = (SoSwitch *) Scene::find("PanelEndSwitch");
    endSwitch->whichChild = SO_SWITCH_ALL;

    // Set up text with the correct info
    char buf[256];
    sprintf(buf, "Final score: %d", score);
    SoText3 *endText = (SoText3 *) Scene::find("PanelEndText");
    endText->string.set1Value(0, "Energy depleted.");
    endText->string.set1Value(1, buf);
    endText->string.set1Value(2, "Try again soon.");
}

////////////////////////////////////////////////////////////////////////////
//
// Turns off all panel display.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::displayNothing()
{
    // Turn off regular graphics
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("PanelMainSwitch");
    mainSwitch->whichChild = SO_SWITCH_NONE;

    // Turn off ending graphics
    SoSwitch *endSwitch = (SoSwitch *) Scene::find("PanelEndSwitch");
    endSwitch->whichChild = SO_SWITCH_NONE;
}

////////////////////////////////////////////////////////////////////////////
//
// Begins display of an effect.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::startEffect(Effect *effect)
{
    // Begin display if not already there
    if (! effect->isDisplayed) {
	effect->isDisplayed = TRUE;
	effect->displaySwitch->whichChild = SO_SWITCH_ALL;
    }

    // Reset time
    effect->displayTime = 0.0;
}

////////////////////////////////////////////////////////////////////////////
//
// Updates display of an effect for given change in time, possibly
// ending display of effect.
//
////////////////////////////////////////////////////////////////////////////

void
Panel::updateEffect(Effect *effect, float numSeconds)
{
    if (effect->isDisplayed) {

	effect->displayTime += numSeconds;

	if (effect->displayTime >= effect->duration) {
	    effect->isDisplayed = FALSE;
	    effect->displaySwitch->whichChild = SO_SWITCH_NONE;
	}
    }
}
