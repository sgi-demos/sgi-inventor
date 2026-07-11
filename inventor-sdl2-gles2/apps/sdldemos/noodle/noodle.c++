/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */
//
// Generalized Cylinder program
//

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <unistd.h>

#include <Inventor/SoPickedPoint.h>
#ifdef SDL_PORT
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/manips/SoHandleBoxManip.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <SoSDL.h>
#include <SoSDLRenderArea.h>
#include <SoSDLExaminerViewer.h>
#define SoXt SoSDL
typedef void *Widget;
typedef void *XtPointer;
#else
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/Xt/viewers/SoXtPlaneViewer.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoShapeHints.h>

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#endif /* SDL_PORT */

#include "LineManip.h"
#include "GeneralizedCylinder.h"
#ifndef SDL_PORT
#include "NoodleSlider.h"
#endif
#include "Interface.h"
#include "WorldInfo.h"
#ifdef SDL_PORT
#include "PullDowns.h"
#endif

////////////////////////////////////////////////////
// Print the usage message
//
static void
print_usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [ -h][-bg r g b] [infiles]\n", progname);
    fprintf(stderr,
	    "\t-h           Help (Print this message)\n"
	    "\t-bg rgb      Background color triple for main renderArea\n"
	    "\tinfiles      Inventor Files to read into Scene\n"
	    );
    exit(0);
}

////////////////////////////////////////////////////
// Parse the command line arguments
//
static void
parse_args(int argc, char **argv, char *&in_filename, SbColor in_bgColor )
{
    int curArg = 1;
    SbBool moreOpts = TRUE;

    while ( moreOpts && curArg < argc ) {
	if ( !strcmp(argv[curArg], "-bg")) {
	    curArg++;
	    float r, g, b;
	    sscanf( argv[curArg++], "%f", &r );
	    sscanf( argv[curArg++], "%f", &g );
	    sscanf( argv[curArg++], "%f", &b );
	    in_bgColor.setValue( r, g, b );
	}
	else if ( !strcmp(argv[curArg], "-h")) {
	    print_usage(argv[0]);
	    exit(0);
	}
	else if ( !strncmp(argv[curArg], "-", 1)) {
	    print_usage(argv[0]);
	    exit(0);
	}
	else
	    moreOpts = FALSE;
    }

    // Remaining argument should be fileName.
    if (argc == curArg)
	in_filename = NULL;
    else
	in_filename = strdup( argv[curArg] );
}

//
// These are defined in profile.c++
//
extern void clearProfilePoints();
extern void clearCrossSectionPoints();
extern void clearSpinePoints();
extern void clearTwistPoints();
extern void makeReverseSection();
extern void makeCircularSection();
extern void makeCircularSpine();
extern void makeSemiCircularSpine();

#ifndef SDL_PORT
Widget closeProfileButton;
Widget closeSectionButton;
Widget closeSpineButton;
Widget closeTwistButton;

SoXtPlaneViewer *profilePlaneViewer;
SoXtPlaneViewer *crossSectionPlaneViewer;
SoXtExaminerViewer *spinePlaneViewer;
SoXtPlaneViewer *twistPlaneViewer;
SoXtExaminerViewer *examViewer;


//
// Callback for the close buttons
//
void
closeProfileCallback(Widget w, XtPointer data, XtPointer)
{
    SbBool test = XmToggleButtonGetState( w );
    Interface  *stuff = (Interface *) data;
    GeneralizedCylinder *s = stuff->getWorldInfo()->getCurrentNoodle();
    if ( s != NULL )
	s->profileClosed = test;

}
void
closeSectionCallback(Widget w, XtPointer data, XtPointer)
{
    SbBool test = XmToggleButtonGetState( w );
    Interface  *stuff = (Interface *) data;
    GeneralizedCylinder *s = stuff->getWorldInfo()->getCurrentNoodle();
    if ( s != NULL )
	s->crossSectionClosed = test;

}
void
closeSpineCallback(Widget w, XtPointer data, XtPointer)
{
    SbBool test = XmToggleButtonGetState( w );
    Interface  *stuff = (Interface *) data;
    GeneralizedCylinder *s = stuff->getWorldInfo()->getCurrentNoodle();
    if ( s != NULL )
	s->spineClosed = test;

}
void
closeTwistCallback(Widget w, XtPointer data, XtPointer)
{
    SbBool test = XmToggleButtonGetState( w );
    Interface  *stuff = (Interface *) data;
    GeneralizedCylinder *s = stuff->getWorldInfo()->getCurrentNoodle();
    if ( s != NULL )
	s->twistClosed = test;

}

//
// Callback for the clear buttons
//
void
clearProfileCallback(Widget, XtPointer data, XtPointer)
{
    clearProfilePoints();

    // Also, set the curve to not be closed. It's really irritating
    // to have to clear, then un-close the polygon. So do it automatically
    // Note that, all we have to do is set the toggle. By passing the
    // third argument as TRUE, the closeCallback gets called for us!
    Widget closeToggle = (Widget) data;
    XmToggleButtonSetState( closeToggle, FALSE, TRUE );
}

void
clearSectionCallback(Widget, XtPointer  data, XtPointer)
{
    clearCrossSectionPoints();

    // Also, set the curve to not be closed. It's really irritating
    // to have to clear, then un-close the polygon. So do it automatically
    // Note that, all we have to do is set the toggle. By passing the
    // third argument as TRUE, the closeCallback gets called for us!
    Widget closeToggle = (Widget) data;
    XmToggleButtonSetState( closeToggle, FALSE, TRUE );
}

void
clearSpineCallback(Widget, XtPointer  data, XtPointer)
{
    clearSpinePoints();

    // Also, set the curve to not be closed. It's really irritating
    // to have to clear, then un-close the polygon. So do it automatically
    // Note that, all we have to do is set the toggle. By passing the
    // third argument as TRUE, the closeCallback gets called for us!
    Widget closeToggle = (Widget) data;
    XmToggleButtonSetState( closeToggle, FALSE, TRUE );
}

void
reverseSectionCallback(Widget, XtPointer, XtPointer)
{
    makeReverseSection();
}

void
circularSectionCallback(Widget, XtPointer, XtPointer)
{
    makeCircularSection();
}

void
circularSpineCallback(Widget, XtPointer, XtPointer)
{
    makeCircularSpine();
}

void
semiCircularSpineCallback(Widget, XtPointer, XtPointer)
{
    makeSemiCircularSpine();
}

void
clearTwistCallback(Widget, XtPointer  data, XtPointer)
{
    clearTwistPoints();

    // Also, set the curve to not be closed. It's really irritating
    // to have to clear, then un-close the polygon. So do it automatically
    // Note that, all we have to do is set the toggle. By passing the
    // third argument as TRUE, the closeCallback gets called for us!
    Widget closeToggle = (Widget) data;
    XmToggleButtonSetState( closeToggle, FALSE, TRUE );
}

void
quitCallback(Widget, void *intrf, void * )
{
    Interface *interface = (Interface *) intrf;
    interface->fileQuitEvent();
}
#endif /* !SDL_PORT */

#ifdef SDL_PORT
//
// SDL flavor: five windows (main viewer + four curve editors) and a
// keyboard in place of the pulldown menus and per-editor buttons.
//
static Interface *theInterface = NULL;

extern void profileEditorLeft();
extern void crossSectionEditorLeft();
extern void spineEditorLeft();
extern void twistEditorLeft();

static void
printHelp()
{
    printf("Noodle keys (any window):\n"
	   "  1-6  surface style: face set, tri strips, quad mesh,\n"
	   "       cubic spline, cubic-to-edge, bezier\n"
	   "  s/t/b  toggle sides / top cap / bottom cap\n"
	   "  m    cycle manipulator (handlebox, trackball, none)\n"
	   "  e    new noodle    d delete    u undelete\n"
	   "  c    copy noodle as .iv to stdout (C: whole scene)\n"
	   "  n    new scene    o re-open file    w save (noodle.iv)\n"
	   "  W    save as noodle-saveas.iv    v save vanilla .iv\n"
	   "  x    toggle texture    f flip normals\n"
	   "  a    about    h this help    q quit\n"
	   "In an editor window: click to add points, drag to move them,\n"
	   "  k    toggle closed curve    l clear curve\n"
	   "  r    reverse (section)    i circle (section/spine)\n"
	   "  j    semi-circle (spine)\n");
}

static bool
handleNoodleKey(SDL_Keycode key, Uint16 mod)
{
    WorldInfo *w = theInterface->getWorldInfo();
    GeneralizedCylinder *g = w->getCurrentNoodle();
    SbBool shifted = (mod & KMOD_SHIFT) != 0;

    switch (key) {
      case SDLK_1: theInterface->menuAction(MM_SHAPE_FACE_SET); return true;
      case SDLK_2: theInterface->menuAction(MM_SHAPE_TRI_STRIP); return true;
      case SDLK_3: theInterface->menuAction(MM_SHAPE_QUAD_MESH); return true;
      case SDLK_4: theInterface->menuAction(MM_SHAPE_CUBIC_SPLINE); return true;
      case SDLK_5: theInterface->menuAction(MM_SHAPE_CUBIC_TO_EDGE); return true;
      case SDLK_6: theInterface->menuAction(MM_SHAPE_BEZIER); return true;
      case SDLK_s: theInterface->menuAction(MM_PARTS_SIDES); return true;
      case SDLK_t: theInterface->menuAction(MM_PARTS_TOP_CAP); return true;
      case SDLK_b: theInterface->menuAction(MM_PARTS_BOT_CAP); return true;
      case SDLK_m: {
	// Cycle handlebox -> trackball -> none
	SoType cur = w->getManipType();
	int next = (cur == SoHandleBoxManip::getClassTypeId()) ? MM_MANIPS_TRACKBALL
		 : (cur == SoTrackballManip::getClassTypeId()) ? MM_MANIPS_NONE
		 : MM_MANIPS_HBOX;
	theInterface->menuAction(next);
	return true;
      }
      case SDLK_e: theInterface->menuAction(MM_EDIT_NEW); return true;
      case SDLK_d: theInterface->menuAction(MM_EDIT_DELETE); return true;
      case SDLK_u: theInterface->menuAction(MM_EDIT_UNDELETE); return true;
      case SDLK_c:
	theInterface->menuAction(shifted ? MM_EDIT_COPY_ALL : MM_EDIT_COPY);
	return true;
      case SDLK_n: theInterface->menuAction(MM_FILE_NEW); return true;
      case SDLK_o: theInterface->menuAction(MM_FILE_OPEN); return true;
      case SDLK_w:
	theInterface->menuAction(shifted ? MM_FILE_SAVE_AS : MM_FILE_SAVE);
	return true;
      case SDLK_v: theInterface->menuAction(MM_FILE_SAVE_VANILLA); return true;
      case SDLK_x: theInterface->menuAction(MM_GIZMOS_TEXTURE); return true;
      case SDLK_f: theInterface->menuAction(MM_GIZMOS_SURFACE); return true;
      case SDLK_a: theInterface->menuAction(MM_ABOUT_ABOUT); return true;
      case SDLK_h: printHelp(); return true;
      case SDLK_q: theInterface->menuAction(MM_FILE_QUIT); return true;
    }
    (void)g;
    return false;
}

class NoodleViewer : public SoSDLExaminerViewer {
  public:
    NoodleViewer(const char *t, int w, int h) : SoSDLExaminerViewer(t, w, h) {}
    virtual void processEvent(const SDL_Event *e) {
	if (e->type == SDL_KEYDOWN &&
	    handleNoodleKey(e->key.keysym.sym, e->key.keysym.mod))
	    return;
	SoSDLExaminerViewer::processEvent(e);
    }
};

class EditorArea : public SoSDLRenderArea {
  public:
    enum Kind { PROFILE, SECTION, SPINE, TWIST };
    EditorArea(Kind k, const char *t, int w, int h)
	: SoSDLRenderArea(t, w, h), kind(k) {}

    virtual void processEvent(const SDL_Event *e) {
	if (e->type == SDL_WINDOWEVENT &&
	    e->window.event == SDL_WINDOWEVENT_LEAVE) {
	    switch (kind) {
	      case PROFILE: profileEditorLeft(); break;
	      case SECTION: crossSectionEditorLeft(); break;
	      case SPINE:   spineEditorLeft(); break;
	      case TWIST:   twistEditorLeft(); break;
	    }
	}
	if (e->type == SDL_KEYDOWN && editorKey(e->key.keysym.sym))
	    return;
	if (e->type == SDL_KEYDOWN &&
	    handleNoodleKey(e->key.keysym.sym, e->key.keysym.mod))
	    return;
	SoSDLRenderArea::processEvent(e);
    }

  private:
    // The per-editor buttons of the original: Close (toggle),
    // Clear, and the section/spine shape presets.
    bool editorKey(SDL_Keycode key) {
	GeneralizedCylinder *g =
	    theInterface->getWorldInfo()->getCurrentNoodle();
	if (!g) return false;
	switch (key) {
	  case SDLK_k:	// toggle closed curve
	    switch (kind) {
	      case PROFILE: g->profileClosed = !g->profileClosed.getValue(); break;
	      case SECTION: g->crossSectionClosed = !g->crossSectionClosed.getValue(); break;
	      case SPINE:   g->spineClosed = !g->spineClosed.getValue(); break;
	      case TWIST:   g->twistClosed = !g->twistClosed.getValue(); break;
	    }
	    return true;
	  case SDLK_l:	// clear curve (and un-close it, like the buttons)
	    switch (kind) {
	      case PROFILE: clearProfilePoints(); g->profileClosed = FALSE; break;
	      case SECTION: clearCrossSectionPoints(); g->crossSectionClosed = FALSE; break;
	      case SPINE:   clearSpinePoints(); g->spineClosed = FALSE; break;
	      case TWIST:   clearTwistPoints(); g->twistClosed = FALSE; break;
	    }
	    return true;
	  case SDLK_r:
	    if (kind == SECTION) { makeReverseSection(); return true; }
	    return false;
	  case SDLK_i:
	    if (kind == SECTION) { makeCircularSection(); return true; }
	    if (kind == SPINE)   { makeCircularSpine(); return true; }
	    return false;
	  case SDLK_j:
	    if (kind == SPINE)   { makeSemiCircularSpine(); return true; }
	    return false;
	}
	return false;
    }

    Kind kind;
};

int
main(int argc, char **argv)
{
    char    *in_filename = NULL;
    SbColor in_bgColor(0,0,0);
    parse_args(argc, argv, in_filename, in_bgColor);

    SoSDL::init();
    GeneralizedCylinder::initClass();
    LineManip2::initClass();

    Interface *interface = new Interface();
    theInterface = interface;

    WorldInfo *worldInfo = new WorldInfo;
    interface->setWorldInfo(worldInfo);
    worldInfo->setFileName(in_filename);
    interface->setBackgroundColor(in_bgColor);
    interface->readScene(NULL, TRUE);

    GeneralizedCylinder *curNoodle = worldInfo->getCurrentNoodle();
    if (!curNoodle)
	curNoodle = worldInfo->addNewNoodle();

    // The original was one 1200x800 form: menus on top, the main view
    // top-right, the profile editor top-left, and the section, spine
    // and twist editors across the bottom. Here: five windows. The
    // main viewer is created LAST: on the web, the single canvas
    // belongs to the most recently created window, and the 3D view is
    // the one worth seeing there.
    EditorArea *profileEd = new EditorArea(EditorArea::PROFILE,
					   "Noodle Profile", 380, 300);
    EditorArea *sectionEd = new EditorArea(EditorArea::SECTION,
					   "Noodle Cross Section", 380, 300);
    EditorArea *spineEd   = new EditorArea(EditorArea::SPINE,
					   "Noodle Spine", 380, 300);
    EditorArea *twistEd   = new EditorArea(EditorArea::TWIST,
					   "Noodle Twist", 380, 300);

    NoodleViewer *examViewer = new NoodleViewer("Noodle", 720, 480);
    examViewer->setBackgroundColor(interface->getBackgroundColor());
    interface->setMainViewer(examViewer);
    examViewer->setSceneGraph(worldInfo->getWorldRoot());
    examViewer->redrawOnSelectionChange(worldInfo->getSelectorNode());
    SoBoxHighlightRenderAction *hlRA = new SoBoxHighlightRenderAction;
    hlRA->setVisible(TRUE);
    examViewer->setGLRenderAction(hlRA);
    interface->setProfileViewer(profileEd);
    interface->setSectionViewer(sectionEd);
    interface->setSpineViewer(spineEd);
    interface->setTwistViewer(twistEd);

    examViewer->viewAll();

    // Wires the editor scenes to the current noodle.
    interface->setPrimarySelection(curNoodle);

    printHelp();
    SoSDL::mainLoop();
    return 0;
}
#else /* !SDL_PORT */

int
main(int argc, char **argv)
{
    // Parse command line arguments. This may fill in the 
    // background color and fileName if successful.
	char    *in_filename = NULL;
	SbColor in_bgColor(0,0,0);
	parse_args(argc, argv, in_filename, in_bgColor );

    // Initialize Inventor
	Widget mainWindow = SoXt::init(argv[0]);
	if (mainWindow == NULL) exit(1);

    // Give title to main window. Include version number and date.
        XtVaSetValues(mainWindow,XtNtitle,"noodle - V1.0.2 - 9.19.94",NULL);

    // Initialize special classes.
	GeneralizedCylinder::initClass();
	LineManip2::initClass();

    // Create an interface.  
	Interface *interface = new Interface();

    // Tell mainWindow to call the quit callback when user double clicks
    // upper left button or selects quit from main window menu.
	// First, tell it to do nothing automatically
        XtVaSetValues(mainWindow,XmNdeleteResponse, XmDO_NOTHING, NULL);
	Atom vmDeleteAtom = XmInternAtom(XtDisplay(mainWindow),
		"WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(mainWindow, vmDeleteAtom,
		(XtCallbackProc) quitCallback, (XtPointer) interface);

    // Create a structure to hold our scene and pass it to the interface.
	WorldInfo *worldInfo = new WorldInfo;
	interface->setWorldInfo( worldInfo );

    // Set the filename and background color for the interface.
        worldInfo->setFileName( in_filename );
	interface->setBackgroundColor( in_bgColor );

    // Read the scene into worldInfo, if it received a filename
    // during parse_args. This will also set the current noodle to 
    // be the first noodle in found in the scene.
	interface->readScene(NULL, TRUE);

    // If the scene was empty, add a noodle to it.
	GeneralizedCylinder *curNoodle = worldInfo->getCurrentNoodle();
	if ( !curNoodle )
	    curNoodle = worldInfo->addNewNoodle();

    // Create the form that surrounds the five viewers and the pulldown
    // menus.
	Arg resources[20];
	int n = 0;
	XtSetArg(resources[n], "width", 1200); n++;
	XtSetArg(resources[n], "height", 800); n++;
	Widget form = XmCreateForm(mainWindow, "form", resources, n); n = 0;

    // Build the interface (pulldowns). Get back the topmost widget it creates.
	Widget menuWidget = interface->build( form );

#define STRING(a) XmStringCreate(a,XmSTRING_DEFAULT_CHARSET)
    
    // And the five viewers (one to edit the cross section, one to
    // edit the spine, one to edit the profile (scale of the cross section as
    // it travels along the spine), one to edit the twist of the crsoss section,
    // and one to view the shaded geometry)
    

    //*************
    // View of complete 3d model
    //*************
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNtopWidget, menuWidget); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNleftPosition, 33); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNbottomPosition, 45); n++;
    Widget frame2 = XmCreateFrame(form, "renderAreaFrame", resources,
				  n); n = 0;

    // Create the examiner viewer...
    examViewer = new SoXtExaminerViewer(frame2, "noodleMainWindow");
    examViewer->setBackgroundColor( interface->getBackgroundColor() );

    interface->setMainViewer( examViewer );

    // Set it as part of the callback data.
    // Scene graph
	examViewer->setSceneGraph( worldInfo->getWorldRoot() );
    // Selection
	examViewer->redrawOnSelectionChange( worldInfo->getSelectorNode() );
    // Highlighting
	SoBoxHighlightRenderAction *hlRA = new SoBoxHighlightRenderAction;
	hlRA->setVisible(TRUE);
	examViewer->setGLRenderAction(hlRA);

    // How it looks/works
	examViewer->setBorder(FALSE);
	examViewer->setAnimationEnabled(FALSE);
	examViewer->show();
    XtManageChild(frame2);
    
    //*************
    // PROFILE VIEW
    //*************
    // Form containing viewer and controls
    n = 0;
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNtopWidget, menuWidget); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNrightPosition, 33); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNbottomPosition, 45); n++;
    Widget form1 = XmCreateForm(form, "profileViewForm", resources,
				  n); n = 0;

    // Row Column - goes along bottom edge of form, contains a button.
    XtSetArg(resources[n], XmNorientation, XmHORIZONTAL); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNadjustLast, FALSE); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget rc1 = XmCreateRowColumn(form1, "profileStuff",
				  resources, n); n = 0;
    // Close profile button
    XtSetArg(resources[n], XmNlabelString, STRING("Close Profile")); ++n;
    closeProfileButton = XmCreateToggleButtonGadget(rc1, "closeProfile",
						 resources, n); n = 0;
    XmToggleButtonSetState( closeProfileButton, 
				curNoodle->profileClosed.getValue(), FALSE );
    XtAddCallback(closeProfileButton, XmNvalueChangedCallback,
		  closeProfileCallback, (XtPointer)interface);
    XtManageChild(closeProfileButton);
    XtManageChild(rc1);

    // Clear profile button
    XtSetArg(resources[n], XmNlabelString, STRING("Clear Profile")); ++n;
    Widget clearProfileButton = XmCreatePushButtonGadget(rc1, "clearProfile",
						 resources, n); n = 0;
    XtAddCallback(clearProfileButton, XmNactivateCallback,
		  clearProfileCallback, (XtPointer)closeProfileButton);
    XtManageChild(clearProfileButton);
    XtManageChild(rc1);

    // Frame = top Section of Form
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, rc1); n++;
    Widget frame1 = XmCreateFrame(form1, "profileViewFrame", resources,
				  n); n = 0;
    // Put viewer in frame
    profilePlaneViewer = new SoXtPlaneViewer(frame1);
    profilePlaneViewer->setBorder(FALSE);
    profilePlaneViewer->setDecoration(FALSE);
    profilePlaneViewer->setViewing(FALSE);
    profilePlaneViewer->show();
    interface->setProfileViewer( profilePlaneViewer );
    interface->setProfileCloseButton( closeProfileButton );
    XtManageChild(frame1);

    XtManageChild(form1);

    //*************
    // CROSS SECTION VIEW
    //*************
    // Form containing viewer and controls
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNtopPosition, 45); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNrightPosition, 33); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget form3 = XmCreateForm(form, "crossSectionViewForm", resources,
				  n); n = 0;

    // Row Column - goes along bottom edge of form, contains a button.
    XtSetArg(resources[n], XmNorientation, XmHORIZONTAL); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNadjustLast, FALSE); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget rc3 = XmCreateRowColumn(form3, "crossSectionStuff",
				  resources, n); n = 0;
    // Close crossSection button
    XtSetArg(resources[n], XmNlabelString, STRING("Close Section")); ++n;
    closeSectionButton = XmCreateToggleButtonGadget(rc3, "closeSection",
						 resources, n); n = 0;
    XmToggleButtonSetState( closeSectionButton, 
			    curNoodle->crossSectionClosed.getValue(), FALSE );
    XtAddCallback(closeSectionButton, XmNvalueChangedCallback,
		  closeSectionCallback, (XtPointer)interface);
    XtManageChild(closeSectionButton);
    XtManageChild(rc3);

    // Clear crossSection button
    XtSetArg(resources[n], XmNlabelString, STRING("Clear Section")); ++n;
    Widget clearSectionButton = XmCreatePushButtonGadget(rc3, "clearSection",
						 resources, n); n = 0;
    XtAddCallback(clearSectionButton, XmNactivateCallback,
		  clearSectionCallback, (XtPointer)closeSectionButton);
    XtManageChild(clearSectionButton);
    XtManageChild(rc3);

    // circular section button
    XtSetArg(resources[n], XmNlabelString, STRING("Circle")); ++n;
    Widget circularSectionButton = XmCreatePushButtonGadget(rc3,"circle",
						 resources, n); n = 0;
    XtAddCallback(circularSectionButton, XmNactivateCallback,
		  circularSectionCallback, (XtPointer)circularSectionButton);
    XtManageChild(circularSectionButton);
    XtManageChild(rc3);

    // reverse section button
    XtSetArg(resources[n], XmNlabelString, STRING("Reverse Order")); ++n;
    Widget reverseSectionButton = XmCreatePushButtonGadget(rc3,"circle",
						 resources, n); n = 0;
    XtAddCallback(reverseSectionButton, XmNactivateCallback,
		  reverseSectionCallback, (XtPointer)reverseSectionButton);
    XtManageChild(reverseSectionButton);
    XtManageChild(rc3);

    // Frame = top Section of Form
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, rc3); n++;
    Widget frame3 = XmCreateFrame(form3, "crossSectionViewFrame", resources,
				  n); n = 0;
    // Put viewer in frame
    crossSectionPlaneViewer = new SoXtPlaneViewer(frame3);
    crossSectionPlaneViewer->setBorder(FALSE);
    crossSectionPlaneViewer->setDecoration(FALSE);
    crossSectionPlaneViewer->setViewing(FALSE);
    crossSectionPlaneViewer->show();
    interface->setSectionViewer( crossSectionPlaneViewer );
    interface->setSectionCloseButton( closeSectionButton );
    XtManageChild(frame3);

    XtManageChild(form3);


    //*************
    // SPINE VIEW
    //*************
    // Form containing viewer and controls
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNtopPosition, 45); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNleftPosition, 33); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNrightPosition, 66); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget form4 = XmCreateForm(form, "spineViewForm", resources,
				  n); n = 0;

    // Row Column - goes along bottom edge of form, contains a button.
    XtSetArg(resources[n], XmNorientation, XmHORIZONTAL); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNadjustLast, FALSE); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget rc4 = XmCreateRowColumn(form4, "spineStuff",
				  resources, n); n = 0;
    // Close spine button
    XtSetArg(resources[n], XmNlabelString, STRING("Close Spine")); ++n;
    closeSpineButton = XmCreateToggleButtonGadget(rc4, "closeSpine",
						 resources, n); n = 0;
    XmToggleButtonSetState( closeSpineButton, 
				curNoodle->spineClosed.getValue(), FALSE );
    XtAddCallback(closeSpineButton, XmNvalueChangedCallback,
		  closeSpineCallback, (XtPointer)interface);
    XtManageChild(closeSpineButton);
    XtManageChild(rc4);

    // Clear spine button
    XtSetArg(resources[n], XmNlabelString, STRING("Clear Spine")); ++n;
    Widget clearSpineButton = XmCreatePushButtonGadget(rc4, "clearSpine",
						 resources, n); n = 0;
    XtAddCallback(clearSpineButton, XmNactivateCallback,
		  clearSpineCallback, (XtPointer)closeSpineButton);
    XtManageChild(clearSpineButton);
    XtManageChild(rc4);

    // circular spine button
    XtSetArg(resources[n], XmNlabelString, STRING("Circle")); ++n;
    Widget circularSpineButton = XmCreatePushButtonGadget(rc4,"circle",
						 resources, n); n = 0;
    XtAddCallback(circularSpineButton, XmNactivateCallback,
		  circularSpineCallback, (XtPointer)circularSpineButton);
    XtManageChild(circularSpineButton);
    XtManageChild(rc4);

    // Semi-circular spine button
    XtSetArg(resources[n], XmNlabelString, STRING("Semi-circle")); ++n;
    Widget semiCircularSpineButton = XmCreatePushButtonGadget(rc4, "semiCircle",
						 resources, n); n = 0;
    XtAddCallback(semiCircularSpineButton, XmNactivateCallback,
		 semiCircularSpineCallback, (XtPointer)semiCircularSpineButton);
    XtManageChild(semiCircularSpineButton);
    XtManageChild(rc4);

    // Frame = top Section of Form
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, rc4); n++;
    Widget frame4 = XmCreateFrame(form4, "spineViewFrame", resources,
				  n); n = 0;
    // Put viewer in frame
    spinePlaneViewer = new SoXtExaminerViewer(frame4);
    spinePlaneViewer->setBorder(FALSE);
    spinePlaneViewer->setDecoration(FALSE);
    spinePlaneViewer->setViewing(FALSE);
    spinePlaneViewer->show();
    interface->setSpineViewer( spinePlaneViewer );
    interface->setSpineCloseButton( closeSpineButton );
    XtManageChild(frame4);

    XtManageChild(form4);

    //*************
    // TWIST VIEW
    //*************
    // Form containing viewer and controls
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNtopPosition, 45); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNleftPosition, 66); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget form5 = XmCreateForm(form, "twistViewForm", resources,
				  n); n = 0;

    // Row Column - goes along bottom edge of form, contains a button.
    XtSetArg(resources[n], XmNorientation, XmHORIZONTAL); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNadjustLast, FALSE); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget rc5 = XmCreateRowColumn(form5, "twistStuff",
				  resources, n); n = 0;
    // Close twist button
    XtSetArg(resources[n], XmNlabelString, STRING("Close Twist")); ++n;
    closeTwistButton = XmCreateToggleButtonGadget(rc5, "closeTwist",
						 resources, n); n = 0;
    XmToggleButtonSetState( closeTwistButton, 
				curNoodle->twistClosed.getValue(), FALSE );
    XtAddCallback(closeTwistButton, XmNvalueChangedCallback,
		  closeTwistCallback, (XtPointer)interface);
    XtManageChild(closeTwistButton);
    XtManageChild(rc5);

    // Clear Twist button
    XtSetArg(resources[n], XmNlabelString, STRING("Clear Twist")); ++n;
    Widget clearTwistButton = XmCreatePushButtonGadget(rc5, "clearTwist",
						 resources, n); n = 0;
    XtAddCallback(clearTwistButton, XmNactivateCallback,
		  clearTwistCallback, (XtPointer)closeTwistButton);
    XtManageChild(clearTwistButton);
    XtManageChild(rc5);

    // Frame = top Section of Form
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, rc5); n++;
    Widget frame5 = XmCreateFrame(form5, "twistViewFrame", resources,
				  n); n = 0;
    // Put viewer in frame
    twistPlaneViewer = new SoXtPlaneViewer(frame5);
    twistPlaneViewer->setBorder(FALSE);
    twistPlaneViewer->setDecoration(FALSE);
    twistPlaneViewer->setViewing(FALSE);
    twistPlaneViewer->show();
    interface->setTwistViewer( twistPlaneViewer );
    interface->setTwistCloseButton( closeTwistButton );
    XtManageChild(frame5);

    XtManageChild(form5);

    XtManageChild(form);

    examViewer->viewAll();

    // Tell the interface to display info about the current noodle
    // This includes parameters in the pulldowns and also info in the
    // graph viewers.
	interface->setPrimarySelection( curNoodle );
    
    SoXt::show(mainWindow);

    SoXt::mainLoop();
    return 0;
}
#endif /* !SDL_PORT */
