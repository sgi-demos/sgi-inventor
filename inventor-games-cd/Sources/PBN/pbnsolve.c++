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
#include <X11/Intrinsic.h>

#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>

#include <Inventor/Xt/SoXt.h>
#include "pbn.h"
#include "MotifHelp.h"
#include "Message.h"

static Grid		*grid;
static Solver		*solver;

static Widget		mainWindow, pulldown, undoBut, redoBut;

////////////////////////////////////////////////////////////////////////////
//
// Callback for when the Edit pulldown menu is displayed. This is
// responsible for graying out disabled buttons.
//
////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////
//
// Callback for help.
//
////////////////////////////////////////////////////////////////////////////

static void
helpCB(Widget, XtPointer, XtPointer)
{
    if (system("showcase -v pbnHelp.sho"))
	fprintf(stderr, "Can't display help file\n");
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
    testGrid->copy(grid);

    Solver solver(testGrid);
    Solver::Result result = solver.solve();

    if (result == Solver::ERROR)
	new Message(mainWindow,
		    "An error occurred during solving.\n"
		    "Some squares must be incorrect.");

    else if (result == Solver::UNSOLVED)
	new Message(mainWindow,
		    "The program cannot solve\n"
		    "the puzzle from this state.");

    else if (result == Solver::SOLVED)
	new Message(mainWindow,
		    "The program found one\n"
		    "solution for the current state.");

    else
	new Message(mainWindow,
		    "The program found at least two\n"
		    "solutions for the current state.");

    delete testGrid;
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for solving one row or column.
//
////////////////////////////////////////////////////////////////////////////

static void
solveOneCB(Widget, XtPointer, XtPointer)
{
    Solver::Result result = solver->solveOne();

    if (result == Solver::ERROR)
	new Message(mainWindow,
		    "An error occurred during solving.\n"
		    "Some squares must be incorrect.");

    else if (result == Solver::UNSOLVED && ! grid->isSolved())
	new Message(mainWindow,
		    "The program cannot solve another\n"
		    "row or column without guessing.");
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for solving entire grid.
//
////////////////////////////////////////////////////////////////////////////

static void
solveAllCB(Widget, XtPointer, XtPointer)
{
    Solver::Result result = solver->solve();

    if (result == Solver::UNSOLVED)
	new Message(mainWindow,
		    "The program cannot\n"
		    "solve this puzzle.");

    if (result == Solver::MULTISOLVED)
	new Message(mainWindow,
		    "The program found at least two\n"
		    "solutions to this puzzle.");

    else if (result == Solver::ERROR)
	new Message(mainWindow,
		    "An error occurred during solving.\n"
		    "Some squares must be incorrect.");

}

////////////////////////////////////////////////////////////////////////////
//
// Builds the top-bar menu widget.
//
////////////////////////////////////////////////////////////////////////////

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
    // SOLVE menu
    //

    // Create a pulldown menu in the pop-up planes
    RESET_ARGS();
    SoXt::getPopupArgs(XtDisplay(menu), NULL, args, &argN);
    pulldown = XmCreatePulldownMenu(menu, "pulldown", ARGS);

    // Create a cascade button in the pulldown
    RESET_ARGS();
    ADD_ARG(XmNsubMenuId,	pulldown);
    ADD_ARG(XmNlabelString,	STRING("Solve"));
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

    // Separator
    but = XmCreateSeparatorGadget(pulldown, "separator", ARGS);
    XtManageChild(but);
    
    // Solve one button
    RESET_ARGS();
    ADD_ARG(XmNaccelerator,	"<Key> s");
    ADD_ARG(XmNacceleratorText, STRING("s"));
    ADD_ARG(XmNlabelString,	STRING("Solve one line"));
    but = XmCreatePushButtonGadget(pulldown, "SolveOne", ARGS);
    XtAddCallback(but, XmNactivateCallback, &solveOneCB, (XtPointer) NULL);
    XtManageChild(but);

    // Solve all button
    RESET_ARGS();
    ADD_ARG(XmNaccelerator,	"Shift <Key> s");
    ADD_ARG(XmNacceleratorText, STRING("S"));
    ADD_ARG(XmNlabelString,	STRING("Solve all"));
    but = XmCreatePushButtonGadget(pulldown, "SolveAll", ARGS);
    XtAddCallback(but, XmNactivateCallback, &solveAllCB, (XtPointer) NULL);
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

////////////////////////////////////////////////////////////////////////////
//
// Prints usage message, then exits.
//
////////////////////////////////////////////////////////////////////////////

static void
printUsage(const char *progName)
{
    fprintf(stderr, "usage: %s inputfile\n", progName);
    exit(1);
}

////////////////////////////////////////////////////////////////////////////
//
// Mainline.
//
////////////////////////////////////////////////////////////////////////////

main(int argc, char **argv)
{
    if (argc != 2)
	printUsage(argv[0]);

    FILE *gridFile = fopen(argv[1], "r");
    if (gridFile == NULL) {
	fprintf(stderr, "Can't read from file \"%s\"\n", argv[1]);
	printUsage(argv[0]);
    }

    mainWindow = SoXt::init(argv[0]);

    grid = new Grid(Grid::SOLVE);

    if (! grid->read(gridFile)) {
	fprintf(stderr, "Can't read grid from file \"%s\"\n", argv[1]);
	printUsage(argv[0]);
    }

    solver = new Solver(grid);

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

    SoXt::mainLoop();
}
