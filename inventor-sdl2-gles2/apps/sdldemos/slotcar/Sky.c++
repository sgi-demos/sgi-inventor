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
 * Copyright (C) 1990,91,92,93,94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Classes:
 |	Sky
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <GL/gl.h>

#include "Sky.h"

SO_NODE_SOURCE(Sky);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Init Sky
//

void
Sky::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_INIT_CLASS(Sky, SoNode, "Node");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor; setup fields, etc.
//

Sky::Sky()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(Sky);

    SO_NODE_ADD_FIELD(horizonColor, (.8, .8, 1.0));
    SO_NODE_ADD_FIELD(horizonHeight, (100.0));
    SO_NODE_ADD_FIELD(skyBottomColor, (0.0, 0.4, 0.7));
    SO_NODE_ADD_FIELD(skyTopColor, (0.0, 0.0, 0.44));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//

Sky::~Sky()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Render
//

void
Sky::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();
    state->push();

    // Turn off lighting
    SoLightModelElement::set(state, this,
			     SoLightModelElement::BASE_COLOR);

    // Don't z-buffer
    glDisable(GL_DEPTH_TEST);

#ifdef SDL_PORT
    // The SoLightModelElement::set above only takes GL effect when the
    // lazy element next sends state for a shape - it does nothing for
    // the raw glBegin/glColor quads below. With GL_LIGHTING enabled the
    // per-vertex colors are ignored and the sky draws black. Force the
    // fixed-function state directly and restore it afterwards.
    GLboolean wasLit = glIsEnabled(GL_LIGHTING);
    glDisable(GL_LIGHTING);
#endif
    
    // Get the view volume
    const SbViewVolume &vv = SoViewVolumeElement::get(state);
    
    // Figure out stuff:
    const SbVec3f &eye = vv.getProjectionPoint();
    const SbVec3f &eyeDir = vv.getProjectionDirection();

    // Point in the middle of the view volume:
    SbVec3f midVolume = eye + eyeDir*(vv.getNearDist() +
				      vv.getDepth()*.98);
    SbVec3f midVolume0(midVolume[0], 0.0, midVolume[2]);
    
    // We'll construct the sky polygons in world space, located at
    // mid-view volume, based at y==0, oriented appropriately in x/z:

    // ??? This will not work for arbitrary eye orientations!  In
    // particular, looking straight up or down the Y axis will cause
    // problems.
    SbVec3f perpendicular(-eyeDir[2], 0.0, eyeDir[0]);
    perpendicular.normalize();
    
    const float bigWidth = 10000.0;

    // Just draw big polygons a good distance along perpendicular,
    // from the midVolume point:
    SbVec3f leftHorizon, rightHorizon;
    SbVec3f leftBottomSky, rightBottomSky;
    SbVec3f leftTopSky, rightTopSky;
    
    leftHorizon = midVolume0 + perpendicular*bigWidth;
    rightHorizon = midVolume0 - perpendicular*bigWidth;
    leftBottomSky = leftHorizon +
	SbVec3f(0.0, horizonHeight.getValue(), 0.0);
    rightBottomSky = rightHorizon +
	SbVec3f(0.0, horizonHeight.getValue(), 0.0);
    
    glBegin(GL_QUADS);
    glColor3fv(horizonColor.getValue().getValue());
    glVertex3fv(leftHorizon.getValue());
    glVertex3fv(rightHorizon.getValue());
    glColor3fv(skyBottomColor.getValue().getValue());
    glVertex3fv(rightBottomSky.getValue());
    glVertex3fv(leftBottomSky.getValue());
    
//    leftTopSky = leftHorizon + SbVec3f(0.0, bigWidth, 0.0);
//    rightTopSky = rightHorizon + SbVec3f(0.0, bigWidth, 0.0);
    leftTopSky = leftHorizon + SbVec3f(0.0, bigWidth, 500.0);
    rightTopSky = rightHorizon + SbVec3f(0.0, bigWidth, 500.0);
    
    glVertex3fv(leftBottomSky.getValue());
    glVertex3fv(rightBottomSky.getValue());
    glColor3fv(skyTopColor.getValue().getValue());
    glVertex3fv(leftTopSky.getValue());
    glVertex3fv(rightTopSky.getValue());
    
    glEnd();
    
#ifdef SDL_PORT
    if (wasLit)
	glEnable(GL_LIGHTING);
#endif
    glEnable(GL_DEPTH_TEST);

    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This node does not affect state.
//

SbBool
Sky::affectsState() const
//
////////////////////////////////////////////////////////////////////////
{
    return FALSE;
}

