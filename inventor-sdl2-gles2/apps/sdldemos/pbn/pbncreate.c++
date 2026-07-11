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
#ifndef SDL_PORT
#include <X11/Intrinsic.h>
#endif

#ifndef SDL_PORT
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#endif

#ifdef SDL_PORT
#include <SoSDL.h>
#define SoXt SoSDL
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoEventCallback.h>
#else
#include <Inventor/Xt/SoXt.h>
#endif
#include "pbn.h"
#ifndef SDL_PORT
#include "MotifHelp.h"
#endif
#include "Message.h"

static const char	*outFileName;
static Grid		*grid;

static Widget		mainWindow, pulldown, undoBut, redoBut;

////////////////////////////////////////////////////////////////////////////
//
// Callback for when the Edit pulldown menu is displayed. This is
// responsible for graying out disabled buttons.
//
////////////////////////////////////////////////////////////////////////////

#ifndef SDL_PORT
static void
editDisplayCB(Widget, XtPointer, XtPointer)
{
    ARG_VARS(5);

    RESET_ARGS();
    ADD_ARG(XmNsensitive, grid->canUndo());
    XtSetValues(undoBut, ARGS);

    RESET_ARGS();
    ADD_ARG(XmNsensitive, grid->canRedo());
    XtSetValues(redoBut, ARGS);
}
#endif /* !SDL_PORT */

////////////////////////////////////////////////////////////////////////////
//
// Callback for help.
//
////////////////////////////////////////////////////////////////////////////

static void
helpCB(Widget, XtPointer, XtPointer)
{
#ifdef SDL_PORT
    printf("\nPBN Create - paint a picture to make a puzzle.\n"
	   "  left click/drag   fill squares\n"
	   "  middle click/drag mark squares empty\n"
	   "  right click/drag  clear squares\n"
	   "  u undo   r redo   c clear   t test solvability\n"
	   "  w write puzzle to file   h this help   q quit\n\n");
#else
    if (system("showcase -v pbnHelp.sho"))
	fprintf(stderr, "Can't display help file\n");
#endif
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for writing grid to file.
//
////////////////////////////////////////////////////////////////////////////

static void
writeCB(Widget, XtPointer, XtPointer)
{
    FILE *outFile = fopen(outFileName, "w");
    grid->write(outFile);
    fclose(outFile);
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for quitting.
//
////////////////////////////////////////////////////////////////////////////

static void
quitCB(Widget, XtPointer, XtPointer)
{
    exit(0);
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for undo.
//
////////////////////////////////////////////////////////////////////////////

static void
undoCB(Widget, XtPointer, XtPointer)
{
    grid->undo();
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for redo.
//
////////////////////////////////////////////////////////////////////////////

static void
redoCB(Widget, XtPointer, XtPointer)
{
    grid->redo();
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for clear.
//
////////////////////////////////////////////////////////////////////////////

static void
clearCB(Widget, XtPointer, XtPointer)
{
    grid->clear();
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for testing solvability.
//
////////////////////////////////////////////////////////////////////////////

static void
testCB(Widget, XtPointer, XtPointer)
{
    // Construct a grid to try solving
    Grid *testGrid = new Grid(Grid::SOLVE);
    testGrid->copy(grid);		// Sets up specs
    testGrid->clear();			// Clears squares

    Solver solver(testGrid);
    Solver::Result result = solver.solve();

    if (result == Solver::UNSOLVED || result == Solver::ERROR)
	new Message(mainWindow,
		    "The program cannot\n"
		    "solve this puzzle.");

    else if (result == Solver::SOLVED)
	new Message(mainWindow,
		    "The program found one\n"
		    "solution to this puzzle.");

    else
	new Message(mainWindow,
		    "The program found at least two\n"
		    "solutions to this puzzle.");

    delete testGrid;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds the top-bar menu widget.
//
////////////////////////////////////////////////////////////////////////////

#ifndef SDL_PORT
static Widget
buildMenu(Widget parent)
{
    ARG_VARS(20);

    RESET_ARGS();
    Widget menu = XmCreateMenuBar(parent, "menuBar", ARGS);

    //
    // FILE menu
    //

    // Create a pulldown menu in the pop-up planes
    RESET_ARGS();
    SoXt::getPopupArgs(XtDisplay(menu), NULL, args, &argN);
    pulldown = XmCreatePulldownMenu(menu, "pulldown", ARGS);

    // Create a cascade button in the pulldown
    RESET_ARGS();
    ADD_ARG(XmNsubMenuId,	pulldown);
    ADD_ARG(XmNlabelString,	STRING("File"));
    Widget cascade = XmCreateCascadeButtonGadget(menu, "cascade", ARGS);
    XtManageChild(cascade);

    // Help button
    RESET_ARGS();
    ADD_ARG(XmNlabelString,	STRING("Help"));
    Widget but = XmCreatePushButtonGadget(pulldown, "Help", ARGS);
    XtAddCallback(but, XmNactivateCallback, &helpCB, (XtPointer) NULL);
    XtManageChild(but);

    // Separator
    but = XmCreateSeparatorGadget(pulldown, "separator", ARGS);
    XtManageChild(but);
    
    // Write button
    RESET_ARGS();
    ADD_ARG(XmNaccelerator,	"<Key> w");
    ADD_ARG(XmNacceleratorText, STRING("w"));
    ADD_ARG(XmNlabelString,	STRING("Write to file"));
    but = XmCreatePushButtonGadget(pulldown, "Write to file", ARGS);
    XtAddCallback(but, XmNactivateCallback, &writeCB, (XtPointer) NULL);
    XtManageChild(but);

    // Quit button
    RESET_ARGS();
    ADD_ARG(XmNaccelerator,	"Alt <Key> q");
    ADD_ARG(XmNacceleratorText, STRING("Alt+q"));
    ADD_ARG(XmNlabelString,	STRING("Quit"));
    but = XmCreatePushButtonGadget(pulldown, "Quit", ARGS);
    XtAddCallback(but, XmNactivateCallback, &quitCB, (XtPointer) NULL);
    XtManageChild(but);

    //
    // EDIT menu
    //

    // Create a pulldown menu in the pop-up planes
    RESET_ARGS();
    SoXt::getPopupArgs(XtDisplay(menu), NULL, args, &argN);
    pulldown = XmCreatePulldownMenu(menu, "pulldown", ARGS);
    XtAddCallback(pulldown, XmNmapCallback, editDisplayCB, (XtPointer) NULL);

    // Create a cascade button in the pulldown
    RESET_ARGS();
    ADD_ARG(XmNsubMenuId,	pulldown);
    ADD_ARG(XmNlabelString,	STRING("Edit"));
    cascade = XmCreateCascadeButtonGadget(menu, "cascade", ARGS);
    XtManageChild(cascade);

    // Undo button
    RESET_ARGS();
    ADD_ARG(XmNaccelerator,	"<Key> u");
    ADD_ARG(XmNacceleratorText, STRING("u"));
    ADD_ARG(XmNlabelString,	STRING("Undo"));
    but = XmCreatePushButtonGadget(pulldown, "Undo", ARGS);
    XtAddCallback(but, XmNactivateCallback, &undoCB, (XtPointer) NULL);
    XtManageChild(but);
    undoBut = but;

    // Redo button
    RESET_ARGS();
    ADD_ARG(XmNaccelerator,	"<Key> r");
    ADD_ARG(XmNacceleratorText, STRING("r"));
    ADD_ARG(XmNlabelString,	STRING("Redo"));
    but = XmCreatePushButtonGadget(pulldown, "Redo", ARGS);
    XtAddCallback(but, XmNactivateCallback, &redoCB, (XtPointer) NULL);
    XtManageChild(but);
    redoBut = but;

    // Separator
    but = XmCreateSeparatorGadget(pulldown, "separator", ARGS);
    XtManageChild(but);
    
    // Clear button
    RESET_ARGS();
    ADD_ARG(XmNaccelerator,	"<Key> c");
    ADD_ARG(XmNacceleratorText, STRING("c"));
    ADD_ARG(XmNlabelString,	STRING("Clear"));
    but = XmCreatePushButtonGadget(pulldown, "Clear", ARGS);
    XtAddCallback(but, XmNactivateCallback, &clearCB, (XtPointer) NULL);
    XtManageChild(but);

    //
    // TEST menu
    //

    // Create a pulldown menu in the pop-up planes
    RESET_ARGS();
    SoXt::getPopupArgs(XtDisplay(menu), NULL, args, &argN);
    pulldown = XmCreatePulldownMenu(menu, "pulldown", ARGS);

    // Create a cascade button in the pulldown
    RESET_ARGS();
    ADD_ARG(XmNsubMenuId,	pulldown);
    ADD_ARG(XmNlabelString,	STRING("Test"));
    cascade = XmCreateCascadeButtonGadget(menu, "cascade", ARGS);
    XtManageChild(cascade);

    // Test solvability button
    RESET_ARGS();
    ADD_ARG(XmNaccelerator,	"<Key> t");
    ADD_ARG(XmNacceleratorText, STRING("t"));
    ADD_ARG(XmNlabelString,	STRING("Test solvability"));
    but = XmCreatePushButtonGadget(pulldown, "Test Solvability", ARGS);
    XtAddCallback(but, XmNactivateCallback, &testCB, (XtPointer) NULL);
    XtManageChild(but);

    return menu;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds widget containing the grid and the pulldown menu.
//
////////////////////////////////////////////////////////////////////////////

static void
buildWidget()
{
    ARG_VARS(20);

    Widget form = XmCreateForm(mainWindow, "form", NULL, 0);
    Widget menu = buildMenu(form);
    Widget gridWidget = grid->buildWidget(form);

    RESET_ARGS();
    ADD_TOP_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_FORM(0);
    XtSetValues(menu, ARGS);

    RESET_ARGS();
    ADD_TOP_WIDGET(menu, 6);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_FORM(0);
    XtSetValues(gridWidget, ARGS);

    // Show the widgets
    XtManageChild(form);
    XtManageChild(menu);
    XtManageChild(gridWidget);
}
#endif /* !SDL_PORT */

#ifdef SDL_PORT
//
// Keyboard shortcuts stand in for the Motif menu (see helpCB).
//
static void
keyboardCB(void *, SoEventCallback *ecb)
{
    const SoEvent *ev = ecb->getEvent();

    if (SO_KEY_PRESS_EVENT(ev, U))	 { undoCB(NULL, NULL, NULL); }
    else if (SO_KEY_PRESS_EVENT(ev, R))	 { redoCB(NULL, NULL, NULL); }
    else if (SO_KEY_PRESS_EVENT(ev, C))	 { clearCB(NULL, NULL, NULL); }
    else if (SO_KEY_PRESS_EVENT(ev, T))	 { testCB(NULL, NULL, NULL); }
    else if (SO_KEY_PRESS_EVENT(ev, W))	 { writeCB(NULL, NULL, NULL); }
    else if (SO_KEY_PRESS_EVENT(ev, H))	 { helpCB(NULL, NULL, NULL); }
    else if (SO_KEY_PRESS_EVENT(ev, Q))	 { quitCB(NULL, NULL, NULL); }
    else return;
    ecb->setHandled();
}
#endif

////////////////////////////////////////////////////////////////////////////
//
// Prints usage message, then exits.
//
////////////////////////////////////////////////////////////////////////////

static void
printUsage(const char *progName)
{
    fprintf(stderr,
	    "usage: %s numRows numCols outputFile\n"
	    "Number of rows and columns must be between 10 and 50\n",
	    progName);
    exit(1);
}

////////////////////////////////////////////////////////////////////////////
//
// Mainline.
//
////////////////////////////////////////////////////////////////////////////

int
main(int argc, char **argv)
{
#ifdef SDL_PORT
    setenv("FL_FONT_PATH", "fonts", 0);
    SoSDL::init();
    mainWindow = NULL;
#else
    mainWindow = SoXt::init(argv[0]);
#endif

    grid = new Grid(Grid::PAINT);

    if (argc == 4) {
	int numRows = atoi(argv[1]);
	int numCols = atoi(argv[2]);

	if (numRows < 10 || numRows > 50 || numCols < 10 || numCols > 50)
	    printUsage(argv[0]);

	grid->setSize(numRows, numCols);

	outFileName = argv[3];
	FILE *outFile = fopen(outFileName, "w");
	if (outFile == NULL) {
	    fprintf(stderr, "Can't write to file \"%s\"\n",
		    argv[0], outFileName);
	    printUsage(argv[0]);
	}
	fclose(outFile);
    }

    else
	printUsage(argv[0]);

#ifdef SDL_PORT
    grid->buildWidget(NULL);
    grid->getEventCallback()->addEventCallback(
	SoKeyboardEvent::getClassTypeId(), keyboardCB, NULL);
    printf("PBN Create - press 'h' in the window for help.\n");
#else
    buildWidget();

    SoXt::show(mainWindow);

    // Make sure window can't be resized
    short sx, sy;
    XtVaGetValues(mainWindow, XmNwidth, &sx, XmNheight, &sy, NULL);
    XtVaSetValues(mainWindow,
		  XmNminWidth, sx, XmNmaxWidth, sx,
		  XmNminHeight, sy, XmNmaxHeight, sy, NULL);

    // Install the correct color map for the pulldown menus
    SoXt::addColormapToShell(pulldown, mainWindow);
#endif

    SoXt::mainLoop();
    return 0;
}
