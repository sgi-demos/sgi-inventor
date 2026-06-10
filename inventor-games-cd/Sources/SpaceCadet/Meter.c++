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
#include <Inventor/nodes/SoTransform.h>
#include "Common.h"
#include "Meter.h"
#include "Scene.h"

// Margin of bar within frame
#define MARGIN	.01

////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
////////////////////////////////////////////////////////////////////////////

Meter::Meter(float start, float _minAlarm, float _maxAlarm)
{
    initValue = start;

    minAlarm = _minAlarm;
    maxAlarm = _maxAlarm;

    inAlarm = FALSE;

    // Build scene graph to display status
    buildGraph();

    // Make sure setValue takes effect
    curValue = start + 100.0;
    setValue(start);
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Meter::~Meter()
{
    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Returns scene graph displaying status.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Meter::getGraph()
{
    return root;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph.
//
////////////////////////////////////////////////////////////////////////////

void
Meter::buildGraph()
{
    root = Scene::readFile("Meter.iv");
    root->ref();

    barXform    = (SoTransform *) Scene::find("MeterBarScale");
    alarmSwitch = (SoSwitch    *) Scene::find("MeterAlarmSwitch");
}

////////////////////////////////////////////////////////////////////////////
//
// Changes current meter value.
//
////////////////////////////////////////////////////////////////////////////

void
Meter::setValue(float newValue)
{
    if (newValue == curValue)
	return;

    curValue = newValue;

    float t = curValue;

    if (t <= 0.0)
	t = 0.0001;	// Avoid scale by 0
    else if (t > 1.0)
	t = 1.0;

    // Scale meter bar
    barXform->scaleFactor.setValue(t, 1, 1);

    // Change color if value is in or out of alarm zone
    if (curValue < minAlarm || curValue > maxAlarm) {
	if (! inAlarm) {
	    alarmSwitch->whichChild = 0;
	    inAlarm = TRUE;
	}
    }
    else {
	if (inAlarm) {
	    alarmSwitch->whichChild = SO_SWITCH_NONE;
	    inAlarm = FALSE;
	}
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Resets to initial value.
//
////////////////////////////////////////////////////////////////////////////

void
Meter::reset()
{
    setValue(initValue);
}
