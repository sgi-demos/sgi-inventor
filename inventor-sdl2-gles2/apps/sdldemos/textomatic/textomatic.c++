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
// Main stuff for 3D text creator program
//
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <iconv.h>
#ifdef __APPLE__
#include <libcharset.h>
#else
#include <nl_types.h>
#include <langinfo.h>
#endif
#ifdef SDL_PORT
#include <Inventor/SoDB.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <GL/gl.h>
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
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#endif /* SDL_PORT */
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLinearProfile.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoProfileCoordinate2.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/sensors/SoSensor.h>

#ifndef SDL_PORT
#include <Xm/XmAll.h>
#endif
#include <locale.h>

#include "../../samples/common/InventorLogo.h"
#include "../../samples/common/LineManip.h"
#include "./TextGraph.h"
#include "../../samples/common/Useful.h"
#include "./labels.h"

//
// Some evil variables global to this file.  I should pass them around
// as paramaters or encapsulate them in a class, but this is easier.
// Especially since so much occurs in response to Xt callbacks, and
// passing arguments to a callback function means wrapping them all up
// in a structure and passing a pointer to that structure (which is a
// pain!).
//
static const char *DefaultString =
        "Open Inventor -\n3D Programming\nFor Humans";
static LineManip2 *manip;

// utf8-ucs2
iconv_t global_iconvCodeL2;
iconv_t global_iconvCode28;

// Default resources...
#ifndef SDL_PORT
XtResource resintl[] = {
  { "copyLabel",       "CopyLabel",       XtRString,  sizeof(String),
    XtOffsetOf(Labels,copy),              XtRString, (XtPointer) "Copy"
  },
  { "edit parts",      "Edit Parts",      XtRString,  sizeof(String),
    XtOffsetOf(Labels,editParts),         XtRString, (XtPointer) "Edit Parts"
  },
  { "aboutLabel",      "AboutLabel",      XtRString,  sizeof(String),
    XtOffsetOf(Labels,about),             XtRString, (XtPointer) "About..."
  },
  { "quitLabel",       "QuitLabel",       XtRString,  sizeof(String),
    XtOffsetOf(Labels,quit),              XtRString, (XtPointer) "Quit"
  },
  { "soFontList",      "SoFontList",      XtRString,  sizeof(String),
    XtOffsetOf(Labels,sofontlist),        XtRString, (XtPointer) "Times-Roman"
  },
  { "front",           "Front",           XtRString,  sizeof(String),
    XtOffsetOf(Labels,front),             XtRString, (XtPointer) "Front:"
  },
  { "sides",           "Sides",           XtRString,  sizeof(String),
    XtOffsetOf(Labels,sides),             XtRString, (XtPointer) "Sides:"
  },
  { "back",            "Back",            XtRString,  sizeof(String),
    XtOffsetOf(Labels,back),              XtRString, (XtPointer) "Back:"
  },
  { "on",              "On",              XtRString,  sizeof(String),
    XtOffsetOf(Labels,on),                XtRString, (XtPointer) "On"
  },
  { "edit",            "Edit",            XtRString,  sizeof(String),
    XtOffsetOf(Labels,edit),              XtRString, (XtPointer) "Edit"
  }
};
int num_resintl = XtNumber( resintl );
#endif /* !SDL_PORT */



//
// This is called whenever the profile's coordinates change.  It has
// to change the profileCoordinate node which defines the text's
// extrusion.
//
static void
profileCallback(void *data, SoSensor *)
{
    SoCoordinate3 *coord = (SoCoordinate3 *)data;

    int n = coord->point.getNum();

    if (n == 0) return;

    // Update the profile node
    const SbVec3f *v = coord->point.getValues(0);

    SbVec2f *newV = new SbVec2f[n];
    for (int i = 0; i < n; i++) {
	newV[i].setValue(coord->point[i][0], coord->point[i][1]);
	profile->index.set1Value(i, i);
    }
    profileCoords->point.setValues(0, n, newV);
    profileCoords->point.deleteValues(n);  // Get rid of any extra
    profile->index.deleteValues(n);

    delete[] newV;
}
//
// Callback for 'About...' button
//
void
showAboutDialog(Widget, XtPointer, XtPointer)
{
#ifdef SDL_PORT
    printf("Textomatic - Open Inventor 3D text editor (SGI, 1994).\n"
	   "Type to edit the text; draw a bevel profile in the small\n"
	   "window. See textomatic.about.pdf in the source distribution\n"
	   "for the original Showcase document.\n");
    return;
#endif
    if (access(IVDEMOBINDIR"/textomatic.about.pdf", R_OK) != 0)
    {
	system("xmessage 'Sorry, could not find "
	       IVDEMOBINDIR"/textomatic.about.pdf' > /dev/null");
	return;
    }

    char command[2*PATH_MAX+100];
    sprintf(command, "which " PDFVIEWER " > /dev/null");

    int err = system(command);
    if (err) {
	system("xmessage 'You must install " PDFVIEWER
	       " for this function to work' > /dev/null");
	return;
    }

    sprintf(command, PDFVIEWER " " IVDEMOBINDIR"/textomatic.about.pdf &");
    system(command);
}	

//
// Called by the quit button
//
static void
quitCallback(Widget, XtPointer, XtPointer)
{
    iconv_close( global_iconvCodeL2 );
    iconv_close( global_iconvCode28 );
    exit(0);
}

//
// These two are used to turn off/on render caching as the cursor
// enters/leaves the profile window.  Caching while editing the
// profile is stupid, since the cache is always wrong (the geometry
// changes).
//
#ifdef SDL_PORT
void
profileWindowEntered()
{
    setTextCaching(FALSE);
}
void
profileWindowLeft()
{
    setTextCaching(TRUE);
    if (manip)
	manip->removeHilights();
}
#else
void
enterProfileWindow(Widget, XtPointer, XEvent *, Boolean *cont)
{
    setTextCaching(FALSE);

    *cont = TRUE;	// Continue dispatching this event
}
void
leaveProfileWindow(Widget, XtPointer, XEvent *, Boolean *cont)
{
    setTextCaching(TRUE);
    manip->removeHilights();

    *cont = TRUE;	// Continue dispatching this event
}
#endif /* SDL_PORT */

//
// The default profile
//
static float
defaultCoords[2][3] =
{
    { -0.2, 0.0, 0.0 },
    {  0.2, 0.0, 0.0 },
};
//
// Grid for window:
//
static char *GridString =
"#Inventor V2.0 ascii\n"
"Separator {"
"	PickStyle { style UNPICKABLE }"
"	LightModel { model BASE_COLOR }"
"	BaseColor { rgb 0.2 0.2 0.2 }"
"	Array {"
"		numElements1 7"
"		separation1 .1 0 0"
"		origin CENTER"
"		Coordinate3 { point [ 0 -1 0, 0 1 0 ] }"
"		LineSet { numVertices [ 2 ] }"
"	}"
"	Array {"
"		numElements1 7"
"		separation1 0 .1 0"
"		origin CENTER"
"		Coordinate3 { point [ -1 0 0, 1 0 0 ] }"
"		LineSet { numVertices [ 2 ] }"
"	}"
"	BaseColor { rgb 0.4 0.0 0.0 }"
"	Coordinate3 { point [ -1.0 0.0 0.0, 1.0 0.0 0.0,"
"			0.0 -1.0 0.0, 0.0 1.0 0.0 ]"
"	}"
"	LineSet { numVertices [ 2, 2 ] }"
"	BaseColor { rgb 0.3 0.3 0.3 }"
"	Transform {"
"		scaleFactor 0.025 0.025 0.025"
"		translation 0.0 -0.06 0.0"
"	}"
"	Font {"
"		size 2"
"		name \"Helvetica\""
"	}"
"	Array {"
"		numElements1 2"
"		separation1 20 0 0"
"		origin CENTER"
"		DEF switch Switch {"
"			whichChild -2"
"		Text3 { string \"Front\" justification LEFT parts FRONT }"
"		Text3 { string \"Back\" justification RIGHT parts FRONT }"
"		}"
"	}"
"}";

//
// Routine that creates the scene graph in the render area at the
// bottom of the window (where the text extrusion profile is defined).
//
SoSeparator *
createProfileSceneGraph()
{
    SoSeparator *root = new SoSeparator;
    root->ref();

    SoOrthographicCamera *c = new SoOrthographicCamera;
    c->nearDistance = 0.5;
    c->height = 0.4;
    c->aspectRatio = 4.0/3.0;
    root->addChild(c);

    SoLightModel *lm = new SoLightModel;
    lm->model = SoLightModel::BASE_COLOR;
    root->addChild(lm);

    // Axes and labels group
    SoInput in;
    in.setBuffer(GridString, strlen(GridString));
    SoNode *node;	
    SoDB::read(&in, node);
    SoSeparator *g1 = (SoSeparator *)node;
    if (g1 != NULL)
    {
	root->addChild(g1);
        g1->renderCaching = SoSeparator::ON;
        g1->boundingBoxCaching = SoSeparator::ON;
    }

    // Line manipulator group

    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue(0.0, 0.8, 0.8);
    root->addChild(m);

    manip = new LineManip2;
    manip->setHilightSize(0.01);
    root->addChild(manip);

    SoCoordinate3 *coord = manip->getCoordinate3();
    SoNodeSensor *d = new SoNodeSensor(profileCallback, coord);
    d->attach(coord);
    coord->point.setValues(0, 2, defaultCoords);

    // Also initialize the profile to be the same in the main scene
    SbVec2f *defaultProfile = new SbVec2f[2];
    for (int i = 0; i < 2; i++) {
        defaultProfile[i].setValue(defaultCoords[i][0], defaultCoords[i][1]);
        profile->index.set1Value(i, i);
    }
    profileCoords->point.setValues(0, 2, defaultProfile);
    delete[] defaultProfile;

    root->unrefNoDelete();

    return root;
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
        glViewport(0, 0, 80, 80);    // See Dave Mott for details!
#endif
    }
}
static void
setOverlayLogo(SoXtRenderArea *ra)
{
    static SoSeparator *logo = NULL;
    if (logo == NULL) {
        SoInput in;
        in.setBuffer((void *)ivLogo, ivLogoSize);
        logo = SoDB::readAll(&in);
        logo->ref();
        SoCallback *cb = new SoCallback; // sets GL viewport
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


#ifndef SDL_PORT
static String _myXtDefaultLanguageProc(Display *, String xnl, XtPointer)
{
    if (! setlocale(LC_ALL, xnl))
        XtWarning("locale not supported by C library, locale unchanged");
    
    if (! XSupportsLocale()) {
        XtWarning("locale not supported by Xlib, locale set to C");
        setlocale(LC_ALL, "C");
    }
    if (! XSetLocaleModifiers(""))
        XtWarning("X locale modifiers not supported, using default");

    setlocale(LC_NUMERIC, "C");
    
    return setlocale(LC_CTYPE, NULL);
}
#endif /* !SDL_PORT */

//////////////////////////////////////////////////////////////////////
//  -------------------
//  |                 |
//  |                 |
//  |      Main       |
//  |                 |
//  |                 |
//  |                 |
//  -------------------  <- MainHeight
//  |But-|Bevel  |Text|
//  | ton|       |    |
//  -------------------
//       ^       ^
//       Vert1   Vert2
//
////////////////////////////////////////////////////////////////////

// constants that define the UI layout
const int MainHeight = 79;
const int Vert1 = 20;
const int Vert2 = 60;
Labels labels;        // Global variable.

#ifdef SDL_PORT
//
// The Motif text widget, buttons, and part-editor dialog become:
// type in the main window to edit the text; keyboard shortcuts with
// Ctrl for everything else (plain letters go into the text).
//
static char textBuf[4096];

static void
printHelp()
{
    printf("Textomatic keys (main window):\n"
	   "  type to edit the 3D text (Enter = new line,\n"
	   "  Backspace deletes; updates after a 1s pause)\n"
	   "  Ctrl+1/2/3  toggle front / sides / back parts\n"
	   "  Ctrl+C      copy text scene as .iv to stdout\n"
	   "  Ctrl+A      about\n"
	   "  Ctrl+H      this help\n"
	   "  Ctrl+Q      quit\n"
	   "Profile window: click to add bevel points, drag to move,\n"
	   "  Backspace deletes the last point.\n");
}

class TextoViewer : public SoSDLExaminerViewer {
  public:
    TextoViewer(const char *t, int w, int h) : SoSDLExaminerViewer(t, w, h) {
	SDL_StartTextInput();
    }
    virtual void processEvent(const SDL_Event *e) {
	if (e->type == SDL_TEXTINPUT) {
	    if (strlen(textBuf) + strlen(e->text.text) < sizeof(textBuf)-1) {
		strcat(textBuf, e->text.text);
		delayUpdateTextString(textBuf);
	    }
	    return;
	}
	if (e->type == SDL_KEYDOWN) {
	    SDL_Keycode k = e->key.keysym.sym;
	    if (e->key.keysym.mod & KMOD_CTRL) {
		switch (k) {
		  case SDLK_1: togglePart3(SoText3::FRONT); return;
		  case SDLK_2: togglePart3(SoText3::SIDES); return;
		  case SDLK_3: togglePart3(SoText3::BACK); return;
		  case SDLK_c: copyTextToStdout(); return;
		  case SDLK_a: showAboutDialog(NULL, NULL, NULL); return;
		  case SDLK_h: printHelp(); return;
		  case SDLK_q: SoSDL::exitMainLoop(); return;
		}
	    }
	    else if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
		if (strlen(textBuf) < sizeof(textBuf)-2) {
		    strcat(textBuf, "\n");
		    delayUpdateTextString(textBuf);
		}
		return;
	    }
	    else if (k == SDLK_BACKSPACE) {
		size_t n = strlen(textBuf);
		if (n > 0) {
		    // Chop one UTF-8 character (skip continuation bytes).
		    do { n--; } while (n > 0 &&
				       (textBuf[n] & 0xC0) == 0x80);
		    textBuf[n] = 0;
		    delayUpdateTextString(textBuf);
		}
		return;
	    }
	}
	SoSDLExaminerViewer::processEvent(e);
    }
};

class BevelArea : public SoSDLRenderArea {
  public:
    BevelArea(const char *t, int w, int h) : SoSDLRenderArea(t, w, h) {}
    virtual void processEvent(const SDL_Event *e) {
	if (e->type == SDL_WINDOWEVENT) {
	    if (e->window.event == SDL_WINDOWEVENT_ENTER)
		profileWindowEntered();
	    else if (e->window.event == SDL_WINDOWEVENT_LEAVE)
		profileWindowLeft();
	}
	SoSDLRenderArea::processEvent(e);
    }
};

int
main(int, char **)
{
    SoSDL::init();

#ifdef __APPLE__
    const char *nl_encord = locale_charset();
#elif defined(__EMSCRIPTEN__)
    const char *nl_encord = "UTF-8";
#else
    const char *nl_encord = (const char *) nl_langinfo( CODESET );
#endif
    if ( (global_iconvCodeL2 = iconv_open( "UCS-2", nl_encord ))==(iconv_t)-1 ) {
	   fprintf( stderr, "textomatic: iconv_open error.\n" );
    }
    if ( (global_iconvCode28 = iconv_open( "UTF-8", "UCS-2" ))==(iconv_t)-1 ) {
	   fprintf( stderr, "textomatic: iconv_open error.\n" );
    }

    // The Xt resource strings, filled directly.
    labels.copy      = (char *)"Copy";
    labels.editParts = (char *)"Edit Parts";
    labels.about     = (char *)"About...";
    labels.quit      = (char *)"Quit";
    labels.sofontlist= (char *)"Times-Roman";
    labels.front     = (char *)"Front:";
    labels.sides     = (char *)"Sides:";
    labels.back      = (char *)"Back:";
    labels.on        = (char *)"On";
    labels.edit      = (char *)"Edit";

    // The bundled Times-Roman lives in fonts/ beside the binary.
    setenv("FL_FONT_PATH", "fonts", 0);

    LineManip2::initClass();

    // Bevel window first: on the web the single canvas belongs to the
    // most recently created window, and the text view is the one worth
    // seeing there.
    BevelArea *profileArea = new BevelArea("Textomatic Bevel", 320, 220);

    TextoViewer *examViewer = new TextoViewer("Textomatic", 620, 490);
    examViewer->setSceneGraph(createTextSceneGraph());
    setOverlayLogo( examViewer );    // Add Inventor logo to overlays

    profileArea->setSceneGraph(createProfileSceneGraph());

    strncpy(textBuf, DefaultString, sizeof(textBuf)-1);
    updateTextString(textBuf);

    printHelp();
    SoSDL::mainLoop();
    return 0;
}
#else /* !SDL_PORT */

int
main(int argc, char **argv)
{
    XtSetLanguageProc( NULL, _myXtDefaultLanguageProc, NULL );

    Widget w = SoXt::init(argv[0],"Textomatic");
    if (w == NULL) exit(1);

#ifdef __APPLE__
    const char *nl_encord = locale_charset();
#else
    const char *nl_encord = (const char *) nl_langinfo( CODESET );
#endif
    if ( (global_iconvCodeL2 = iconv_open( "UCS-2", nl_encord ))==(iconv_t)-1 ) {
           fprintf( stderr, "textomatic: iconv_open error.\n" );
    }
    if ( (global_iconvCode28 = iconv_open( "UTF-8", "UCS-2" ))==(iconv_t)-1 ) {
           fprintf( stderr, "textomatic: iconv_open error.\n" );
    }

    XtGetApplicationResources( SoXt::getTopLevelWidget(), &labels, 
                               resintl, num_resintl, NULL, 0 );

    LineManip2::initClass();

    Arg resources[20];

    int n = 0;
    XtSetArg(resources[n], "width", 620); n++;
    XtSetArg(resources[n], "height", 620); n++;
    Widget form = XmCreateForm(w, "form", resources, n); n = 0;

    //
    // There are two 'tab groups' because keyboard focus must go to
    // either the text type-in, or to the profile render area (which
    // will respond to backspace-key events by deleting the last point
    // added).
    //
    XmAddTabGroup(form);

    SoXtExaminerViewer *examViewer = new SoXtExaminerViewer(form);
    examViewer->setSceneGraph(createTextSceneGraph());
    examViewer->setDecoration(FALSE);
    examViewer->setBorder(TRUE);
    examViewer->setFeedbackVisibility(FALSE);
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNbottomPosition, MainHeight); n++;
    XtSetValues(examViewer->getWidget(), resources, n); n = 0;
    setOverlayLogo( examViewer );    // Add Inventor logo to overlays
    examViewer->show();

    SoXtRenderArea *profile = new SoXtRenderArea(form);
    profile->setSceneGraph(createProfileSceneGraph());
    profile->setBorder(TRUE);
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNtopPosition, MainHeight); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNleftPosition, Vert1); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNrightPosition, Vert2); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNtraversalOn, TRUE); n++;
    XtSetValues(profile->getWidget(), resources, n); n = 0;
    XtAddEventHandler(profile->getWidget(), EnterWindowMask,
		      FALSE, enterProfileWindow, NULL);
    XtAddEventHandler(profile->getWidget(), LeaveWindowMask,
		      FALSE, leaveProfileWindow, NULL);
    profile->show();

    XtSetArg(resources[n], XmNvalue, DefaultString); ++n;
    XtSetArg(resources[n], XmNeditMode, XmMULTI_LINE_EDIT); ++n;
    XtSetArg(resources[n], XmNwordWrap, TRUE); ++n;
    XtSetArg(resources[n], XmNscrollHorizontal, FALSE); ++n;
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_POSITION); ++n;
    XtSetArg(resources[n], XmNtopPosition, MainHeight); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); ++n;
    XtSetArg(resources[n], XmNleftPosition, Vert2); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); ++n;
    Widget textEdit = XmCreateScrolledText(form, "textEdit",
			    resources, n); n = 0;
    XtAddCallback(textEdit, XmNactivateCallback,
		  updateText, (XtPointer)textEdit);
    XtAddCallback(textEdit, XmNvalueChangedCallback,
		  delayUpdateText, (XtPointer)textEdit);
    XtManageChild(textEdit);
    updateText(textEdit, (XtPointer)textEdit, NULL);

    XtSetArg(resources[n], XmNnumColumns, 1); ++n;
    XtSetArg(resources[n], XmNorientation, XmVERTICAL); ++n;
    XtSetArg(resources[n], XmNpacking, XmPACK_COLUMN); ++n;
    XtSetArg(resources[n], XmNadjustLast, FALSE); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); ++n;
    XtSetArg(resources[n], XmNrightPosition, Vert1); ++n;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNbottomOffset, 2); ++n;
    XtSetArg(resources[n], XmNspacing, 0); ++n;
    XtSetArg(resources[n], XmNmarginHeight, 0); ++n;
    XtSetArg(resources[n], XmNmarginWidth, 17); ++n;
    Widget rc = XmCreateRowColumn(form, "buttons", resources, n);
    n = 0;

#define STRING(a) XmStringCreateLocalized(a)

    XtSetArg(resources[n], XmNlabelString, STRING(labels.copy)); ++n;
    Widget copyButton = XmCreatePushButtonGadget(rc, "copyButton",
					   resources, n); n = 0;
    XtAddCallback(copyButton, XmNactivateCallback,
		  copyText, (XtPointer)rc);
    XtManageChild(copyButton);

    XtSetArg(resources[n], XmNlabelString, STRING(labels.editParts)); ++n;
    Widget editButton = XmCreatePushButtonGadget(rc, "editButton",
					   resources, n); n = 0;
    XtAddCallback(editButton, XmNactivateCallback,
		  createPartEditor, (XtPointer)rc);
    XtManageChild(editButton);

    XtSetArg(resources[n], XmNlabelString, STRING(labels.about)); ++n;
    Widget aboutButton = XmCreatePushButtonGadget(rc, "about",
						  resources, n); n = 0;
    XtAddCallback(aboutButton, XmNactivateCallback,
		  showAboutDialog, NULL);
    XtManageChild(aboutButton);

    XtSetArg(resources[n], XmNlabelString, STRING(labels.quit)); ++n;
    Widget quitButton = XmCreatePushButtonGadget(rc, "quitButton",
					   resources, n); n = 0;
    XtAddCallback(quitButton, XmNactivateCallback,
		  quitCallback, NULL);
    XtManageChild(quitButton);

    XtManageChild(rc);

    XtManageChild(form);

    SoXt::show(w);
    SoXt::mainLoop();
    return 0;
}
#endif /* !SDL_PORT */
