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

Solver::Solver(Grid *_grid)
{
    grid = _grid;

    // Allocate enough room to hold runs. Overestimate.
    numRows = grid->getCol(0)->length;
    numCols = grid->getRow(0)->length;
    int maxLength = (numRows > numCols ? numRows : numCols);

    runs = new Run [maxLength];

    // Allocate lines we can use for temporary work
    workRow = new Line();
    workCol = new Line();
    workRow->setLength(numCols);
    workCol->setLength(numRows);
    workSquares = new Square [maxLength];
    for (int col = 0; col < numCols; col++)
	workRow->squares[col] = &workSquares[col];
    for (int row = 0; row < numRows; row++)
	workCol->squares[row] = &workSquares[row];
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Solver::~Solver()
{
    delete [] runs;
    delete workRow;
    delete workCol;
    delete [] workSquares;
}

////////////////////////////////////////////////////////////////////////////
//
// Solves entire grid, if possible, changing it.
//
////////////////////////////////////////////////////////////////////////////

Solver::Result
Solver::solve()
{
    // Make a copy of the grid so we can detect all changes to it
    Grid *saveGrid = grid;
    grid = new Grid(Grid::SOLVE);
    grid->copy(saveGrid);

    // Solve the copy
    Result result = solveFull();

    // Determine what changes were made and copy them
    if (result == SOLVED) {
	saveGrid->beginChange();

	for (int row = 0; row < numRows; row++) {
	    const Line *rowLine = grid->getRow(row);
	    for (int col = 0; col < numCols; col++)
		saveGrid->change(row, col, rowLine->squares[col]->state);
	}

	saveGrid->endChange();
    }

    // Free up the copy
    delete grid;
    grid = saveGrid;

    return result;
}

////////////////////////////////////////////////////////////////////////////
//
// Solves one row or column of grid, if possible.
//
////////////////////////////////////////////////////////////////////////////

Solver::Result
Solver::solveOne()
{
    // Try rows first
    for (int row = 0; row < numRows; row++) {

	workRow->copy(grid->getRow(row));

	Result result = solveLine(workRow);

	// Stop on error
	if (result == ERROR)
	    return ERROR;

	// If anything changed, make a change to the grid and stop
	else if (result == SOLVED) {
	    grid->beginChange();

	    for (int col = 0; col < numCols; col++)
		grid->change(row, col, workRow->squares[col]->state);

	    grid->endChange();

	    return SOLVED;
	}
    }

    // Continue to columns if nothing was solved
    for (int col = 0; col < numCols; col++) {

	workCol->copy(grid->getCol(col));

	Result result = solveLine(workCol);

	// Stop on error
	if (result == ERROR)
	    return ERROR;

	// If anything changed, make a change to the grid and stop
	else if (result == SOLVED) {
	    grid->beginChange();

	    for (int row = 0; row < numRows; row++)
		grid->change(row, col, workCol->squares[row]->state);

	    grid->endChange();

	    return SOLVED;
	}
    }

    // If we get here, nothing changed
    return UNSOLVED;
}

////////////////////////////////////////////////////////////////////////////
//
// Solves full grid using whatever means necessary.
//
////////////////////////////////////////////////////////////////////////////

Solver::Result
Solver::solveFull()
{
    Result result = solveDeterministic();

    // If the grid wasn't totally solved, it's time to start guessing
    if (result == UNSOLVED)
	result = guess();

    return result;
}

////////////////////////////////////////////////////////////////////////////
//
// Solves full grid using only deterministic means.
//
////////////////////////////////////////////////////////////////////////////

Solver::Result
Solver::solveDeterministic()
{
    Result result;
    SbBool anyChange;

    while (TRUE) {

	anyChange = FALSE;

	// Solve rows
	for (int row = 0; row < numRows; row++) {
	    workRow->copy(grid->getRow(row));
	    result = solveLine(workRow);

	    // Stop on error
	    if (result == ERROR)
		break;

	    if (result == SOLVED) {
		anyChange = TRUE;
		for (int col = 0; col < numCols; col++)
		    grid->change(row, col, workRow->squares[col]->state);
	    }
	}

	// Stop if an error was found or if grid is solved
	if (result == ERROR)
	    break;
	if (grid->isSolved()) {
	    result = SOLVED;
	    break;
	}

	// Solve columns
	for (int col = 0; col < numCols; col++) {
	    workCol->copy(grid->getCol(col));
	    result = solveLine(workCol);

	    // Stop on error
	    if (result == ERROR)
		break;

	    if (result == SOLVED) {
		anyChange = TRUE;
		for (int row = 0; row < numRows; row++)
		    grid->change(row, col, workCol->squares[row]->state);
	    }
	}

	// Stop if an error was found, if grid is solved, or if no
	// change was made
	// Stop if an error was found or if grid is solved
	if (result == ERROR)
	    break;
	if (grid->isSolved()) {
	    result = SOLVED;
	    break;
	}
	if (! anyChange) {
	    result = UNSOLVED;
	    break;
	}
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////
//
// Solves a line. Returns SOLVED if any square changed, ERROR if an
// error occurred, and UNSOLVED otherwise.
//
////////////////////////////////////////////////////////////////////////////

Solver::Result
Solver::solveLine(Line *lineToSolve)
{
    line = lineToSolve;

    // Set up Run structures for runs in line
    if (! setUpRuns())
	return ERROR;

    // Repeat until nothing changes
    SbBool anySolved = FALSE;
    do {
	// Pretend nothing has changed so far
	squaresChanged = runsChanged = FALSE;

	// Try to solve line, checking for errors
	if (! checkExclusives() ||
	    ! checkEnds()	||
	    ! checkSequences()	||
	    ! updateRuns()	||
	    ! updateSquares())
	    return ERROR;

	if (squaresChanged)
	    anySolved = TRUE;

    } while (squaresChanged || runsChanged);

    return (anySolved ? SOLVED : UNSOLVED);
}

////////////////////////////////////////////////////////////////////////////
//
// Sets up runs for current line. Returns FALSE on error.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::setUpRuns()
{
    // For each run in line, set up corresponding Run
    numRuns = line->spec->num;

    // Initialize runs
    int i;
    for (i = 0; i < numRuns; i++) {
	Run *run = &runs[i];
	run->length = line->spec->runs[i];
	run->exclusive = FALSE;
	run->solved = FALSE;
    }

    // Starting from left, set up min starting point for runs
    int curSquare = 0; 
    for (i = 0; i < numRuns; i++) {
	Run *run = &runs[i];

	// Find first sequence of squares, starting at curSquare, in
	// which the run will fit
	run->start = line->findRoomForward(curSquare, run->length);
	if (run->start < 0)
	    return FALSE;

	// Prepare for next run
	curSquare = run->start + run->length + 1;
    }

    // Do the same thing, starting from the right
    curSquare = line->length - 1;
    for (i = numRuns - 1; i >= 0; --i) {
	Run *run = &runs[i];
	run->end = line->findRoomBackward(curSquare, run->length);
	if (run->end < 0)
	    return FALSE;
	curSquare = run->end - (run->length + 1);
    }

    // Verify runs
    for (i = 0; i < numRuns; i++)
	if (! verifyRun(&runs[i]))
	    return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Looks for runs that are exclusive over their ranges.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::checkExclusives()
{
    for (int i = 0; i < numRuns; i++) {
	Run *run = &runs[i];

	if (! run->solved && run->exclusive) {

	    // Find first filled square, if any
	    int firstFilled = line->findMatchForward(run->start, FILLED);

	    // The run must cover that square if it's within range
	    if (firstFilled >= 0 && firstFilled <= run->end) {

		// Find the end of the run that starts at firstFilled
		int lastFilled = line->findRunForward(firstFilled);

		if (! adjustRun(run, firstFilled, lastFilled))
		    return FALSE;
	    }

	    // Do same, but go backwards
	    int lastFilled = line->findMatchBackward(run->end, FILLED);
	    if (lastFilled >= 0 && lastFilled >= run->start) {
		int firstFilled = line->findRunBackward(lastFilled);
		if (! adjustRun(run, firstFilled, lastFilled))
		    return FALSE;
	    }
	}
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Sees if position of first run (from either end) is forced by filled
// squares.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::checkEnds()
{
    Run *run;

    // Find first unsolved run
    int i;
    for (i = 0; i < numRuns; i++) {
	run = &runs[i];
	if (! run->solved)
	    break;
    }

    if (i < numRuns) {
	// Find first filled square, if any
	int firstFilled = line->findMatchForward(run->start, FILLED);
	if (firstFilled >= 0) {

	    // Find the end of the run that starts at firstFilled
	    int lastFilled = line->findRunForward(firstFilled);

	    // If first filled square is within first (length+1)
	    // squares, the first run must cover that square
	    if (run->start + run->length >= firstFilled) {
		if (! adjustRun(run, firstFilled, lastFilled))
		    return FALSE;
	    }

	    // Otherwise, if filled run is longer than the first run,
	    // it must end before the first filled square
	    else if (lastFilled - firstFilled + 1 > run->length) {
		if (run->end > firstFilled - 2) {
		    run->end = firstFilled - 2;
		    if (! markRunChanged(run))
			return FALSE;
		}
	    }

	    // Make sure that run doesn't go past the first empty
	    // square after the first filled one
	    int firstEmpty = line->findMatchForward(firstFilled + 1, EMPTY);
	    if (firstEmpty >= 0 && firstEmpty <= run->end) {
		run->end = firstEmpty - 1;
		if (! markRunChanged(run))
		    return FALSE;
	    }
	}
    }

    // Do same for last run
    for (i = numRuns - 1; i >= 0; i--) {
	run = &runs[i];
	if (! run->solved)
	    break;
    }
    if (i >= 0) {
	int lastFilled = line->findMatchBackward(run->end, FILLED);
	if (lastFilled >= 0) {
	    int firstFilled = line->findRunBackward(lastFilled);

	    if (run->end - run->length <= lastFilled) {
		if (! adjustRun(run, firstFilled, lastFilled))
		    return FALSE;
	    }
	    else if (lastFilled - firstFilled + 1 > run->length) {
		if (run->start < firstFilled + 2) {
		    run->start = firstFilled + 2;
		    if (! markRunChanged(run))
			return FALSE;
		}
	    }

	    int lastEmpty = line->findMatchBackward(lastFilled - 1, EMPTY);
	    if (lastEmpty >= 0 && lastEmpty >= run->start) {
		run->start = lastEmpty + 1;
		if (! markRunChanged(run))
		    return FALSE;
	    }
	}
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Checks if any sequences of squares match only one run.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::checkSequences()
{
    int startFilled, endFilled = -1;

    while (TRUE) {

	startFilled = line->findMatchForward(endFilled + 1, FILLED);

	if (startFilled < 0)
	    break;

	// Find end of filled run
	endFilled = line->findRunForward(startFilled);

	int length = endFilled - startFilled + 1;

	// See if there are any filled sequences bounded by empty
	// squares or borders on both sides. If so, see if just one
	// run matches it.
	if ((startFilled == 0 ||
	     line->squares[startFilled - 1]->state == EMPTY) &&
	    (endFilled == line->length - 1 ||
	     line->squares[endFilled + 1]->state == EMPTY)) {

	    // Find only 1 run that contains this and matches the length
	    int numCandidates = 0;
	    Run *run, *matchRun;
	    int i;
	    for (i = 0; i < numRuns; i++) {
		run = &runs[i];
		if (run->length == length &&
		    run->start <= startFilled && run->end >= endFilled) {
		    numCandidates++;
		    matchRun = run;
		}
	    }

	    // If only 1, assign it
	    if (numCandidates == 1 &&
		(matchRun->start != startFilled ||
		 matchRun->end != endFilled)) {
		matchRun->start = startFilled;
		matchRun->end = endFilled;
		if (! markRunChanged(matchRun))
		    return FALSE;
	    }
	}

	// See if there are any filled sequences that can be contained
	// by only one run OR if all runs that could contain it have
	// the same length.
	else {
	    // Find only 1 run that can contain this sequence
	    int numCandidates = 0, matchLength;
	    Run *run, *matchRun;
	    for (int i = 0; i < numRuns; i++) {
		run = &runs[i];
		if (run->length >= length &&
		    run->start <= startFilled && run->end >= endFilled) {
		    numCandidates++;
		    matchRun = run;
		    if (numCandidates == 1)
			matchLength = run->length;
		    else if (matchLength != run->length)
			matchLength = -1;
		}
	    }

	    // If there are no runs that can contain it, there's some error
	    if (numCandidates == 0)
		return FALSE;

	    // If only 1, assign it
	    if (numCandidates == 1) {
		if (! adjustRun(matchRun, startFilled, endFilled))
		    return FALSE;
	    }

	    // If more than 1, but they are all the same length, see
	    // if we can mark any squares
	    else if (matchLength > 0) {

		if (startFilled == 0 ||
		    line->squares[startFilled - 1]->state == EMPTY) {

		    for (int i = 0; i < matchLength; i++)
			setSquare(startFilled + i, FILLED);
		    setSquare(startFilled + matchLength, EMPTY);
		}

		if (endFilled == line->length - 1 ||
		    line->squares[endFilled + 1]->state == EMPTY) {

		    for (int i = 0; i < matchLength; i++)
			setSquare(endFilled - i, FILLED);
		    setSquare(endFilled - matchLength, EMPTY);
		}
	    }
	}

	if (endFilled == line->length - 1)
	    break;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Updates all runs based on current grid.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::updateRuns()
{
    // Make sure no run starts before prev run ends 
    Run *prevRun = NULL;
    int i;
    for (i = 0; i < numRuns; i++) {
	Run *run = &runs[i];

	if (prevRun != NULL) {
	    int minStart;
	    if (prevRun->solved)
		minStart = prevRun->end + 1;
	    else
		minStart = prevRun->start + prevRun->length + 1;
	    if (run->start < minStart) {
		run->start = minStart;
		if (! markRunChanged(run))
		    return FALSE;
	    }
	}
	prevRun = run;
    }

    // Or ends after next run starts
    prevRun = NULL;
    for (i = numRuns - 1; i >= 0; i--) {
	Run *run = &runs[i];

	if (prevRun != NULL) {
	    int maxEnd;
	    if (prevRun->solved)
		maxEnd = prevRun->start - 1;
	    else
		maxEnd = prevRun->end - (prevRun->length + 1);
	    if (run->end > maxEnd) {
		run->end = maxEnd;
		if (! markRunChanged(run))
		    return FALSE;
	    }
	}
	prevRun = run;
    }

    // See if any runs can be narrowed
    for (i = 0; i < numRuns; i++) {
	Run *run = &runs[i];
	if (! run->solved) {
	    while (line->squares[run->start]->state == EMPTY)
		run->start++;
	    while (line->squares[run->end]->state == EMPTY)
		run->end--;
	}
    }

    // See if any runs are now exclusive
    for (i = 0; i < numRuns; i++) {
	Run *run = &runs[i];
	if (! run->exclusive) {

	    // If it's the only run
	    if (i == 0 && i == numRuns - 1) {
		run->exclusive = TRUE;
		if (! markRunChanged(run))
		    return FALSE;
	    }

	    // Or it doesn't overlap neighbors at all
	    else {
		if ((i == 0 || runs[i - 1].end < run->start) &&
		    (i == numRuns - 1 || runs[i + 1].start > run->end)) {
		    run->exclusive = TRUE;
		    if (! markRunChanged(run))
			return FALSE;
		}
	    }
	}
    }

    // See if any runs are now solved
    for (i = 0; i < numRuns; i++) {
	Run *run = &runs[i];
	if (! run->solved && (run->start + run->length - 1 == run->end)) {
	    run->solved = TRUE;
	    if (! markRunChanged(run))
		return FALSE;
	}
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Updates squares based on current set of runs.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::updateSquares()
{
    //
    // Check for new empty squares between runs
    //

    int prevEnd = 0;
    int i;
    for (i = 0; i < numRuns; i++) {
	Run *run = &runs[i];

	// Turn off all squares between previous end and this start
	while (prevEnd < run->start)
	    setSquare(prevEnd++, EMPTY);

	prevEnd = run->end + 1;
    }

    // Do same for stuff after last run
    while (prevEnd < line->length)
	setSquare(prevEnd++, EMPTY);

    //
    // Check for new filled squares within runs
    //

    // See if length of run overlaps in given space
    for (i = 0; i < numRuns; i++) {
	Run *run = &runs[i];

	int lastForward   = run->start + run->length - 1;
	int firstBackward = run->end   - run->length + 1;

	if (lastForward < 0 || firstBackward < 0)
	    return FALSE;

	while (firstBackward <= lastForward)
	    setSquare(firstBackward++, FILLED);
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Adjusts run to contained the given filled squares.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::adjustRun(Run *run, int firstFilled, int lastFilled)
{
    int minStart = lastFilled  - run->length + 1;
    int maxEnd   = firstFilled + run->length - 1;

    if (minStart > 0 && line->squares[minStart - 1]->state == FILLED)
	minStart++;
    if (maxEnd < line->length-1 && line->squares[maxEnd + 1]->state == FILLED)
	maxEnd--;

    if (run->start < minStart) {
	run->start = minStart;
	if (! markRunChanged(run))
	    return FALSE;
    }

    if (run->end > maxEnd) {
	run->end = maxEnd;
	if (! markRunChanged(run))
	    return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Changes given square of current line to new state
//
////////////////////////////////////////////////////////////////////////////

void
Solver::setSquare(int index, SqState newState)
{
    if (line->squares[index]->state != newState) {
	line->squares[index]->state = newState;
	squaresChanged = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Marks run as having changed. Checks for internal errors.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::markRunChanged(Run *run)
{
    runsChanged = TRUE;

    if (! verifyRun(run))
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//
// Checks run for internal errors.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::verifyRun(Run *run)
{
    SbBool ok = TRUE;

    if (run->start < 0 ||
	run->end < 0 ||
	run->start > run->end ||
	run->start + run->length - 1 > run->end ||
	(run->solved && run->start + run->length - 1 != run->end))
	ok = FALSE;

    return ok;
}

////////////////////////////////////////////////////////////////////////////
//
// Guesses values for unknown squares until a result is found.
//
////////////////////////////////////////////////////////////////////////////

Solver::Result
Solver::guess()
{
    Result result;

    int guessRow = 0, guessCol = 0;

    // Repeat this until we run out of squares or the grid is solved
    do {
	// Find the first unknown square
	int row, col;
	if (! getFirstUnknown(guessRow, guessCol, row, col)) {
	    result = UNSOLVED;
	    break;
	}

	result = guessOne(row, col);

	// Skip to next square
	guessRow = row;
	guessCol = col + 1;
	if (guessCol == numCols) {
	    guessRow++;
	    guessCol = 0;
	}

    } while (result == UNSOLVED);

    return result;
}

////////////////////////////////////////////////////////////////////////////
//
// Makes a guess for the given square. Returns a Result code.
//
////////////////////////////////////////////////////////////////////////////

Solver::Result
Solver::guessOne(int row, int col)
{
    Result result;

    // Guess both FILLED and EMPTY for the square
    Result filledResult = testOne(row, col, FILLED);
    Result emptyResult  = testOne(row, col, EMPTY);

    // If one is in error and the other is solved, set that square and
    // solve the grid
    if (filledResult == SOLVED && emptyResult == ERROR) {
	grid->change(row, col, FILLED);
	solveDeterministic();
	result = SOLVED;
    }
    else if (filledResult == ERROR && emptyResult == SOLVED) {
	grid->change(row, col, EMPTY);
	solveDeterministic();
	result = SOLVED;
    }

    // If both are solved, there are multiple solutions
    else if (filledResult == SOLVED && emptyResult == SOLVED)
	result = MULTISOLVED;

    // If both are error, something is wrong
    else if (filledResult == ERROR && emptyResult == ERROR)
	result = ERROR;

    // Otherwise, try another square
    else
	result = UNSOLVED;

    return result;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets the given square to the given value and sees what happens.
// First makes a copy of the grid so it is not changed. Returns a
// Result code.
//
////////////////////////////////////////////////////////////////////////////

Solver::Result
Solver::testOne(int row, int col, SqState state)
{
    // Make a copy of the grid so we don't screw it up
    Grid *saveGrid = grid;
    grid = new Grid(Grid::SOLVE);
    grid->copy(saveGrid);

    // Change the square and solve the grid
    grid->change(row, col, state);
    Result result = solveDeterministic();

    // Free up the copy
    delete grid;
    grid = saveGrid;

    return result;
}

////////////////////////////////////////////////////////////////////////////
//
// Returns first unknown square of grid starting at given square.
// Returns FALSE if none left.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Solver::getFirstUnknown(int startRow, int startCol, int &row, int &col)
{
    for (row = startRow; row < numRows; row++) {
	const Line *l = grid->getRow(row);
	for (col = (row == startRow ? startCol : 0); col < numCols; col++)
	    if (l->squares[col]->state == UNKNOWN)
		return TRUE;
    }

    return FALSE;
}
