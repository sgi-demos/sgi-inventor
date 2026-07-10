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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	LODD : faster, dumber level of detail.  Based on code for
 |      SoLevelOfDetail.  This version just switches based on how
 |      near/far the origin in local space is to the eyepoint.
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "LODD.h"

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/misc/SoState.h>

#ifndef DEBUG
#define NDEBUG
#endif
#include <assert.h>

SO_NODE_SOURCE(LODD);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Init LODD class
//

void
LODD::initClass()

//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_INIT_CLASS(LODD, SoGroup, "Group");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

LODD::LODD()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(LODD);
    SO_NODE_ADD_FIELD(distance, (0));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

LODD::~LODD()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements typical traversal, determining child to traverse
//    based on distance from camera
//
// Use: extender

void
LODD::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		childToTraverse;
    int		numKids  = getNumChildren();
    int		numDistances = distance.getNum();
    SoState	*state   = action->getState();

    // If no children or 1 child, decision is easy
    if (numKids == 0)
	return;

    if (numKids == 1 || numDistances == 0)
	childToTraverse = 0;

    int i;

    // Ok, (0,0,0) transformed into world space just turns out to
    // be the translation part of the current model
    // matrix:
    const SbMatrix &modelMtx = SoModelMatrixElement::get(state);
    SbVec3f worldPt( modelMtx[3][0], modelMtx[3][1], modelMtx[3][2]);

    // And find out where the eye is in world space:
    SbMatrix viewMtx = SoViewingMatrixElement::get(state).inverse();
    SbVec3f eyePt( viewMtx[3][0], viewMtx[3][1], viewMtx[3][2]);
	
    // Figure out distance:
    float d = (worldPt - eyePt).length();

    // Figure out how close we are...
    for (i = 0; i < numDistances; i++)
	if (d < distance[i])
	    break;

    // Make sure we didn't go off the deep end
    if (i >= numKids)
	i = numKids - 1;

    childToTraverse = i;

    // Traverse just the one kid
    state->push();
    children->traverse(action, childToTraverse, childToTraverse);
    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements callback action for LODD nodes.
//
// Use: extender

void
LODD::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    LODD::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for computing bounding box.  Computes bbox of ALL kids.
//
// Use: extender

void
LODD::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    action->getState()->push();
    SoGroup::getBoundingBox(action);
    action->getState()->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements getMatrix action.
//
// Use: extender

void
LODD::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    // Only need to compute matrix if this node is in the middle of
    // the current path chain. We don't need to push or pop the state,
    // since this shouldn't have any effect on other nodes being
    // traversed.

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	children->traverse(action, 0, indices[numIndices - 1]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for rendering. This uses the screen distance comparison.
//
// Use: extender

void
LODD::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    LODD::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking.
//
// Use: extender

void
LODD::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    LODD::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements search action for LODD nodes. This determines if
//    the LODD should be searched. If so, this calls the search
//    method for SoGroup to do the work.
//
// Use: extender

void
LODD::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	doSearch = TRUE;

    // See if we're supposed to search only if the stuff under the
    // LODD is relevant to the search path (stolen from
    // SoSeparator)

    if (! action->isSearchingAll()) {
	int		numIndices;
	const int	*indices;

	// Search through this LODD node only if not searching along
	// a path or this node is on the path
	if (action->getPathCode(numIndices, indices) == SoAction::OFF_PATH)
	    doSearch = FALSE;
    }

    if (doSearch) {
	action->getState()->push();
	SoGroup::search(action);
	action->getState()->pop();
    }
}
