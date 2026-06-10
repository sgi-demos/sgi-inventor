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
    card.c++ - Christmas card

    Tim Heidmann
    December 22, 1993
*/

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/sensors/SoSensors.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoDB.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbTime.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoInfo.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <swirl.h>
#include <sincos.h>
#include <flakeset.h>


// Global variables
Widget mainWindow;
SoXtRenderArea *renderArea;
char *cmdName, cardFileName[128];
SoSeparator *root;
SoTransform *silXform;
SoTransform *domeXform;
SoRotationXYZ *rotx, *roty, *rotz, *unrotx, *unroty, *unrotz;
SoCallback *flakeCB;
SbTime lastTime;
float agitCoef = 1.0;
int mouseButton;
SbVec2s mousePos, lastMousePos, mouseDownPos;
SbVec3f curRot(0.0, 0.0, 0.0);
SbVec3f downRot;
SoIdleSensor *drawer;
SoIdleSensor *sens;

#define MAXSWIRLS 10
int nSwirls = 0;
Swirl *swirl[MAXSWIRLS];

int nPoints = 1000; // number of flakes requested
float fallMin = 0.001, fallRange = 0.003; // rate of drift down
FlakeSet *flakes = NULL;

void ParseArgs(int c, char *v[]);
void InitRenderArea();
void SetupFile(char *);
void ParseInfo(SoPath *p);
void HandleMouse(void *, SoEventCallback *);
void DrawIt(void *, SoSensor *);
void MoveIt(void *d, SoSensor *);
void DrawFlakes(void *d, SoAction *);
void Wmzmzfzc(void *, SoAction *);


main(int argc, char *argv[]) {
    ParseArgs(argc, argv);
    InitRenderArea();
    SetupFile(cardFileName);
    SoXt::show(mainWindow);
    SoXt::mainLoop();
}

void
ParseArgs(int c, char *v[]) {
    cmdName = v[0];
    strcpy(cardFileName, c>1 ? v[1] : "./card.iv");
}

void
InitRenderArea() {
    // Initialize window, create render area
    mainWindow = SoXt::init(cmdName);
    renderArea = new SoXtRenderArea( mainWindow );
    renderArea->setTitle("Season's Greetings");
    renderArea->setAutoRedraw(FALSE);
    renderArea->setClearBeforeRender(FALSE);
    renderArea->show();
}

void
SetupFile(char *name) {
    // Set up graph root; Set up to receive events
    root = new SoSeparator;
    root->ref();
    renderArea->setSceneGraph(root);
    SoEventCallback *ecb = new SoEventCallback;
    root->addChild(ecb);
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), HandleMouse);
    ecb->addEventCallback(SoLocation2Event::getClassTypeId(), HandleMouse);
    ecb->grabEvents();

    // Read a scene file
    SoInput myFile;
    if (!myFile.openFile(name)) {
	fprintf(stderr, "Can't open scene file %s\n", name);
	exit(1);
    }
    SoNode *inNode;
    if (! SoDB::read(&myFile, inNode)) {
	fprintf(stderr, "Error reading scene file %s\n", name);
	exit(1);
    }
    root->addChild(inNode);

    // Find paths to all labels; find important labelled nodes
    SoSearchAction sa;
    int i;
    sa.setType(SoLabel::getClassTypeId());
    sa.setInterest( SoSearchAction::ALL);
    sa.setSearchingAll(TRUE);
    sa.apply(root);

    SoGroup *theGroup = (SoGroup *) SoNode::getByName("dome");
    rotx = new SoRotationXYZ;
    roty = new SoRotationXYZ;
    rotz = new SoRotationXYZ;
    theGroup->addChild(rotz);
    theGroup->addChild(rotx);
    theGroup->addChild(roty);
    rotx->axis.setValue(SoRotationXYZ::X);
    roty->axis.setValue(SoRotationXYZ::Y);
    rotz->axis.setValue(SoRotationXYZ::Z);

    theGroup = (SoGroup *) SoNode::getByName("silhouette");
    unrotx = new SoRotationXYZ;
    unroty = new SoRotationXYZ;
    unrotz = new SoRotationXYZ;
    theGroup->addChild(unroty);
    theGroup->addChild(unrotx);
    theGroup->addChild(unrotz);
    unrotx->axis.setValue(SoRotationXYZ::X);
    unroty->axis.setValue(SoRotationXYZ::Y);
    unrotz->axis.setValue(SoRotationXYZ::Z);

    flakeCB = (SoCallback *) SoNode::getByName("flakes");

    // Find and parse info nodes
    sa.setType(SoInfo::getClassTypeId());
    sa.setInterest( SoSearchAction::ALL);
    sa.setSearchingAll(TRUE);
    sa.apply(root);
    for (i = 0; i < sa.getPaths().getLength(); i++)
	ParseInfo(sa.getPaths()[i]);

    // Make the snowflakes and set up the callback to draw them
    if (flakeCB != NULL) {
	flakes = new FlakeSet(nPoints, fallMin, fallRange);
	flakeCB->setCallback(DrawFlakes, flakes);
    }

    // Create IdleSensor's to draw, make the cube spin and image move.
    lastTime = SbTime::getTimeOfDay();
    drawer = new SoIdleSensor(DrawIt, root);
    drawer->schedule();
    sens = new SoIdleSensor(MoveIt, NULL);
    sens->schedule();
}


void
ParseInfo(SoPath *p) {
    char cmd[64];
    const SbString *s = &((SoInfo *) p->getTail())->string.getValue();
    sscanf(s->getString(), "%s", cmd);
    
    if (strcmp(cmd, "nflakes") == 0) {
	sscanf(s->getString(), "%*s %d", &nPoints);


    } else if (strcmp(cmd, "swirl") == 0) {
	// Read pivot point - x y z
	SbVec3f v;
	Swirl *thisSwirl;

	sscanf(s->getString(), "%*s %f %f %f", &v[0], &v[1], &v[2]);
	thisSwirl = swirl[nSwirls++] = new Swirl;
	thisSwirl->setPivot(v);


    } else if (strcmp(cmd, "arm") == 0) {
	// Read a swirl arm description
	int axis;
	float radius, hispeed, speed, hidecay, decay, holdtime;

	if (nSwirls > 0) {
	    sscanf(s->getString(), "%*s %d %f %f %f %f %f %f",
		&axis, &radius, &hispeed, &speed, &hidecay, &decay, &holdtime);
	    swirl[nSwirls-1]->addArm(axis, radius,
		hispeed, speed, hidecay, decay, holdtime);
	}


    } else if (strcmp(cmd, "fall") == 0) {
	sscanf(s->getString(), "%*s %f %f", &fallMin, &fallRange);


    } else if (strcmp(cmd, "wmzmzfzc") == 0) {
	int wm, zm, zf, zc;
	sscanf(s->getString(), "%*s %d %d %d %d", &wm, &zm, &zf, &zc);
	SoCallback *cb = new SoCallback;
	((SoGroup *)(p->getNodeFromTail(1)))->addChild(cb);
	cb->setCallback(Wmzmzfzc, (void *)(wm<<3|zm<<2|zf<<1|zc));


    }
    else if (strcmp(cmd, "coefs") == 0) {
	sscanf(s->getString(), "%*s %f", &agitCoef);


    }
}
    
void
HandleMouse(void *, SoEventCallback *ecb) {
    if (ecb->getEvent()->isOfType(SoMouseButtonEvent::getClassTypeId())) {
	mouseButton = SoMouseButtonEvent::isButtonPressEvent(
	    ecb->getEvent(), SoMouseButtonEvent::ANY);
	mouseDownPos = ecb->getEvent()->getPosition();
	lastMousePos = mouseDownPos;
	downRot = curRot;
    } else if (ecb->getEvent()->isOfType(SoLocation2Event::getClassTypeId())) {
	mousePos = ecb->getEvent()->getPosition();
    }
    
    ecb->setHandled();
}


void
DrawIt(void *, SoSensor *) {
    renderArea->render();
    drawer->schedule();
}


int mouseButtonDown = 0;
SbVec3f rotHiLim(1.0, 1.0, 1.0), rotLoLim(0.0, -1.0, -1.0);
SbVec3f downPoint;
int idleTheta = 0;
float ic[3][4] = {
    0.10, 0.01, 0.0, 0.20,
    0.30, 0.01, 0.3, 0.00,
    0.02, 0.01, 0.7, 0.00
};

void
MoveIt(void *, SoSensor *) {

    SbTime thisTime = SbTime::getTimeOfDay();
    float interval = (thisTime - lastTime).getValue();
    lastTime = thisTime;
    float agitation;

    SbVec2s delta;
    if (mouseButton) {
	// Calculate mouse motion; Run spring model on actual rotation.
	delta = mousePos - mouseDownPos;
	SbVec3f targetRot;
	targetRot.setValue(downRot[0] + -0.01  * delta[1],
			   downRot[1] +  0.003 * delta[1],
			   downRot[2] + -0.01  * delta[0]);

	for (int i=2; i>= 0; i--) {
	    if (targetRot[i] > rotHiLim[i])
		targetRot[i] = rotHiLim[i];
	    else if (targetRot[i] < rotLoLim[i])
		targetRot[i] = rotLoLim[i];
	    curRot[i] += 0.5 * (targetRot[i] - curRot[i]);
	}

	// Amount of agitation related to mouse motion
	delta = mousePos - lastMousePos;
	agitation = 0.003 * (abs(delta[0]) + abs(delta[1]));
	if (agitation > 1.0) agitation = 1.0;
	lastMousePos = mousePos;

    } else {
	// Run spring model on a drifting rotation
	mouseButtonDown = FALSE;
	idleTheta++;
	int i;
	for (i=2; i>=0; i--) {
	    float ss, cc, r;
	    sincos.eval(ic[i][1]*idleTheta + ic[i][2], ss, cc);
	    r = ic[i][0]*ss + ic[i][3];
	    curRot[i] += 0.2 * (r - curRot[i]);
	}
    }

    // Set Euler angle rotations in SoRotationXYZ nodes.
    rotx->angle = curRot[0];
    roty->angle = curRot[1];
    rotz->angle = curRot[2];
    unrotx->angle = -curRot[0];
    unroty->angle = -curRot[1];
    unrotz->angle = -curRot[2];

    // Run the snowflake model
    if (flakes != NULL) {
	if (mouseButton) flakes->randomize(agitCoef * agitation);
	for (int i=0; i<nSwirls; i++) {
	    if (mouseButton)
		swirl[i]->attack(interval, agitation);   // Swirl the model up
	    else
		swirl[i]->decay(interval);               // Let it calm down

	    flakes->swirl(*swirl[i], interval);          // Move the flakes
	}
	flakes->fall(interval);
	flakes->limit();
    }
 
    sens->schedule();
}

void
DrawFlakes(void *d, SoAction *action) {
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
	// Draw those flakes!
	((FlakeSet *) d)->draw();
	
	// Invalidate the state so that a cache is not made!
	SoCacheElement::invalidate(action->getState());
    }
}

void
Wmzmzfzc(void *d, SoAction *) {
    int i = (int) d;

    if (i & 0x8)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    else
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    if (i & 0x4)
	glDepthMask(GL_TRUE);
    else 
	glDepthMask(GL_FALSE);

    if (i & 0x2)
	glDepthFunc(GL_LEQUAL);
    else
	glDepthFunc(GL_ALWAYS);

    if (i & 0x1) 
	glClearDepth(1.0);
}
