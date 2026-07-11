#include <stdlib.h>
#ifdef SDL_PORT
#include <SoSDLRenderArea.h>
#define SoXtRenderArea SoSDLRenderArea
typedef void *Widget;
typedef void *XtPointer;
#else
#include <X11/Intrinsic.h>
#endif

#include <Inventor/SbColor.h>

extern class SoBaseColor;
extern class SoCamera;
extern class SoCoordinate3;
extern class SoEventCallback;
extern class SoIndexedFaceSet;
extern class SoNode;
extern class SoPickedPoint;
extern class SoSwitch;
#ifndef SDL_PORT
extern class SoXtRenderArea;
#endif

////////////////////////////////////////////////////////////////////////////
//
// A square can be in one of these states:
//
////////////////////////////////////////////////////////////////////////////

enum SqState {
    UNKNOWN,		// No value set yet
    FILLED,		// Set to filled value
    EMPTY,		// Set to empty value
};

////////////////////////////////////////////////////////////////////////////
//
// A grid square.
//
////////////////////////////////////////////////////////////////////////////

struct Square {
    int		row, col;
    SqState	state;
    SqState	oldState;	// Used in Change class
    Square	*next;		// Used to create lists
};

////////////////////////////////////////////////////////////////////////////
//
// A row or column specification: a list of lengths of filled runs.
//
////////////////////////////////////////////////////////////////////////////

class Spec {
  public:
    int		num;
    int		*runs;

    Spec(int numRuns);
    ~Spec();
};

////////////////////////////////////////////////////////////////////////////
//
// Row or column of grid.
//
////////////////////////////////////////////////////////////////////////////

class Line {
  public:
    int		length;
    Square	**squares;
    Spec	*spec;

    Line();
    ~Line();
    void	setLength(int length);
    void	copy(const Line *from);

    // Returns index of first square that has the given state,
    // starting at the given index, going forward or backward.
    // Returns -1 if not found.
    int		findMatchForward(int start,  SqState state);
    int		findMatchBackward(int start, SqState state);

    // Same, but looks for the first square that doesn't match
    int		findNonMatchForward(int start,  SqState state);
    int		findNonMatchBackward(int start, SqState state);

    // Returns end of run of contiguous squares (of any state)
    // starting at given square
    int		findRunForward(int start);
    int		findRunBackward(int start);

    // Looks for enough room in the line for a filled run of the given
    // length, starting at the given square. Returns the index of the
    // first square for the room, or -1 if there is none.
    int		findRoomForward(int start,  int runLength);
    int		findRoomBackward(int start, int runLength);
};

////////////////////////////////////////////////////////////////////////////
//
// A change to one or more grid squares. Changes are stored as Square
// instances.
//
////////////////////////////////////////////////////////////////////////////

class Change {
  public:
    Square	*changes;
    Change	*next, *prev;		// Used to create lists

    Change();
    ~Change();
    void	add(int row, int col, SqState oldState, SqState newState);
};

////////////////////////////////////////////////////////////////////////////
//
// Grid class. Stores and displays an MxN grid of squares. Contains an
// SoXtRenderArea - it displays and allows interaction with a
// representation of the squares.
//
// A grid can be constructed in one of two modes. In PAINT mode, all
// squares are initialized to EMPTY and can be painted randomly to
// either FILLED or EMPTY. In SOLVE mode, squares are initialized to
// UNKNOWN and can be set in straight lines to FILLED, EMPTY, or
// UNKNOWN. Row and column specs are shown only in SOLVE mode.
//
////////////////////////////////////////////////////////////////////////////

class Grid {
  public:

    enum Mode {
	PAINT,
	SOLVE,
    };

    // Constructor
    Grid(Mode mode);

    //Destructor
    ~Grid();

    // Sets number of rows and columns
    void		setSize(int numRows, int numCols);

    // Reads grid specification from the given file. Returns FALSE on error.
    SbBool		read(FILE *fp);

    // Writes grid specification to the given file
    void		write(FILE *fp);

    // Copies from another grid
    void		copy(const Grid *copyFrom);

    // Builds and returns widget for grid
    Widget		buildWidget(Widget parent);
#ifdef SDL_PORT
    // The grid's event-callback node; the SDL port hangs its keyboard
    // shortcuts (the Motif menu equivalents) off it.
    SoEventCallback	*getEventCallback() const { return ecb; }
#endif

    // Clears all grid squares to initial color
    void		clear();

    // These are used to make changes to squares from outside
    void		beginChange();
    void		change(int row, int col, SqState newState);
    void		endChange();

    // Undo the last change/redo the last undone change
    void		undo();
    void		redo();

    // These return TRUE if it is possible to undo or redo
    SbBool		canUndo();
    SbBool		canRedo();

    // These return a row or column of the grid
    const Line *	getRow(int row)		{ return &rows[row]; }
    const Line *	getCol(int col)		{ return &cols[col]; }

    // Returns TRUE if entire grid is solved
    SbBool		isSolved() const;

  private:
    Mode		mode;
    int			numRows, numCols, numSquares;
    int			numRowSpecSpaces, numColSpecSpaces;
    Square		*squares;
    Line		*rows, *cols;
    SbBool		*squaresChanged;
    int			*changedSquares;
    int			numChangedSquares;
    SbBool		isBuilt;
    SoXtRenderArea	*renderArea;
    SbColor		colors[3];
    SoCamera		*gridCamera;
    SoBaseColor		*gridColor;
    SoCoordinate3	*gridCoord;
    SoSwitch		*gridFBSwitch;
    SoIndexedFaceSet	*gridFBIFS;
    SoBaseColor		*gridFBColor;
    SoEventCallback	*ecb;
    Change		*changes;
    Change		*curChange;
    Change		*openChange;
    SbColor		*changingColors;

    // Stuff changing with mouse interaction
    int			startSquare;		// Starting square
    int			lastSquare;		// Last square moved to
    SqState		newState;		// State changing to

    // Sets grid and window size
    void		setSizes();

    // Reads stuff
    static SbBool	readNum(FILE *fp, int &num);
    static Spec *	readSpec(FILE *fp);

    // Writes specification for the given row or column line
    static void		writeSpec(FILE *fp, Line *line);

    // Computes specification for the given row or column line
    static void		computeSpec(Line *line);

    // Builds and installs main scene graph
    void		buildGraph();

    // Builds and returns scene graph used for interaction feedback
    SoNode *		buildFeedbackGraph();

    // Builds subgraphs containing grid squares and spec text
    SoNode *		buildGridGraph();
    SoNode *		buildSpecGraph();

    // Builds subgraph for one piece of spec text
    SoNode *		build1SpecGraph(float x, float y, int runLength);

    // Processes a mouse button or mouse motion event
    static void		processButton(void *userData, SoEventCallback *ecb);
    static void		processMotion(void *userData, SoEventCallback *ecb);

    // Begins, continues, or ends a change to the grid resulting from
    // interaction
    void		beginInteraction(int square, SqState state);
    void		continueInteraction(int square);
    void		endInteraction();

    // Initializes graph for interaction feedback, changes it based on
    // current mouse position, or finishes processing.
    void		initFeedback();
    void		changeFeedback();
    void		finishFeedback();

    // Returns index of grid square containing given picked point.
    // Returns -1 if no square is picked.
    int			pickSquare(const SoPickedPoint *pp);

    // Returns range of change made via interaction. row1 <= row2 and
    // col1 <= col2.
    void		getChangeRange(int &row1, int &col1,
				       int &row2, int &col2);

    // Frees up list of changes starting with the given one
    void		freeChanges(Change *start);

    // Returns index of square in given row and column
    int			squareIndex(int row, int col)
	{ return row * numCols + col; }
};

////////////////////////////////////////////////////////////////////////////
//
// Solver class, which solves a grid.
//
////////////////////////////////////////////////////////////////////////////

class Solver {

  public:
    enum Result {
	UNSOLVED,		// Couldn't solve the grid
	SOLVED,			// Solved successfully
	MULTISOLVED,		// Multiple solutions
	ERROR,			// Some inconsistency in grid specification
    };

    // Constructor and destructor
    Solver(Grid *grid);
    ~Solver();

    // Solves entire grid, if possible, changing it
    Result		solve();

    // Solves part of a row or column of grid, if possible. Returns
    // SOLVED if any square changed, ERROR if an error occurred, and
    // UNSOLVED otherwise.
    Result		solveOne();

  private:
    // This structure represents a run of filled squares
    struct Run {
	int	length;		// Length of run
	int	start;		// Index of square that run starts at or after
	int	end;		// Index of square that run ends at or before
	SbBool	exclusive;	// TRUE if run is only one on range of squares
	SbBool	solved;		// TRUE if run has been solved completely
    };

    Grid		*grid;
    int			numRows, numCols;
    Run			*runs;
    int			numRuns;
    Line		*line;
    Line		*workRow, *workCol;
    Square		*workSquares;
    SbBool		squaresChanged;
    SbBool		runsChanged;

    // Solves full grid using whatever means necessary
    Solver::Result	solveFull();

    // Solves full grid using only deterministic means
    Solver::Result	solveDeterministic();

    // Solves a line. Returns SOLVED if any square changed, ERROR if
    // an error occurred, and UNSOLVED otherwise.
    Solver::Result	solveLine(Line *lineToSolve);

    // Sets up Run structures for current line
    SbBool		setUpRuns();

    ////////////////////////////////////////////////////////
    //
    // These all check for solutions or perform some solving work,
    // returning FALSE on error:
    //

    // Looks for runs that are exclusive over their ranges
    SbBool		checkExclusives();

    // Sees if position of first run (from either end) is forced by
    // filled squares
    SbBool		checkEnds();

    // Checks if any sequences of squares match only one run
    SbBool		checkSequences();

    // Updates all runs based on current grid
    SbBool		updateRuns();

    // Updates squares based on current set of runs
    SbBool		updateSquares();

    // Adjusts run to contained the given filled squares.
    SbBool		adjustRun(Run *run, int firstFilled, int lastFilled);

    // Changes given square of current line to new state
    void		setSquare(int index, SqState newState);

    // Marks run as having changed. Verifies run and returns FALSE on error.
    SbBool		markRunChanged(Run *run);

    // Checks run for errors. Returns FALSE on error.
    SbBool		verifyRun(Run *run);

    ////////////////////////////////////////////////////////
    //
    // These are all used for guessing when deterministic solutions
    // don't work:
    //

    // Guesses values for unknown squares until a result is found
    Result		guess();

    // Makes a guess for the given square. Returns a Result code.
    Result		guessOne(int row, int col);

    // Sets the given square to the given value and sees what happens.
    // First makes a copy of the grid so it is not changed. Returns a
    // Result code.
    Result		testOne(int row, int col, SqState state);

    // Returns first unknown square of grid starting at given square.
    // Returns FALSE if none left.
    SbBool		getFirstUnknown(int startRow, int startCol,
					int &row, int &col);
};
