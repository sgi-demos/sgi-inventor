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
#include "pbn.h"

////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
////////////////////////////////////////////////////////////////////////////

Change::Change()
{
    changes = NULL;
    next = prev = NULL;
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Change::~Change()
{
    if (changes != NULL) {

	Square *sq, *nextSq;
	for (sq = changes; sq != NULL; sq = nextSq) {
	    nextSq = sq->next;
	    delete sq;
	}

	delete [] changes;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Adds a change to a square to the list in this instance.
//
////////////////////////////////////////////////////////////////////////////

void
Change::add(int row, int col, SqState oldState, SqState newState)
{
    Square *sq = new Square;

    sq->row = row;
    sq->col = col;
    sq->oldState = oldState;
    sq->state = newState;

    // Add to list
    sq->next = changes;
    changes = sq;
}
