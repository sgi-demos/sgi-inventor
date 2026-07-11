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
#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>
#ifndef SDL_PORT
#include <Inventor/Xt/SoXtRenderArea.h>
#endif
#include "pbn.h"

// Size in pixels of a grid square
#define SQUARE_SIZE	20

// Width of gap between squares. The second gap is for every 5th square
#define SQUARE_GAP1	0.05
#define SQUARE_GAP2	0.1

// Character that starts a comment in input file
#define COMMENT_CHAR	'#'

////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
////////////////////////////////////////////////////////////////////////////

Grid::Grid(Mode _mode)
{
    mode = _mode;

    numRows = 0;
    numCols = 0;

    isBuilt = FALSE;

    // Add a dummy change to make list management easier
    changes = curChange = new Change;
    openChange = NULL;
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Grid::~Grid()
{
    delete [] rows;
    delete [] cols;
    delete [] squares;

    delete [] squaresChanged;
    delete [] changedSquares;

    if (isBuilt)
	delete renderArea;

    // Free up the storage used to save changes to the grid
    freeChanges(changes);
}

////////////////////////////////////////////////////////////////////////////
//
// Sets number of rows and columns.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::setSize(int _numRows, int _numCols)
{
    int row, col;

    numRows = _numRows;
    numCols = _numCols;

    // Create and initialize grid squares
    numSquares = numRows * numCols;
    squares    = new Square [numSquares];
    Square *sq = squares;
    for (row = 0; row < numRows; row++) {
	for (col = 0; col < numCols; col++) {
	    sq->row   = row;
	    sq->col   = col;
	    sq->state = (mode == SOLVE ? UNKNOWN : EMPTY);
	    sq->next  = NULL;
	    sq++;
	}
    }

    // Create and initialize row and column lines
    rows = new Line [numRows];
    cols = new Line [numCols];
    Line *l = rows;
    for (row = 0; row < numRows; row++) {
	l->setLength(numCols);
	for (col = 0; col < numCols; col++)
	    l->squares[col] = &squares[squareIndex(row, col)];
	l++;
    }
    l = cols;
    for (col = 0; col < numCols; col++) {
	l->setLength(numRows);
	for (row = 0; row < numRows; row++)
	    l->squares[row] = &squares[squareIndex(row, col)];
	l++;
    }

    // Create storage to save changes to squares
    squaresChanged = new SbBool [numSquares];
    changedSquares = new int    [numSquares];
}

////////////////////////////////////////////////////////////////////////////
//
// Reads grid specification from the given file. Returns FALSE on error.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Grid::read(FILE *fp)
{
    // Read numRows and numCols
    int numRows, numCols;
    if (! readNum(fp, numRows) || ! readNum(fp, numCols) ||
	numRows <= 0 || numCols <= 0)
	return FALSE;

    setSize(numRows, numCols);

    // Read row specifications
    for (int row = 0; row < numRows; row++) {
	Spec *spec = readSpec(fp);
	if (spec == NULL)
	    return FALSE;
	rows[row].spec = spec;
    }

    // Read column specifications
    for (int col = 0; col < numCols; col++) {
	Spec *spec = readSpec(fp);
	if (spec == NULL)
	    return FALSE;
	cols[col].spec = spec;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Reads in a Spec from the given file pointer, setting up and
// returning a Spec instance. Returns NULL on error.
//
////////////////////////////////////////////////////////////////////////////

Spec *
Grid::readSpec(FILE *fp)
{
    // Read number of runs
    int	numRuns;
    if (! readNum(fp, numRuns))
	return NULL;

    Spec *spec = new Spec(numRuns);

    // Read specs for the row
    for (int i = 0; i < numRuns; i++) {
	int len;
	if (! readNum(fp, len))
	    return NULL;
	spec->runs[i] = len;
    }

    return spec;
}

////////////////////////////////////////////////////////////////////////////
//
// Reads in an integer from the given file pointer, storing it in
// "num". Returns FALSE on error.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Grid::readNum(FILE *fp, int &num)
{
    int c;

    // Keep going until we find a non-space, non-comment character
    while (TRUE) {

	// Find first non-space character
	while ((c = getc(fp)) != EOF && isspace(c))
	    ;

	if (c == EOF)
	    return FALSE;

	if (c == COMMENT_CHAR)
	    while ((c = getc(fp)) != EOF && c != '\n')
		;

	else {
	    ungetc(c, fp);
	    break;
	}
    }

    return (fscanf(fp, "%d", &num) == 1);
}

////////////////////////////////////////////////////////////////////////////
//
// Writes grid specification to the given file.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::write(FILE *fp)
{
    // Write cheesy header
    SbTime now;
    now.setToTimeOfDay();
    fprintf(fp, "%c Created by %s on %s\n\n",
	    COMMENT_CHAR, 
#ifdef SDL_PORT
	    (getenv("USER") ? getenv("USER") : "player")
#else
	    cuserid(NULL)
#endif
	    , now.formatDate().getString());

    // Write numRows and numCols
    fprintf(fp, "%d %d\n\n", numRows, numCols);

    // Write row and column specifications. Note that the
    // specifications are computed here, not assumed to be correct in
    // the spec instances.
    fprintf(fp, "\n%c Rows:\n", COMMENT_CHAR);
    for (int row = 0; row < numRows; row++)
	writeSpec(fp, &rows[row]);
    fprintf(fp, "\n%c Cols:\n", COMMENT_CHAR);
    for (int col = 0; col < numCols; col++)
	writeSpec(fp, &cols[col]);
}

////////////////////////////////////////////////////////////////////////////
//
// Writes specification for the given row or column line.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::writeSpec(FILE *fp, Line *line)
{
    // First, count runs in line

    int numRuns = 0, curSquare = 0;

    // Find next run
    while (curSquare < line->length) {

	curSquare = line->findMatchForward(curSquare, FILLED);

	// No more runs
	if (curSquare < 0)
	    break;

	numRuns++;

	// Go to end of run
	curSquare = line->findMatchForward(curSquare, EMPTY);

	if (curSquare < 0)
	    break;
    }

    fprintf(fp, "%d\t", numRuns);

    curSquare = 0;
    for (int run = 0; run < numRuns; run++) {
	int runStart = line->findMatchForward(curSquare, FILLED), runLength;

	// Go to end of run
	curSquare = line->findMatchForward(runStart + 1, EMPTY);

	if (curSquare < 0)
	    runLength = line->length - runStart;
	else
	    runLength = curSquare - runStart;

	fprintf(fp, "%d ", runLength);
    }

    fprintf(fp, "\n");
}

////////////////////////////////////////////////////////////////////////////
//
// Copies from another grid.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::copy(const Grid *copyFrom)
{
    setSize(copyFrom->numRows, copyFrom->numCols);

    // Copy rows and columns
    for (int row = 0; row < numRows; row++)
	rows[row].copy(&copyFrom->rows[row]);
    for (int col = 0; col < numCols; col++)
	cols[col].copy(&copyFrom->cols[col]);

    // If copying from a PAINT grid to a SOLVE grid, we need to set up
    // row and column specs
    if (mode == SOLVE && copyFrom->mode == PAINT) {
	for (int row = 0; row < numRows; row++)
	    computeSpec(&rows[row]);
	for (int col = 0; col < numCols; col++)
	    computeSpec(&cols[col]);
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Computes specification for the given row or column line, storing it
// in the line.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::computeSpec(Line *line)
{
    // First, count runs in line
    int numRuns = 0, curSquare = 0;

    // Find next run
    while (curSquare < line->length) {

	curSquare = line->findMatchForward(curSquare, FILLED);

	// No more runs
	if (curSquare < 0)
	    break;

	numRuns++;

	// Go to end of run
	curSquare = line->findMatchForward(curSquare, EMPTY);

	if (curSquare < 0)
	    break;
    }

    Spec *spec = new Spec(numRuns);

    curSquare = 0;
    for (int run = 0; run < numRuns; run++) {
	int runStart = line->findMatchForward(curSquare, FILLED), runLength;

	// Go to end of run
	curSquare = line->findMatchForward(runStart + 1, EMPTY);

	if (curSquare < 0)
	    runLength = line->length - runStart;
	else
	    runLength = curSquare - runStart;

	spec->runs[run] = runLength;
    }

    line->spec = spec;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds and returns widget for grid.
//
////////////////////////////////////////////////////////////////////////////

Widget
Grid::buildWidget(Widget parent)
{
    if (! isBuilt) {
#ifdef SDL_PORT
	renderArea = new SoSDLRenderArea(
	    mode == SOLVE ? "Paint By Numbers" : "PBN Create", 400, 400);
	(void) parent;
#else
	renderArea = new SoXtRenderArea(parent);
#endif

	// Figure out window size, based on whether specs appear
	setSizes();

	// Set square colors
	colors[UNKNOWN].setValue(.5, .5, .5);
	colors[FILLED].setValue(.9, .9, .2);
	colors[EMPTY].setValue(.1, .1, .4);

	// Build and install the scene graph
	buildGraph();

	// Display the widget
#ifndef SDL_PORT
	renderArea->show();
#endif

	isBuilt = TRUE;
    }

#ifdef SDL_PORT
    return NULL;
#else
    return renderArea->getWidget();
#endif
}

////////////////////////////////////////////////////////////////////////////
//
// Sets grid and window size.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::setSizes()
{
    int row, col;

    // Determine the largest spec size, if any
    numRowSpecSpaces = numColSpecSpaces = 0;
    if (mode == SOLVE) {
	for (row = 0; row < numRows; row++)
	    if (rows[row].spec->num > numRowSpecSpaces)
		numRowSpecSpaces = rows[row].spec->num;
	for (col = 0; col < numCols; col++)
	    if (cols[col].spec->num > numColSpecSpaces)
		numColSpecSpaces = cols[col].spec->num;
    }

    // Compute size
    SbVec2s size(SQUARE_SIZE * (numCols + numRowSpecSpaces),
		 SQUARE_SIZE * (numRows + numColSpecSpaces));

    // Put some room around specs, if any
    if (numRowSpecSpaces > 0)
	size[0] += 10;
    if (numColSpecSpaces > 0)
	size[1] += 10;

    // Set window size
    renderArea->setSize(size);
}

////////////////////////////////////////////////////////////////////////////
//
// Builds and installs main scene graph.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::buildGraph()
{
    //
    // Set up root, event callback, camera, and light model
    //

    SoSeparator	*root = new SoSeparator;
    root->ref();

    ecb = new SoEventCallback;
    root->addChild(ecb);
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
			  &Grid::processButton, this);

    SoOrthographicCamera *cam = new SoOrthographicCamera;
    root->addChild(cam);
    SbBox2f gridBox(-numRowSpecSpaces, -numRows, numCols, numColSpecSpaces);
    SbVec2f center = gridBox.getCenter();
    cam->position.setValue(center[0], center[1], 3.0);
    cam->height = gridBox.getMax()[1] - gridBox.getMin()[1];
    cam->aspectRatio = ((gridBox.getMax()[0] - gridBox.getMin()[0]) /
			(gridBox.getMax()[1] - gridBox.getMin()[1]));
    cam->viewportMapping = SoCamera::LEAVE_ALONE;
    cam->nearDistance = 1.0;
    cam->farDistance  = 5.0;

    SoLightModel *lm = new SoLightModel;
    root->addChild(lm);
    lm->model = SoLightModel::BASE_COLOR;

    //
    // Set up subgraph containing the grid squares and the spec text
    // (if any). None of this stuff is pickable.
    //

    SoSeparator *sep = new SoSeparator;
    root->addChild(sep);

    SoPickStyle *ps = new SoPickStyle;
    sep->addChild(ps);
    ps->style = SoPickStyle::UNPICKABLE;

    sep->addChild(buildGridGraph());

    if (mode == SOLVE)
	sep->addChild(buildSpecGraph());

    //
    // Create an invisible, pickable rectangular face to do all the
    // picking with.
    //

    sep = new SoSeparator;
    root->addChild(sep);

    SoDrawStyle *ds = new SoDrawStyle;
    sep->addChild(ds);
    ds->style = SoDrawStyle::INVISIBLE;

    SoCoordinate3 *coord = new SoCoordinate3;
    sep->addChild(coord);
    coord->point.set1Value(0, -numRowSpecSpaces,  numColSpecSpaces, -1.0);
    coord->point.set1Value(1, -numRowSpecSpaces, -numRows,          -1.0);
    coord->point.set1Value(2,  numCols,          -numRows,          -1.0);
    coord->point.set1Value(3,  numCols,           numColSpecSpaces, -1.0);

    sep->addChild(new SoFaceSet);

    //
    // Add the graph for interaction feedback.
    //

    root->addChild(buildFeedbackGraph());

    renderArea->setSceneGraph(root);
    root->unref();

    gridCamera = cam;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds subgraph containing grid squares.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Grid::buildGridGraph()
{
    SoSeparator *sep = new SoSeparator;
    sep->ref();

    // Material binding
    SoMaterialBinding *mb = new SoMaterialBinding;
    sep->addChild(mb);
    mb->value = SoMaterialBinding::PER_FACE;

    // Color node to index into for grid squares
    SoBaseColor	*color = new SoBaseColor;
    sep->addChild(color);

    // Coordinates of square corners, 4 per square
    SoCoordinate3 *coord = new SoCoordinate3;
    sep->addChild(coord);

    // Faces forming squares
    SoFaceSet *fset = new SoFaceSet;
    sep->addChild(fset);

    // Make room in various fields
    color->rgb.insertSpace(0, numSquares - 1);
    coord->point.insertSpace(0, 4 * numSquares - 1);
    fset->numVertices.insertSpace(0, numSquares - 1);

    // For each square on grid, set up coordinates and such. The
    // origin is at the upper left corner of the squares. Each square
    // is 1 unit in each dimension. 
    int curSquare = 0;
    for (int row = 0; row < numRows; row++) {
	for (int col = 0; col < numCols; col++) {

	    // Set up square color
	    color->rgb.set1Value(curSquare,
				 colors[mode == SOLVE ? UNKNOWN : EMPTY]);

	    // Set up 4 coordinates of the square. Move the right and
	    // bottom in a little to leave spaces between the squares;
	    // every 5'th space is a little larger.
	    float xl =  col, xr =   col + 1.0;
	    float yt = -row, yb = -(row + 1.0);

	    if (col == 0)
		xl += SQUARE_GAP1;
	    if (row == 0)
		yt -= SQUARE_GAP1;

	    if (col % 5 == 4)
		xr -= SQUARE_GAP2;
	    else
		xr -= SQUARE_GAP1;
	    if (row % 5 == 4)
		yb += SQUARE_GAP2;
	    else
		yb += SQUARE_GAP1;

	    coord->point.set1Value(curSquare * 4 + 0, xl, yt, 0.0);
	    coord->point.set1Value(curSquare * 4 + 1, xl, yb, 0.0);
	    coord->point.set1Value(curSquare * 4 + 2, xr, yb, 0.0);
	    coord->point.set1Value(curSquare * 4 + 3, xr, yt, 0.0);

	    // Set up face set
	    fset->numVertices.set1Value(curSquare, 4);

	    curSquare++;
	}
    }

    // Save grid colors so we can change them later
    gridColor = color;

    // Save coordinate node so feedback graph can use it
    gridCoord = coord;

    sep->unrefNoDelete();
    return sep;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds subgraph containing spec text.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Grid::buildSpecGraph()
{
    // Build stuff common to all specs
    SoSeparator *sep = new SoSeparator;
    sep->ref();
    sep->renderCaching = SoSeparator::ON;

    SoBaseColor *color = new SoBaseColor;
    sep->addChild(color);
    color->rgb.setValue(.6, .6, .4);

    SoFont *font = new SoFont;
    sep->addChild(font);
    font->name = "Helvetica-Bold";
    font->size = 15;

    //////////////////////////////////////////////////////
    //
    // Build the column specs
    //

    SoSeparator *colSep = new SoSeparator;
    sep->addChild(colSep);

    for (int col = 0; col < numCols; col++) {
	Spec *spec = cols[col].spec;
	for (int sp = 0; sp < spec->num; sp++)
	    colSep->addChild(build1SpecGraph(col + .5,
					     spec->num - sp - 1 + .3,
					     spec->runs[sp]));
    }

    //////////////////////////////////////////////////////
    //
    // Build the row specs
    //

    SoSeparator *rowSep = new SoSeparator;
    sep->addChild(rowSep);

    for (int row = 0; row < numRows; row++) {
	Spec *spec = rows[row].spec;
	for (int sp = 0; sp < spec->num; sp++)
	    rowSep->addChild(build1SpecGraph(-(spec->num - sp - 1) - .5,
					     -(row + 1) + .3,
					     spec->runs[sp]));
    }

    ////////////////////////////////////////////////////////

    sep->unrefNoDelete();
    return sep;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds and returns a subgraph for one row or column spec, given the
// run-length and the x and y positions.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Grid::build1SpecGraph(float x, float y, int runLength)
{
    SoSeparator		*sep = new SoSeparator;
    SoTranslation	*xl  = new SoTranslation;
    SoText2		*tx  = new SoText2;

    sep->ref();
    sep->addChild(xl);
    sep->addChild(tx);

    xl->translation.setValue(x, y, 0.5);
    tx->justification = SoText2::CENTER;
    tx->string = SbString(runLength);

    sep->unrefNoDelete();
    return sep;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds and returns feedback scene graph.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Grid::buildFeedbackGraph()
{
    SoSwitch *sw = new SoSwitch;
    sw->ref();
    sw->whichChild = SO_SWITCH_NONE;

    SoSeparator *ann = new SoSeparator;
    ann->renderCaching = SoSeparator::OFF;
    sw->addChild(ann);

    // Set up color node for the graph
    SoBaseColor *color = new SoBaseColor;
    ann->addChild(color);

    // Use same coordinates used for grid
    ann->addChild(gridCoord);

    SoIndexedFaceSet *ifs = new SoIndexedFaceSet;
    ann->addChild(ifs);
    ifs->coordIndex.deleteValues(0);	// Totally empty

    // Save nodes for later changes
    gridFBSwitch = sw;
    gridFBIFS    = ifs;
    gridFBColor  = color;

    sw->unrefNoDelete();
    return sw;
}

////////////////////////////////////////////////////////////////////////////
//
// Clears all grid squares to initial color.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::clear()
{
    SqState newState = (mode == PAINT ? EMPTY : UNKNOWN);

    beginChange();

    for (int i = 0; i < numSquares; i++) {
	Square *sq = &squares[i];
	change(sq->row, sq->col, newState);
    }

    endChange();
}

////////////////////////////////////////////////////////////////////////////
//
// These are used to make changes to squares from outside.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::beginChange()
{
    // If we had undone stuff, clear out the list after the current
    // change
    if (curChange != NULL)
	freeChanges(curChange->next);

    // Create a new change and add it to the end of the list
    Change *newChange = new Change;
    curChange->next = newChange;

    newChange->prev = curChange;
    newChange->next = NULL;

    curChange = openChange = newChange;

    if (isBuilt)
	changingColors = gridColor->rgb.startEditing();
}

void
Grid::change(int row, int col, SqState newState)
{
    int sq = squareIndex(row, col);

    if (squares[sq].state != newState) {

	if (openChange != NULL) {
	    openChange->add(row, col, squares[sq].state, newState);
	    if (isBuilt)
		changingColors[sq] = colors[newState];
	}
	else {
	    if (isBuilt)
		gridColor->rgb.set1Value(sq, colors[newState]);
	}

	squares[sq].state = newState;
    }
}

void
Grid::endChange()
{
    openChange = NULL;

    if (isBuilt)
	gridColor->rgb.finishEditing();
}

////////////////////////////////////////////////////////////////////////////
//
// Undo the last change.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::undo()
{
    if (! canUndo())
	return;

    // Restore the squares to their previous states
    if (isBuilt)
	changingColors = gridColor->rgb.startEditing();
    for (Square *sq = curChange->changes; sq != NULL; sq = sq->next) {
	int i = squareIndex(sq->row, sq->col);
	// Change square in grid
	if (isBuilt)
	    changingColors[i] = colors[sq->oldState];
	// Change color in scene graph
	squares[i].state = sq->oldState;
    }
    if (isBuilt)
	gridColor->rgb.finishEditing();

    // Move the change pointer back one
    curChange = curChange->prev;
}

////////////////////////////////////////////////////////////////////////////
//
// Redo the last undone change.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::redo()
{
    if (! canRedo())
	return;

    // Move the change pointer forward one
    curChange = curChange->next;

    // Restore the squares to their previous states
    if (isBuilt)
	changingColors = gridColor->rgb.startEditing();
    for (Square *sq = curChange->changes; sq != NULL; sq = sq->next) {
	int i = squareIndex(sq->row, sq->col);
	// Change square in grid
	if (isBuilt)
	    changingColors[i] = colors[sq->state];
	// Change color in scene graph
	squares[i].state = sq->state;
    }
    if (isBuilt)
	gridColor->rgb.finishEditing();
}

////////////////////////////////////////////////////////////////////////////
//
// These return TRUE if it is possible to undo or redo
//
////////////////////////////////////////////////////////////////////////////

SbBool
Grid::canUndo()
{
    // Can't undo if we're at the beginning (that is, the current
    // change is the dummy one we inserted at the beginning)
    return (curChange != changes);
}

SbBool
Grid::canRedo()
{
    // Can't redo if we're at the end
    return (curChange->next != NULL);
}

////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if entire grid is solved.
//
////////////////////////////////////////////////////////////////////////////

SbBool
Grid::isSolved() const
{
    // Look for any unknown square
    for (int i = 0; i < numSquares; i++)
	if (squares[i].state == UNKNOWN)
	    return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Processes a mouse button or mouse motion event.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::processButton(void *userData, SoEventCallback *ecb)
{
    Grid *grid = (Grid *) userData;

    if (SO_MOUSE_PRESS_EVENT(ecb->getEvent(), ANY)) {
	int square = grid->pickSquare(ecb->getPickedPoint());

	if (square >= 0) {

	    // See which state we're changing to
	    SqState state;
	    if (SO_MOUSE_PRESS_EVENT(ecb->getEvent(), BUTTON1))
		state = FILLED;
	    else if (SO_MOUSE_PRESS_EVENT(ecb->getEvent(), BUTTON2))
		state = EMPTY;
	    else
		state = (grid->mode == SOLVE ? UNKNOWN : EMPTY);

	    grid->beginInteraction(square, state);

	    ecb->grabEvents();
	    ecb->addEventCallback(SoLocation2Event::getClassTypeId(),
				  &Grid::processMotion, grid);
	}
    }

    else {
	grid->endInteraction();
	ecb->releaseEvents();
	ecb->removeEventCallback(SoLocation2Event::getClassTypeId(),
				 &Grid::processMotion, grid);
    }

    ecb->setHandled();
}

void
Grid::processMotion(void *userData, SoEventCallback *ecb)
{
    Grid *grid = (Grid *) userData;

    grid->continueInteraction(grid->pickSquare(ecb->getPickedPoint()));

    ecb->setHandled();
}

////////////////////////////////////////////////////////////////////////////
//
// Begins, continues, or ends a change to the grid resulting from
// interaction.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::beginInteraction(int square, SqState state)
{
    // Save stuff for later motion
    newState       = state;
    startSquare    = square;
    lastSquare     = square;

    // Set the feedback graph up for motion
    initFeedback();

    // Make sure the starting square is changed
    changeFeedback();
}

void
Grid::continueInteraction(int square)
{
    // If motion went out of grid area, or is over same square as last
    // time, don't do anything
    if (square < 0 || square == lastSquare)
	return;

    lastSquare = square;

    // Change the feedback graph to include the new square
    changeFeedback();
}

void
Grid::endInteraction()
{
    // Finish change to feedback
    finishFeedback();
}

////////////////////////////////////////////////////////////////////////////
//
// Initializes feedback graph for interaction feedback, changes it
// based on current mouse position, or finishes processing.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::initFeedback()
{
    // Turn on feedback switch
    gridFBSwitch->whichChild = SO_SWITCH_ALL;

    // Set up feedback color
    gridFBColor->rgb = colors[newState];

    // Nothing has changed yet
    if (mode == PAINT) {
	for (int i = 0; i < numSquares; i++)
	    squaresChanged[i] = FALSE;
	numChangedSquares = 0;
    }
}

void
Grid::changeFeedback()
{
    // In paint mode, just add the current square to the list of
    // changed squares
    if (mode == PAINT) {

	// Add current square if not already there
	if (! squaresChanged[lastSquare]) {
	    squaresChanged[lastSquare] = TRUE;
	    changedSquares[numChangedSquares] = lastSquare;

	    // Turn on display of square
	    int32_t *ind = gridFBIFS->coordIndex.startEditing();   // SDL port: LP64
	    int i  = numChangedSquares * 5;
	    int sq = lastSquare * 4;
	    gridFBIFS->coordIndex.set1Value(i + 0, sq + 0);
	    gridFBIFS->coordIndex.set1Value(i + 1, sq + 1);
	    gridFBIFS->coordIndex.set1Value(i + 2, sq + 2);
	    gridFBIFS->coordIndex.set1Value(i + 3, sq + 3);
	    gridFBIFS->coordIndex.set1Value(i + 4, SO_END_FACE_INDEX);
	    gridFBIFS->coordIndex.finishEditing();

	    numChangedSquares++;
	}
    }

    // In solve mode, change just the line of squares from the
    // starting square to the current one
    else {
	int row1, col1, row2, col2;
	getChangeRange(row1, col1, row2, col2);

	// Horizontal motion
	if (row1 == row2) {
	    gridFBIFS->coordIndex.setNum(col2 - col1 + 1);
	    int32_t *ind = gridFBIFS->coordIndex.startEditing();   // SDL port: LP64
	    int i = 0;
	    for (int col = col1; col <= col2; col++) {
		int sq = squareIndex(row1, col) * 4;
		gridFBIFS->coordIndex.set1Value(i + 0, sq + 0);
		gridFBIFS->coordIndex.set1Value(i + 1, sq + 1);
		gridFBIFS->coordIndex.set1Value(i + 2, sq + 2);
		gridFBIFS->coordIndex.set1Value(i + 3, sq + 3);
		gridFBIFS->coordIndex.set1Value(i + 4, SO_END_FACE_INDEX);
		i += 5;
	    }
	    gridFBIFS->coordIndex.finishEditing();
	}

	// Vertical motion
	else {
	    gridFBIFS->coordIndex.setNum(row2 - row1 + 1);
	    int32_t *ind = gridFBIFS->coordIndex.startEditing();   // SDL port: LP64
	    int i = 0;
	    for (int row = row1; row <= row2; row++) {
		int sq = squareIndex(row, col1) * 4;
		gridFBIFS->coordIndex.set1Value(i + 0, sq + 0);
		gridFBIFS->coordIndex.set1Value(i + 1, sq + 1);
		gridFBIFS->coordIndex.set1Value(i + 2, sq + 2);
		gridFBIFS->coordIndex.set1Value(i + 3, sq + 3);
		gridFBIFS->coordIndex.set1Value(i + 4, SO_END_FACE_INDEX);
		i += 5;
	    }
	    gridFBIFS->coordIndex.finishEditing();
	}
    }
}

void
Grid::finishFeedback()
{
    // Make changes to grid based on what is set in feedback
    beginChange();

    if (mode == PAINT) {
	for (int i = 0; i < numChangedSquares; i++) {
	    Square *sq = &squares[changedSquares[i]];
	    change(sq->row, sq->col, newState);
	}
    }

    else {
	// Find out squares that changed
	int row1, col1, row2, col2;
	getChangeRange(row1, col1, row2, col2);

	// Change grid
	if (row1 == row2)
	    for (int col = col1; col <= col2; col++)
		change(row1, col, newState);
	else
	    for (int row = row1; row <= row2; row++)
		change(row, col1, newState);
    }

    endChange();

    // Clear out feedback squares
    gridFBIFS->coordIndex.deleteValues(0);

    // Turn off feedback switch
    gridFBSwitch->whichChild = SO_SWITCH_NONE;
}

////////////////////////////////////////////////////////////////////////////
//
// Returns index of grid square containing given picked point. Returns
// -1 if no square is picked.
//
////////////////////////////////////////////////////////////////////////////

int
Grid::pickSquare(const SoPickedPoint *pp)
{
    int square;

    if (pp == NULL)
	square = -1;

    else {
	const SbVec3f &pt = pp->getPoint();

	int col = (int) pt[0], row = (int) -pt[1];

	if (col < 0 || row < 0 || col >= numCols || row >= numRows)
	    square = -1;

	else
	    square = squareIndex(row, col);
    }

    return square;
}


////////////////////////////////////////////////////////////////////////////
//
// Returns range of change made via interaction. row1 <= row2 and
// col1 <= col2.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::getChangeRange(int &row1, int &col1, int &row2, int &col2)
{
    row1 = startSquare / numCols;
    col1 = startSquare - row1 * numCols;
    row2 =  lastSquare / numCols;
    col2 =  lastSquare - row2 * numCols;

    // See which difference is greater, and use that direction
    int	rowDiff = row2 - row1, colDiff = col2 - col1;

    if (rowDiff < 0)
	rowDiff = -rowDiff;
    if (colDiff < 0)
	colDiff = -colDiff;

    if (rowDiff > colDiff)			// Primarily vertical
	col2 = col1;
    else					// Primarily horizontal
	row2 = row1;

    // Make sure row1 < row2 and col1 < col2

    if (row1 > row2) {
	int t = row1;
	row1 = row2;
	row2 = t;
    }

    if (col1 > col2) {
	int t = col1;
	col1 = col2;
	col2 = t;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Frees up list of changes starting with the given one.
//
////////////////////////////////////////////////////////////////////////////

void
Grid::freeChanges(Change *start)
{
    Change *change, *nextChange;

    for (change = start; change != NULL; change = nextChange) {
	nextChange = change->next;
	delete change;
    }
}
