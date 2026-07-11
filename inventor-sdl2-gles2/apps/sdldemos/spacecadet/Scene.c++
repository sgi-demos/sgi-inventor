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
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/nodes/SoSeparator.h>
#include "Common.h"
#include "Scene.h"

////////////////////////////////////////////////////////////////////////////
//
// Reads scene from file.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Scene::readFile(const char *filename)
{
    SbString	path;
    SoInput	in;

    path = "scenes/";
    path += filename;

    if (! in.openFile(path.getString())) {
	fprintf(stderr, "Can't open file \"%s\"\n", path.getString());
	return NULL;
    }

    SoSeparator	*root = SoDB::readAll(&in);

    if (root == NULL)
	fprintf(stderr, "Can't read from file \"%s\"\n", path.getString());

    return root;
}

////////////////////////////////////////////////////////////////////////////
//
// Reads scene from character string.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Scene::readString(const char *string)
{
    SoInput in;
    in.setBuffer((void *) string, strlen(string));

    SoSeparator	*root = SoDB::readAll(&in);

    if (root == NULL)
	fprintf(stderr, "Can't read from string\n");

    return root;
}

////////////////////////////////////////////////////////////////////////////
//
// Finds node by name.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Scene::find(const char *nodeName)
{
    SoNode *node = SoNode::getByName(nodeName);

    if (node == NULL)
	fprintf(stderr, "Can't find node named \"%s\"\n", nodeName);

    return node;
}

////////////////////////////////////////////////////////////////////////////
//
// Finds node by name under given root.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Scene::findUnder(const char *nodeName, SoNode *root)
{
    SoSearchAction sa;

    sa.setInterest(SoSearchAction::FIRST);
    sa.setName(nodeName);
    sa.setSearchingAll(TRUE);

    sa.apply(root);

    if (sa.getPath() == NULL)
	fprintf(stderr, "Can't find node named \"%s\" under node %#x\n",
		nodeName, root);

    return sa.getPath()->getTail();
}

////////////////////////////////////////////////////////////////////////////
//
// Finds engine by name.
//
////////////////////////////////////////////////////////////////////////////

SoEngine *
Scene::findEngine(const char *engineName)
{
    SoEngine *engine = SoEngine::getByName(engineName);

    if (engine == NULL)
	fprintf(stderr, "Can't find engine named \"%s\"\n", engineName);

    return engine;
}
