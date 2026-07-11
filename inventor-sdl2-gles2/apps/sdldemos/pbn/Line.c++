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

Line::Line()
{
    squares = NULL;
    spec    = NULL;
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Line::~Line()
{
    if (squares != NULL)
	delete [] squares;

    if (spec != NULL)
	delete spec;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets length and allocates squares.
//
////////////////////////////////////////////////////////////////////////////

void
Line::setLength(int _length)
{
    length  = _length;
    squares = new Square * [length];
}

////////////////////////////////////////////////////////////////////////////
//
// Copies from another line of same length.
//
////////////////////////////////////////////////////////////////////////////

void
Line::copy(const Line *from)
{
    for (int i = 0; i < length; i++)
	*(squares[i]) = *(from->squares[i]);

    // Copy spec
    if (from->spec == NULL)
	spec = NULL;
    else {
	spec = new Spec(from->spec->num);
	for (int i = 0; i < spec->num; i++)
	    spec->runs[i] = from->spec->runs[i];
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Returns index of first square that has the given state, starting at
// the given index, going forward or backward. Returns -1 if not found.
//
////////////////////////////////////////////////////////////////////////////

int
Line::findMatchForward(int start, SqState state)
{
    for (int sq = start; sq < length; sq++)
	if (squares[sq]->state == state)
	    return sq;

    // Not found
    return -1;
}

int
Line::findMatchBackward(int start, SqState state)
{
    for (int sq = start; sq >= 0; sq--)
	if (squares[sq]->state == state)
	    return sq;

    return -1;
}

////////////////////////////////////////////////////////////////////////////
//
// Same, but looks for the first square that doesn't match.
//
////////////////////////////////////////////////////////////////////////////

int
Line::findNonMatchForward(int start, SqState state)
{
    for (int sq = start; sq < length; sq++)
	if (squares[sq]->state != state)
	    return sq;

    // Not found
    return -1;
}

int
Line::findNonMatchBackward(int start, SqState state)
{
    for (int sq = start; sq >= 0; sq--)
	if (squares[sq]->state != state)
	    return sq;

    return -1;
}

////////////////////////////////////////////////////////////////////////////
//
// Returns end of run of contiguous squares (of any state)
// starting at given square.
//
////////////////////////////////////////////////////////////////////////////

int
Line::findRunForward(int start)
{
    SqState matchState = squares[start]->state;

    for (int sq = start + 1; sq < length; sq++)
	if (squares[sq]->state != matchState)
	    return sq - 1;

    // Run goes to end of line
    return length - 1;
}

int
Line::findRunBackward(int start)
{
    SqState matchState = squares[start]->state;

    for (int sq = start - 1; sq >= 0; sq--)
	if (squares[sq]->state != matchState)
	    return sq + 1;

    // Run goes to beginning of line
    return 0;
}

////////////////////////////////////////////////////////////////////////////
//
// Looks for enough room in the line for a filled run of the given
// length, starting at the given square. Returns the index of the
// first square for the room, or -1 if there is none.
//
////////////////////////////////////////////////////////////////////////////

int
Line::findRoomForward(int start, int runLength)
{
    int startSquare = start, sq;

    while (TRUE) {

	// Find first square that could contain a run, starting at given one
	startSquare = findNonMatchForward(startSquare, EMPTY);

	// No such square? Error, so stop
	if (startSquare < 0)
	    return -1;

	// See if there's enough room starting at that square
	for (sq = 1; sq < runLength; sq++) {
	    if (startSquare + sq >= length)
		return -1;
	    if (squares[startSquare + sq]->state == EMPTY)
		break;
	}

	// Can fit?
	if (sq == runLength)
	    return startSquare;

	// Can't fit? Keep going after next empty square
	startSquare += sq + 1;
    }

    // Didn't find one?
    return -1;
}

int
Line::findRoomBackward(int start, int runLength)
{
    int startSquare = start, sq;

    while (TRUE) {

	// Find first square that could contain a run, starting at given one
	startSquare = findNonMatchBackward(startSquare, EMPTY);

	// No such square? Error, so stop
	if (startSquare < 0)
	    return -1;

	// See if there's enough room starting at that square
	for (sq = 1; sq < runLength; sq++) {
	    if (startSquare - sq < 0)
		return -1;
	    if (squares[startSquare - sq]->state == EMPTY)
		break;
	}

	// Can fit?
	if (sq == runLength)
	    return startSquare;

	// Can't fit? Keep going after next empty square
	startSquare -= sq + 1;
    }

    // Didn't find one?
    return -1;
}
