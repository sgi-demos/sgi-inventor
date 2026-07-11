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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Classes:
 |	ButtonStrip
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>

#include <Inventor/actions/SoGetBoundingBoxAction.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/SoLists.h>

#include "Buttons.h"
#include "ButtonStrip.h"
#include "LinkClasses.h"

ButtonStrip::ButtonStrip( Widget parent, const char *name,
    SbBool buildInsideParent)
{
    // Build the renderArea ...
#ifdef SDL_PORT
	renderArea = new SoSDLRenderArea("Linkatron Parts", 800, 130);
	(void) parent; (void) name; (void) buildInsideParent;
#else
	renderArea = new SoXtRenderArea(parent,name,buildInsideParent);
#endif

    // Make allScene
	allScene = new SoSeparator;
	allScene->ref();

    // Make camera and lightModel.
	camera = new SoOrthographicCamera;
	allScene->addChild( camera );
	camera->position     = SbVec3f( 0, 0, 10);
	camera->nearDistance = 0.01;
	camera->farDistance  = 20;
	camera->height       = 2;

	lightModel = new SoLightModel;
	allScene->addChild( lightModel );
	lightModel->model = SoLightModel::BASE_COLOR;
	lightModel->setOverride( TRUE );

    // Make empty button and transform lists.
	buttonList = new SoNodeList;

    // Make the backing and backingActive
	SoNode *backGeom = SoNode::getByName("buttonStripBacking");
	if (backGeom) {
	    backing = new SoGroup;
	    backing->ref();
	    // It should fit in a (2x2x2) square
	    // Translate it back 2 in Z so the icon will be in front of it.
	    SoTranslation *xl = new SoTranslation;
	    backing->addChild( xl );
	    xl->translation = SbVec3f(0,0,-2);
	    // Then the geom.
	    backing->addChild( backGeom );
	}
	else
	    backing = NULL;

	SoNode *backGeomActive = SoNode::getByName("buttonStripBackingActive");
	if (backGeomActive) {
	    backingActive = new SoGroup;
	    backingActive->ref();
	    // It should fit in a (2x2x2) square
	    // Translate it back 2 in Z so the icon will be in front of it.
	    SoTranslation *xl = new SoTranslation;
	    backingActive->addChild( xl );
	    xl->translation = SbVec3f(0,0,-2);
	    // Then the geom.
	    backingActive->addChild( backGeomActive );
	}
	else
	    backingActive = NULL;

    // Set the scene graph.
        renderArea->setSceneGraph(allScene);
}

ButtonStrip::~ButtonStrip()
{
    delete renderArea;
    delete buttonList;
    allScene->unref();
    if (backing) backing->unref();
    if (backingActive) backingActive->unref();
}

void
ButtonStrip::addButton( SoNode *icon, SoNode *iconActive,
			SoDraggerCB *pressedCallback, void *userData )
{
    if (icon) icon->ref();
    if (iconActive) iconActive->ref();

    Button *newButton = new Button;

    // Inactive scene graph:
    SoSeparator *sep = new SoSeparator;
    sep->ref();
	// First add icon:
	if (icon)
	    sep->addChild(icon);
	// Then add the buttonBacking.
	if (backing)
	    sep->addChild(backing);

    // Active scene graph:
    SoSeparator *activeSep = new SoSeparator;
    activeSep->ref();
	// First add iconActive:
	if (iconActive)
	    activeSep->addChild(iconActive);
	if (backingActive)
	    activeSep->addChild(backingActive);

    newButton->setPart( "buttonGeom", sep );
    newButton->setPart( "buttonActiveGeom", activeSep );

    // Get rid of the default text.
    newButton->setPart( "buttonText", NULL );
    newButton->setPart( "buttonActiveText", NULL );

    if ( pressedCallback )
        newButton->addStartCallback( pressedCallback, userData );

    addButton( newButton );

    if (icon) icon->unref();
    if (iconActive) iconActive->unref();
    if (sep) sep->unref();
    if (activeSep) activeSep->unref();
}

void
ButtonStrip::addButton( Button *newButton )
{
    if (!newButton)
	return;

    newButton->ref();

    // All buttons need to be scaled to fit in a (2,2) in (x,y) square. 
    // Then, they spaced 2 units apart in X.

	SbBox3f buttonBounds;
	SoGetBoundingBoxAction bba(SbViewportRegion(0,0));
	bba.apply( newButton );
	buttonBounds = bba.getBoundingBox();
	if (buttonBounds.isEmpty()) {
	    newButton->unrefNoDelete();
	    return;
	}

    // Add the button to the list.
        buttonList->append( newButton );

    // Now, get it in the scene:

	// If not the first one, translate the clump over 2 units.
	if (buttonList->getLength() != 1) { 
	    SoTranslation *xl = new SoTranslation;
	    allScene->addChild( xl );
	    xl->translation = SbVec3f( 2, 0, 0);
	}

	// Add the button itself.
	allScene->addChild( newButton );

    // Adjust the camera.
	int numButs = buttonList->getLength();
	camera->position = SbVec3f( (numButs - 1),   0, 10);

    newButton->unref();
}
