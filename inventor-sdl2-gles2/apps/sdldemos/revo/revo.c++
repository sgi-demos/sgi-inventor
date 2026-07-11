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
// Surface of revolution program
//

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <unistd.h>

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <GL/gl.h>
#ifdef SDL_PORT
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoSeparator.h>
#include <SoSDL.h>
#include <SoSDLRenderArea.h>
#include <SoSDLExaminerViewer.h>
#define SoXt SoSDL
#define SoXtRenderArea SoSDLRenderArea
#define SoXtExaminerViewer SoSDLExaminerViewer
typedef void *Widget;
typedef void *XtPointer;
#else
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/Xt/SoXtClipboard.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoSeparator.h>

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/ScrollBar.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#endif /* SDL_PORT */

#include "../../samples/common/InventorLogo.h"
#include "LineManip.h"
#include "RevClass.h"

//
// These are defined in profile.c++
//
extern SoNode *createProfileGraph(Widget, RevolutionSurface *);
extern void clearPoints();
#ifdef SDL_PORT
extern void profileWindowLeft();
#endif

//
// Callback for 'About...' button
//
void
showAboutDialog(Widget, XtPointer, XtPointer)
{
#ifdef SDL_PORT
    printf("Revo - Open Inventor surface-of-revolution demo (SGI, 1994).\n"
	   "Draw a profile in the left window; the surface of revolution\n"
	   "appears in the right window. See revo.about.pdf in the source\n"
	   "distribution for the original Showcase document.\n");
    return;
#endif
    if (access(IVDEMOBINDIR "/revo.about.pdf", R_OK) != 0)
    {
	system("xmessage 'Sorry, could not find "
	       IVDEMOBINDIR "/revo.about.pdf' > /dev/null");
	return;
    }

    char command[2*PATH_MAX+100];
    sprintf(command, "which " PDFVIEWER " >& /dev/null");

    int err = system(command);
    if (err)
    {
	system("xmessage 'You must install " PDFVIEWER
	       " for this function to work' > /dev/null");
	return;
    }

    sprintf(command, PDFVIEWER " " IVDEMOBINDIR "/revo.about.pdf &");
    system(command);
}	

//
// Called by the quit button
//
static void
quitCallback(Widget, XtPointer, XtPointer)
{
    exit(0);
}

//
// Callback for the text widget (called when the user hits return).
//
#ifdef SDL_PORT
void
changeNumSides(RevolutionSurface *s, int numSides)
{
    if (numSides < 3) numSides = 3;
    if (numSides > 999) numSides = 999;
    printf("# sides: %d\n", numSides);
    s->changeNumSides(numSides + 1);
}
#else
void
changeNumSides(Widget textWidget, XtPointer data, XtPointer)
{
    static char t[4];
    char *str = XmTextGetString((Widget)textWidget);
    int NumSides = atoi(str) + 1;
    if (NumSides < 4) NumSides = 4;

    sprintf(t, "%d", (NumSides-1) % 1000);
    XmTextSetString((Widget)textWidget, t);

    XtFree(str);

    RevolutionSurface *s = (RevolutionSurface *)data;
    s->changeNumSides(NumSides);
}
#endif /* SDL_PORT */

//
// Callback for the copy button.  Copies the surface of revolution to
// the clipboard.
//
#ifdef SDL_PORT
// No X clipboard; "copy" writes the surface scene as Inventor ASCII
// to stdout (visible in the browser console on the web build).
static void
copyCallback(SoXtExaminerViewer *v)
{
    SoWriteAction wa;
    wa.getOutput()->setFilePointer(stdout);
    wa.apply(v->getSceneGraph());
    fflush(stdout);
}
#else
static void
copyCallback(Widget, XtPointer vwr, XtPointer cbstruct)
{
    static SoXtClipboard *theClipboard = NULL;

    SoXtExaminerViewer *v = (SoXtExaminerViewer *)vwr;

    if (theClipboard == NULL)
    {
	theClipboard = new SoXtClipboard(v->getWidget());
    }
    SoNode *root = v->getSceneGraph();
    theClipboard->copy(root,
	((XmAnyCallbackStruct *)cbstruct)->event->xbutton.time);
}
#endif /* SDL_PORT */

static void
clearCallback(Widget, XtPointer, XtPointer)
{
    clearPoints();
}
////////////////////////////////////////////////////////////////////////
//
//  Draw the Inventor logo in the overlays.
//
////////////////////////////////////////////////////////////////////////

static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
#ifdef SDL_PORT
	// The overlay is a second pass over the main viewport here, so
	// the viewport change must be restored (drop precedent).
	static int pushedViewport = 0;
	if (!pushedViewport) {
	    pushedViewport = 1;
	    glPushAttrib(GL_VIEWPORT_BIT);
	    glViewport(0, 0, 80, 80);
	}
	else {
	    pushedViewport = 0;
	    glPopAttrib();
	}
#else
        glViewport(0, 0, 80, 80);  // See Dave Mott for details!
#endif
    }
}
static void
setOverlayLogo(SoXtRenderArea *ra)
{
    static SoSeparator *logo = NULL;
    if (logo == NULL) {
        SoInput in;
        in.setBuffer((void *)ivLogo, ivLogoSize);    // common directory
        logo = SoDB::readAll(&in);
        logo->ref();

        SoCallback *cb = new SoCallback; // set the GL viewport
        cb->setCallback(logoCB);
        logo->insertChild(cb, 0);
#ifdef SDL_PORT
        logo->addChild(cb);	// second visit pops the viewport
#endif
    }
    SbColor col(1, 1, 1);
    ra->setOverlayColorMap(1, 1, &col);
    ra->setOverlaySceneGraph(logo);
}


#ifdef SDL_PORT
//
// Keyboard replaces the Motif button strip: the original had a
// "# sides" text field and Copy / Clear / About / Quit buttons.
//
static RevolutionSurface *theSurface = NULL;
static SoSDLExaminerViewer *theViewer = NULL;
static int theNumSides = 20;

static void
printHelp()
{
    printf("Revo keys (either window):\n"
	   "  left window: click to add profile points, drag to move them\n"
	   "  +/-  more/fewer sides (# sides: %d)\n"
	   "  c    clear the profile\n"
	   "  w    write surface scene as .iv to stdout (was: Copy)\n"
	   "  a    about\n"
	   "  h    this help\n"
	   "  q    quit\n", theNumSides);
}

static bool
handleRevoKey(SDL_Keycode key)
{
    switch (key) {
      case SDLK_c:
	clearPoints();
	return true;
      case SDLK_EQUALS: case SDLK_PLUS: case SDLK_KP_PLUS:
	changeNumSides(theSurface, ++theNumSides);
	return true;
      case SDLK_MINUS: case SDLK_KP_MINUS:
	if (theNumSides > 3)
	    changeNumSides(theSurface, --theNumSides);
	return true;
      case SDLK_w:
	copyCallback(theViewer);
	return true;
      case SDLK_a:
	showAboutDialog(NULL, NULL, NULL);
	return true;
      case SDLK_h:
	printHelp();
	return true;
      case SDLK_q:
	SoSDL::exitMainLoop();
	return true;
    }
    return false;
}

class RevoViewer : public SoSDLExaminerViewer {
  public:
    RevoViewer(const char *t, int w, int h) : SoSDLExaminerViewer(t, w, h) {}
    virtual void processEvent(const SDL_Event *e) {
	if (e->type == SDL_KEYDOWN && handleRevoKey(e->key.keysym.sym))
	    return;
	SoSDLExaminerViewer::processEvent(e);
    }
};

class ProfileArea : public SoSDLRenderArea {
  public:
    ProfileArea(const char *t, int w, int h) : SoSDLRenderArea(t, w, h) {}
    virtual void processEvent(const SDL_Event *e) {
	if (e->type == SDL_KEYDOWN && handleRevoKey(e->key.keysym.sym))
	    return;
	if (e->type == SDL_WINDOWEVENT &&
	    e->window.event == SDL_WINDOWEVENT_LEAVE)
	    profileWindowLeft();	// turn off manip hilights
	SoSDLRenderArea::processEvent(e);
    }
};
#endif /* SDL_PORT */

////////////////////////////////////////////////////////////////////////
//
//  main
//
////////////////////////////////////////////////////////////////////////

int
main(int, char **argv)
{
#ifdef SDL_PORT
    (void)argv;
    SoSDL::init();
    LineManip2::initClass();

    RevolutionSurface *revSurf = new RevolutionSurface();
    theSurface = revSurf;

    // The original was one 800x400 form: profile editor on the left
    // half, examiner viewer on the right, buttons along the bottom.
    // Here that is two 400x400 windows and keyboard shortcuts.
    ProfileArea *profile = new ProfileArea("Revo Profile", 400, 400);
    SoSDLExaminerViewer *examViewer = new RevoViewer("Revo", 400, 400);
    theViewer = examViewer;

    examViewer->setSceneGraph(revSurf->getSceneGraph());
    setOverlayLogo(examViewer);

    profile->setSceneGraph(createProfileGraph(NULL, revSurf));

    examViewer->viewAll();
    printHelp();
    SoSDL::mainLoop();
    return 0;
#else
    Widget w = SoXt::init(argv[0]);
    if (w == NULL) exit(1);
    
    LineManip2::initClass();
    
    RevolutionSurface *revSurf = new RevolutionSurface();

    Arg resources[20];	
    int n = 0;
    XtSetArg(resources[n], "width", 800); n++;
    XtSetArg(resources[n], "height", 400); n++;
    Widget form = XmCreateForm(w, "form", resources, n); n = 0;
    
    //
    // This RowColumn widget holds all the stuff at the bottom
    // of the screen
    //
    XtSetArg(resources[n], XmNorientation, XmHORIZONTAL); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNadjustLast, FALSE); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget rc = XmCreateRowColumn(form, "bottomStuff", resources, n); 
    n = 0;
    
    //
    // Add the viewer to view the shaded geometry
    // 
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNleftPosition, 50); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, rc); n++;
    Widget frame2 = XmCreateFrame(form, "renderAreaFrame", resources, n); 
    n = 0;

    SoXtExaminerViewer *examViewer = new SoXtExaminerViewer(frame2);
    examViewer->setSceneGraph(revSurf->getSceneGraph());
    examViewer->setBorder(FALSE);
    examViewer->setDecoration(FALSE);
    setOverlayLogo( examViewer );    // Add Inventor logo to overlays
    examViewer->show();
    XtManageChild(frame2);
    
    //
    // Add viewer to edit the revolution profile
    //
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNrightPosition, 50); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, rc); n++;
    Widget frame1 = XmCreateFrame(form, "renderAreaFrame", resources, n); 
    n = 0;

    SoXtRenderArea *ra = new SoXtRenderArea(frame1);
    ra->setSceneGraph(createProfileGraph(ra->getWidget(), revSurf));
    ra->setBorder(FALSE);
    ra->show();
    XtManageChild(frame1);

#define STRING(a) XmStringCreate(a,XmSTRING_DEFAULT_CHARSET)
    
    XtSetArg(resources[n], XmNlabelString, STRING("# sides:")); ++n;
    Widget nsidesLabel = XmCreateLabelGadget(
		rc, "nsidesLabel", resources, n); 
    n = 0;
    XtManageChild(nsidesLabel);
    
    XtSetArg(resources[n], XmNvalue, "20"); ++n;
    XtSetArg(resources[n], XmNcolumns, 3); ++n;
    XtSetArg(resources[n], XmNeditMode, XmSINGLE_LINE_EDIT); ++n;
    Widget nsidesEdit = XmCreateText(
		rc, "nsidesEdit", resources, n); 
    n = 0;
    XtAddCallback(nsidesEdit, XmNactivateCallback, 
		changeNumSides, (XtPointer)revSurf);
    XtManageChild(nsidesEdit);
    
    XtSetArg(resources[n], XmNlabelString, STRING("Copy")); ++n;
    Widget copyButton = XmCreatePushButtonGadget(
		rc, "copy", resources, n); 
    n = 0;
    XtAddCallback(copyButton, XmNactivateCallback,
		  copyCallback, (XtPointer)examViewer);
    XtManageChild(copyButton);
    
    XtSetArg(resources[n], XmNlabelString, STRING("Clear")); ++n;
    Widget clearButton = XmCreatePushButtonGadget(
		rc, "clear", resources, n); 
    n = 0;
    XtAddCallback(clearButton, XmNactivateCallback,
		  clearCallback, (XtPointer)examViewer);
    XtManageChild(clearButton);

    XtSetArg(resources[n], XmNlabelString, STRING("About...")); ++n;
    Widget aboutButton = XmCreatePushButtonGadget(
		rc, "about", resources, n); 
    n = 0;
    XtAddCallback(aboutButton, XmNactivateCallback,
		  showAboutDialog, NULL);
    XtManageChild(aboutButton);

    XtSetArg(resources[n], XmNlabelString, STRING("Quit")); ++n;
    Widget quitButton = XmCreatePushButtonGadget(
		rc, "quit", resources, n); 
    n = 0;
    XtAddCallback(quitButton, XmNactivateCallback,
		  quitCallback, NULL);
    XtManageChild(quitButton);
    
    XtManageChild(rc);
    XtManageChild(form);

    examViewer->viewAll();
    SoXt::show(w);
    SoXt::mainLoop();

    return 0;  // Keep C++ from complaining...
#endif /* SDL_PORT */
}
