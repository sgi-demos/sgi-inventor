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
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <Xm/MessageB.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtPlaneViewer.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/sensors/SoTimerSensor.h>

#include "InventorLogo.h"
#include "ButtonStrip.h"
#include "linkatronMenus.h"
#include "GraphPaper.h"
#include "Shadow.h"
#include "TableTop.h"
#include "MechPiece.h"
#include "Axle.h"
#include "WorldInfo.h"
#include "Sound.h"

// New classes for linkages.
#include "LinkEngines.h"
#include "LinkClasses.h"
#include "Buttons.h"

#include "Motors.h"

typedef struct {
    int    count;
    char **names;
} FileInfo;

// Function prototypes 
    // Various:
    static void print_usage(const char *);
    static void parse_args(int , char **);
    static SoSeparator *readScene(FileInfo);
    static void readResources();
    static void setOverlayLogo(SoXtRenderArea *);
    static void logoCB(void *, SoAction *);
    static void overlayViewportCB(void *, SoAction *);

    // Functions relating to the FILE menu.
    static void showHelpDialog();
    static void fileNewEvent();
    static void newSceneCB(Widget, void *, void *);
    static void fileOpenEvent();
    static void openSceneCB(Widget, void *, void *);
    static void readFromFileCB(Widget, void *,
			       XmFileSelectionBoxCallbackStruct *);
    static void fileSaveEvent();
    static void fileSaveAsEvent();
    static void writeToFirstFile();
    static void writeToFileCB(Widget, void *,
			       XmFileSelectionBoxCallbackStruct *);
    static void fileQuitEvent();
    static void quitProgramCB(Widget, void *, void *);

    // For our special kind of file selector and dialog box:
    static void showFileSelectionDialog( XtCallbackProc );
    static void createOkayCancelDialog(Widget,XtCallbackProc,
				       char *,char *,char *);
    static void destroyDialogCB(Widget dialog, void *, void *);

    // For creating layout and processing top bar
    static void processTopbarEvent(Widget, mmMenuItem *, XmAnyCallbackStruct *);
    static Widget buildAndLayoutMenus(Widget );
    static ButtonStrip    *buildAndLayoutButtonStrip(Widget , Widget );
    static SoXtExaminerViewer *buildAndLayoutViewer(Widget , Widget );

    // Callbacks for buttons in the button strip.
    static void newRigidStrutCB( void *, SoDragger *);
    static void newDoubleStrutCB( void *, SoDragger *);
    static void newWheelCB( void *, SoDragger *);
    static void newArrowCB( void *, SoDragger *);
    static void newDoubleArrowCB( void *, SoDragger *);
    static void newBellCB( void *, SoDragger *);
    static void newRotorMotorCB( void *, SoDragger *);

// Global variables 
static ButtonStrip    *buttonStrip; 
static SoXtExaminerViewer *viewer; 
static Widget mgrWidget;
static FileInfo files;
static WorldInfo *theWorld;
static SbBool    withSound = TRUE;

//////////////////////////////////////////////////////////////////////
//
// Print the help message
//
static void
print_usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [ -hq] [ infiles ]\n", progname);
    fprintf(stderr,
	    "\t-h       Help (Print this message)\n"
	    "\t-q       Quiet mode (Disable audio effects)\n"
	    "\tinfiles  Inventor Files to read into Scene\n"
	    );
    exit(0);
}

/////////////////////////////////////////////////////////////////////
//
// Parse the command line arguments
//
static void
parse_args(int argc, char **argv)
{
    int err = FALSE;	// Flag: error in options?
    int c;
    

    while ((c = getopt(argc, argv, "hq")) != -1) {
	switch(c) {
	  case 'h':	// Help
	    err = TRUE;
	    break;
	  case 'q':	// Quiet mode
	    withSound = FALSE;
	    break;
	  default:
	    err = TRUE;
	    break;
	}
    }
    if (err)
	print_usage(argv[0]);

    /* Handle multiple filenames */
    files.count = argc - optind;
    if (files.count > 0) 
        files.names = (char **) malloc (files.count * sizeof(char *));
    else 
        files.names = (char **) malloc (1 * sizeof(char *));

    for (int i=0; i<files.count; i++, optind++) 
	files.names[i] = argv[optind];

}


/////////////////////////////////////////////////////////////////////
//
// Read the scene from the file(s) 
// If there are multiple files, combine their contents under one separator.
//
static SoSeparator *
readScene(FileInfo files)
{
    SoInput in;
    SoSeparator *root;
    FILE *filePtr = NULL;
    char cmd[250];
    
    if (files.count == 0) 
	return (NULL);

    fprintf(stderr, "Reading file(s)...\n");

    const char *slashPtr;
    char *searchPath = NULL;

    if (files.count == 1) {
	filePtr = fopen(files.names[0], "r");
	if (filePtr == NULL) {
	    fprintf(stderr, "Error opening file %s\n", files.names[0]);
	    return (NULL);
	}

	//
	// If the filename includes a directory path, add the 
	// directory name to the list of directories where to look
	// for files referenced in the input file (e.g. SoFile nodes)
	//
	if ((slashPtr = strrchr(files.names[0], '/')) != NULL) {
            searchPath = strdup(files.names[0]);
            searchPath[slashPtr - files.names[0]] = '\0';
            in.addDirectoryFirst(searchPath);
        }

	in.setFilePointer(filePtr);
	root = SoDB::readAll(&in);
	if (root == NULL) 
	    fprintf(stderr, "Error reading file %s\n", files.names[0]);

	fclose(filePtr);
    }

    else {  	// Multiple input files
        SoNode *inputRoot;
	root = new SoSeparator;
	root->ref();

	//
	// Combine all the files under one separator
	//
        for (int i=0; i<files.count; i++) {
	    filePtr = fopen(files.names[i], "r");
	    if (filePtr == NULL) {
	        fprintf(stderr, "Error opening file %s\n", files.names[i]);
	        return (NULL);
	    }
            in.setFilePointer(filePtr);

	    //
	    // If the filename includes a directory path, add the 
	    // directory name to the list of directories where to look
	    // for files referenced in the input file (e.g. SoFile nodes)
	    //
	    if ((slashPtr = strrchr(files.names[i], '/')) != NULL) {
                searchPath = strdup(files.names[i]);
                searchPath[slashPtr - files.names[i]] = '\0';
                in.addDirectoryFirst(searchPath);
            }

  	    do {
	        if (! SoDB::read(&in, inputRoot)) {
	            fprintf(stderr, "Error reading file %s\n", files.names[i]);
		    root->unref();
		    return (NULL);
	        }
	        else if (inputRoot != NULL)
	            root->addChild(inputRoot);

	    } while (inputRoot != NULL);

	    fclose(filePtr);

        }
    }
   
    return (root);
}

//
// This just reads in the file and doesn't hold on to what it reads.
// Anything with a DEF label will get installed in the global dictionary
// and saved as a resource, however
//
static void
readResources()
{
    SoInput	in;

    if ( in.openFile( "linkagesResourceFile.iv", TRUE )) {

	SoSeparator *root = SoDB::readAll( &in );
	if (root == NULL) {
	    fprintf(stderr,"ERROR in readResources \n");
	    fprintf(stderr,"      Result of reading resources is NULL\n");
	}
	else
	    root->ref();

    }
    else {
	fprintf(stderr,"ERROR in readResources \n");
	fprintf(stderr,"      Can't read resources since the file linkageResourceFile.iv was not found\n");
    }
}


////////////////////////////////////////////////////////////////////
//
// Set the Inventor logo on the screen
//
static void
setOverlayLogo(SoXtRenderArea *ra)
{
    SoInput in;
    in.setBuffer((void *)ivLogo, ivLogoSize);
    SoSeparator *logo = SoDB::readAll(&in);
    logo->ref();

    // Add a callback node which will set the viewport
    SoCallback *cb = new SoCallback;
    cb->setCallback(logoCB);
    logo->insertChild(cb, 0);

    SbColor col(1, 1, 1);
    ra->setOverlayColorMap(1, 1, &col);
    ra->setOverlaySceneGraph(logo);
}

static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
        glViewport(0, 0, 80, 80);
    }
}

static void
overlayViewportCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) 
        glViewport(0, 0, 40, 40);
}


///////////////////////////////////////////////////////////////////
//
// Brings up the "Help..." dialog
//
static void
showHelpDialog()
{
   if (access("linkatron.help", R_OK) != 0) {
        system("xconfirm -t 'Sorry, could not find "
               "linkatron.help' > /dev/null");
        return;
    }

    char command[100];
    sprintf(command, "showcase -v linkatron.help");

    int err = system(command);
    if (err) {
        system("xconfirm -t 'You must install showcase"
               " for this function to work' > /dev/null");
        return;
    }
}

///////////////////////////////////////////////////////////////////
//
// If the scene is not empty, check to see if the user
// wants to save the scene first.
// If not, then create an empty scene so user can start over.
//
static void 
fileNewEvent()
{
    if (theWorld->isSceneEmpty() == FALSE ) {
	createOkayCancelDialog(mgrWidget, 
	    newSceneCB,
	    "New Scene Error Dialog",
	    "Your current scene is not empty.",
	    "Hit OK to continue, CANCEL if you want to Save first.");

    }
    else
	newSceneCB(NULL, NULL, NULL);
}

static void newSceneCB(Widget, void *, void *)
{
    theWorld->setScene( new SoSeparator );
}

///////////////////////////////////////////////////////////////////
//
// If the scene is not empty, check to see if the user
// wants to save the scene first.
// If not, then bring up the file dialog box and read a new scene from file.
//
static void 
fileOpenEvent()
{
    if (theWorld->isSceneEmpty() == FALSE ) {
	createOkayCancelDialog(mgrWidget, 
	    openSceneCB,
	    "Open Scene Error Dialog",
	    "Your current scene is not empty.",
	    "Hit OK to continue, CANCEL if you want to Save first.");

    }
    else
	openSceneCB(NULL, NULL, NULL);
}

static void openSceneCB(Widget, void *, void *)
{
    showFileSelectionDialog( (XtCallbackProc) readFromFileCB );
}

//
// Callback routine that gets called when the new filename
// has been entered. 
//
// Reads the scene and gives it to 'theWorld'
//
static void
readFromFileCB(Widget, void *, XmFileSelectionBoxCallbackStruct *data)
{
    // Get the file name
    char *filename;
    XmStringGetLtoR(data->value,
        (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename);
   
    files.count = 1;
    files.names[0] = strdup(filename);

    SoSeparator *fileRoot = readScene(files);
    if (fileRoot == NULL) {
        char str[100];
        strcpy(str, "Error reading file: ");
        strcat(str, files.names[0]);
        SoXt::createSimpleErrorDialog(mgrWidget, "File Error Dialog", str);
        return;
    } 

    theWorld->setScene( fileRoot );

    XtFree(filename);
}

///////////////////////////////////////////////////////////////////
//
// If the scene is empty, show a dialog and return.
//
// If there is more than one file in the list, direct them to Save As...
//
// If everything's okay, ask the world for the scene and write it out.
//
static void 
fileSaveEvent()
{
    if (theWorld->isSceneEmpty() == TRUE ) {
	SoXt::createSimpleErrorDialog(mgrWidget, 
	    "File Save Empty Error Dialog",
	    "The current scene is empty. Create a scene first,",
	    "or fix my code to grey out this menu item.");
    }
    else if ( files.count > 1) {
	    SoXt::createSimpleErrorDialog(mgrWidget, 
	    "File Save Needs Name Error Dialog",
	    "You began with more than one file.", 
	    "Use \"Save As...\" to save to a single file.");
    }
    else
	writeToFirstFile();
}

static void 
writeToFirstFile()
{
    SoWriteAction   wa;

    if ( files.count != 1 )
	return;

    if (! wa.getOutput()->openFile(files.names[0])) {
	char str[100];
	strcpy(str, "Error creating file: ");
	strcat(str, files.names[0]);
	SoXt::createSimpleErrorDialog(mgrWidget, 
				      "File Write Error Dialog", str);
    }
    else {
	// Get the scene from theWorld and write it out.
	wa.apply(theWorld->getScene());

	wa.getOutput()->closeFile();
    }
}

///////////////////////////////////////////////////////////////////
//
// If the scene is empty, show a dialog and return.
//
// If everything's okay, ask the world for the scene and write it out.
//
static void 
fileSaveAsEvent()
{
    if (theWorld->isSceneEmpty() == TRUE ) {
	SoXt::createSimpleErrorDialog(mgrWidget, 
	    "File Save As Empty Error Dialog",
	    "The current scene is empty. Create a scene first,",
	    "or fix my code to grey out this menu item.");
    }
    else
	showFileSelectionDialog( (XtCallbackProc) writeToFileCB );
}

//
// Callback routine that gets called when the new filename
// has been entered. 
//
// Writes the scene held by 'theWorld'
//
static void
writeToFileCB(Widget, void *, XmFileSelectionBoxCallbackStruct *data)
{
    // Get the file name
    char *filename;
    XmStringGetLtoR(data->value,
        (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename);
   
    files.count = 1;
    files.names[0] = strdup(filename);
    XtFree(filename);

    writeToFirstFile();
}

///////////////////////////////////////////////////////////////////
//
// If the scene is not empty, see if the user would like to save.
//
// Otherwise, quit.
//
static void 
fileQuitEvent()
{
    if (theWorld->isSceneEmpty() == FALSE ) {
	createOkayCancelDialog(mgrWidget, 
	    quitProgramCB,
	    "Quit Error Dialog",
	    "Your current scene is not empty.",
	    "Hit OK to Quit anyway, CANCEL if you want to Save first.");

    }
    else
	quitProgramCB(NULL, NULL, NULL);
}

static void quitProgramCB(Widget, void *, void *)
{
    exit(0);
}

///////////////////////////////////////////////////////////////////
//
// Use a motif file selection dialog to get the new filename.  
//
// When calling this, you must pass a callback for when the 
// OK button is pressed.
//
static void
showFileSelectionDialog( XtCallbackProc okCB )
{
    static Widget fileDialog = NULL;

    if (fileDialog == NULL) {
        Arg args[5];
        int n = 0;

        // Unmanage when ok/cancel are pressed
        XtSetArg(args[n], XmNautoUnmanage, TRUE); n++;
        fileDialog = XmCreateFileSelectionDialog(
            XtParent(mgrWidget), "File Dialog", args, n);

        XtAddCallback(fileDialog, XmNokCallback,
                      (XtCallbackProc)okCB, NULL);
    }
    else {
	// We need to remove the old callback, 
	// then install the one we were passed, since this file selector
	// is used for both reading and writing files.
        XtRemoveAllCallbacks(fileDialog, XmNokCallback );
        XtAddCallback(fileDialog, XmNokCallback,
                      (XtCallbackProc)okCB, NULL);
    }

    // Manage the dialog
    XtManageChild(fileDialog);
}


////////////////////////////////////////////////////////////////////////
//
// creates a dialog box.
// Prints a message and has two buttons:
// CANCEL and OK.
//
// When calling this, you must pass a callback for when the 
// OK button is pressed.
//
static void
createOkayCancelDialog(Widget widget, XtCallbackProc okCB,
    char *dialogTitle, char *str1, char *str2)
{
    Widget shell = SoXt::getShellWidget(widget);
    if (shell == NULL)
	return;
    
    Arg args[5];
    XmString xmstr = XmStringCreateSimple(str1);
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringCreateSimple(str2));
    
    int n = 0;
    XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
    XtSetArg(args[n], XtNtitle, dialogTitle); n++;
    XtSetArg(args[n], XmNmessageString, xmstr); n++;
    Widget dialog = XmCreateErrorDialog(shell, "Error Dialog", args, n);
    XmStringFree(xmstr);
    
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    
    // First, destroy the dialog box.
    // Then, call the passed callback.
    XtAddCallback(dialog, XmNokCallback, 
	(XtCallbackProc) destroyDialogCB, (XtPointer)NULL);
    XtAddCallback(dialog, XmNokCallback, 
	(XtCallbackProc) okCB, (XtPointer)NULL);

    // If the user cancels, we'll always want to destroy the dialog.
    // register callback to destroy (and not just unmap) the dialog
    XtAddCallback(dialog, XmNcancelCallback, 
	(XtCallbackProc) destroyDialogCB, (XtPointer)NULL);
    
    XtManageChild(dialog);
}

static void destroyDialogCB(Widget dialog, void *, void *)
{ XtDestroyWidget(dialog); }


///////////////////////////////////////////////////////////////////
//
// Process the topbar menu events.
static void
processTopbarEvent(Widget, mmMenuItem *data, XmAnyCallbackStruct *)
{
    switch (data->id) {

    //
    // File
    //
 
    case MM_FILE_NEW:
	fileNewEvent();
	break;

    case MM_FILE_OPEN:
	fileOpenEvent();
	break;

    case MM_FILE_SAVE:
	fileSaveEvent();
	break;

    case MM_FILE_SAVE_AS:
	fileSaveAsEvent();
	break;

    case MM_FILE_QUIT:
	fileQuitEvent();
	break;	

    //
    // Edit
    //
  
    case MM_EDIT_DELETE:
	// Add the current selection to the list of deleted objects.
	theWorld->deleteCurrentPiece();
	break;

    case MM_EDIT_UNDELETE:
	// Undelete the most recently deleted object.
	theWorld->undeletePiece();
	break;

    //
    // Options
    //
  
    case MM_OPTIONS_SHADOW:
	if (theWorld->isShadowOn())
	    theWorld->setShadowOn( FALSE );
	else
	    theWorld->setShadowOn( TRUE );
	break;

    case MM_OPTIONS_SOUND:
	if (Sound::isEnabled())
	    Sound::enable( FALSE );
	else
	    Sound::enable( TRUE );
	break;

    case MM_OPTIONS_TABLE:
	if (theWorld->isTableOn())
	    theWorld->setTableOn( FALSE );
	else
	    theWorld->setTableOn( TRUE );
	break;

    case MM_OPTIONS_TRANSPARENCY:
	// Toggle the transparency mode
	if (viewer->getTransparencyType() == SoGLRenderAction::SCREEN_DOOR)
	    viewer->setTransparencyType(SoGLRenderAction::DELAYED_BLEND);
	else
            viewer->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
	break;

    // 
    // Help
    // 
    case MM_HELP_HELP:
 	showHelpDialog();
	break;

    } 
}

///////////////////////////////////////////////////////////////////////
//
// Create the top menu bar and the associated menus
//
static Widget
buildAndLayoutMenus(Widget mgrWidget)
{
    mmMenuItem *menuItems = new mmMenuItem[MM_MENU_NUM];
    for (int i=0; i<MM_MENU_NUM; i++) {
	menuItems[i].id = i;
	menuItems[i].widget = NULL;
    }

    // 
    // Create the topbar menu
    //
    Widget menuWidget = XmCreateMenuBar(mgrWidget, "menuBar", NULL, 0);

    Arg popupargs[4];
    int popupn = 0;
#ifdef MENUS_IN_POPUP
    SoXt::getPopupArgs(XtDisplay(menuWidget), NULL, popupargs, &popupn);
#endif

    int itemCount = XtNumber(pulldownData);
    WidgetList buttons = (WidgetList) XtMalloc(itemCount * sizeof(Widget));
    Widget popupWidget = NULL;

    
    Arg args[12];
    int n;
    for (i=0; i<itemCount; i++) {
	//
        // Make Topbar menu button
	//
        Widget subMenu = 
		XmCreatePulldownMenu(menuWidget, NULL, popupargs, popupn);

        // We only need one widget for loading the proper popup colormap
        if (! popupWidget)
            popupWidget = subMenu;

        int id = pulldownData[i].id;
        menuItems[id].widget = subMenu;

        XtSetArg(args[0], XmNsubMenuId, subMenu);
        buttons[i] = XtCreateWidget(pulldownData[i].name,
            		xmCascadeButtonGadgetClass, menuWidget, args, 1);

	//
        // Make subMenu buttons
	//
        int subItemCount = pulldownData[i].subItemCount;
        WidgetList subButtons = 
		(WidgetList) XtMalloc(subItemCount * sizeof(Widget));

        for (int j=0; j<subItemCount; j++) {
            if (pulldownData[i].subMenu[j].id == MM_SEPARATOR) {
                subButtons[j] = XtCreateWidget(
			NULL, xmSeparatorGadgetClass, subMenu, NULL, 0);
  	    }
            else {
		String callbackReason;

                switch (pulldownData[i].subMenu[j].buttonType) {
                    case MM_PUSH_BUTTON:
                        widgetClass = xmPushButtonGadgetClass;
                        callbackReason = XmNactivateCallback;
                        n = 0;
                        break;
                    case MM_TOGGLE_BUTTON:
                        widgetClass = xmToggleButtonGadgetClass;
                        callbackReason = XmNvalueChangedCallback;
                        n = 0;
                        break;
                    case MM_RADIO_BUTTON:
                        widgetClass = xmToggleButtonGadgetClass;
                        callbackReason = XmNvalueChangedCallback;
                        XtSetArg(args[0], XmNindicatorType, XmONE_OF_MANY);
                        n = 1;
                        break;
                    case MM_SEPARATOR:
			n = 0;
			break;
                    default:
                        fprintf(stderr, 
				"linkatron INTERNAL ERROR: bad buttonType\n");
                        break;
                }

		//
                // Check for keyboard accelerator
		//
                char *accel = pulldownData[i].subMenu[j].accelerator;
                char *accelText = pulldownData[i].subMenu[j].accelText;
                XmString xmstr = NULL;
                if (accel != NULL) {
                    XtSetArg(args[n], XmNaccelerator, accel); n++;

                    if (accelText != NULL) {
                        xmstr = XmStringCreate(accelText,
                                         XmSTRING_DEFAULT_CHARSET);
                        XtSetArg(args[n], XmNacceleratorText, xmstr); n++;
                    }
                }

                subButtons[j] = XtCreateWidget(
					pulldownData[i].subMenu[j].name,
                    			widgetClass, subMenu, args, n);
                if (xmstr != NULL)
                    XmStringFree(xmstr);
                id = pulldownData[i].subMenu[j].id;
                menuItems[id].widget = subButtons[j];
                XtAddCallback(subButtons[j], callbackReason,
                    (XtCallbackProc)processTopbarEvent,
                    (XtPointer) &menuItems[id]);

		// If a toggle, set the starting state:
                if (pulldownData[i].subMenu[j].buttonType == MM_TOGGLE_BUTTON){
		    SbBool isOn;
		    isOn = pulldownData[i].subMenu[j].isOn;

		    // Sound on/off is determined by command line input.
		    if (pulldownData[i].subMenu[j].id == MM_OPTIONS_SOUND)
			isOn = withSound;
			
		    XmToggleButtonGadgetSetState(subButtons[j], isOn, FALSE);
		}
            }
        }
        XtManageChildren(subButtons, subItemCount);
        XtFree((char *)subButtons);
    }
    XtManageChildren(buttons, itemCount);
    XtFree((char *)buttons);

    //
    // Layout the menu bar
    //
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetValues(menuWidget, args, n);
    XtManageChild(menuWidget);

    return (menuWidget);
}

///////////////////////////////////////////////////////////////////
// Create the buttonStrip and lay it out so that it is attached
// to the bottom of the menu bar
//
static ButtonStrip *
buildAndLayoutButtonStrip(Widget mgrWidget, Widget menuWidget)
{
    //
    //
    Arg args[12];
    int n = 0;
    buttonStrip = new ButtonStrip(mgrWidget);

    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,        menuWidget); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,   20); n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetValues(buttonStrip->getRenderArea()->getWidget(), args, n);

    return (buttonStrip);
}


///////////////////////////////////////////////////////////////////
// Create the viewer and lay it out so that it is attached
// to the bottom of the buttonStrip
//
static SoXtExaminerViewer *
buildAndLayoutViewer(Widget mgrWidget, Widget buttonStripWidget)
{
    //
    //
    Arg args[12];
    int n = 0;
    SoXtExaminerViewer *viewer = new SoXtExaminerViewer(mgrWidget);
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,        buttonStripWidget); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetValues(viewer->getWidget(), args, n);

    return (viewer);
}

static void 
newRigidStrutCB( void *inWorld, SoDragger *)
{ 
    ((WorldInfo *)inWorld)->addNewRigidStrut(); 
}

static void 
newDoubleStrutCB( void *inWorld, SoDragger *)
{ 
    ((WorldInfo *)inWorld)->addNewDoubleStrut(); 
}

static void 
newDoubleArrowCB( void *inWorld, SoDragger *)
{ 
    ((WorldInfo *)inWorld)->addNewDoubleArrow(); 
}

static void 
newBellCB( void *inWorld, SoDragger *)
{ 
    ((WorldInfo *)inWorld)->addNewBell(); 
}

static void 
newRotorMotorCB( void *inWorld, SoDragger *)
{ 
    ((WorldInfo *)inWorld)->addNewRotorMotor(); 
}

static void 
newWheelCB( void *inWorld, SoDragger *)
{ 
    ((WorldInfo *)inWorld)->addNewWheel(); 
}

static void 
newArrowCB( void *inWorld, SoDragger *)
{ 
    ((WorldInfo *)inWorld)->addNewArrow(); 
}

///////////////////////////////////////////////////////////////////
//
main(int argc, char **argv)
{
    // Parse command line arguments
    parse_args(argc, argv);

    // Initialize Inventor 
    Widget mainWindow = SoXt::init(argv[0]);
    SoDB::setRealTimeInterval( .03 );

    CheckingEngine::initClass();
    EncloseEngine::initClass();
    AxleFollowEngine::initClass();
    StrutEngine::initClass();
    RigidStrutEngine::initClass();
    DoubleStrutMoveEndEngine::initClass();
    DoubleStrutMoveMiddleEngine::initClass();
    WheelEngine::initClass();
    ArrowEngine::initClass();
    DoubleArrowEngine::initClass();
    BellEngine::initClass();
    ZAngleFromRotationEngine::initClass();

    Shadow::initClass();
    ShadowKit::initClass();
    GraphPaper::initClass();
    TableTop::initClass();

    MechPiece::initClass();
    AxleShaft::initClass();
    AxleLeg::initClass();
    AxlePin::initClass();
    Axle::initClass();

    Button::initClass();
    ToggleButton::initClass();

    LinkBase::initClass();
    SimpleLink::initClass();
    Strut::initClass();
    RigidStrut::initClass();
    DoubleStrut::initClass();
    Wheel::initClass();
    Arrow::initClass();
    DoubleArrow::initClass();
    Bell::initClass();

    MotorBase::initClass();    
    RotorMotor::initClass();    

    // Read the resource file that should contain 
    // resource geometries for this program.
    readResources();

    // See if you should write out
    int curArg = 1;

    // Create the form widget that holds everything together
    Arg args[10];
    int n = 0;
    XtSetArg(args[n], XtNwidth, 520); n++;
    XtSetArg(args[n], XtNheight, 510); n++;
    mgrWidget = XtCreateWidget(
			"Form", xmFormWidgetClass, mainWindow, args, n); 

    // Create and lay out the menus
    Widget menuWidget = buildAndLayoutMenus(mgrWidget);

    // Create and lay out the buttonStrip
    buttonStrip = buildAndLayoutButtonStrip(mgrWidget, menuWidget);

    // Create and lay out the viewer
    viewer = buildAndLayoutViewer(mgrWidget, buttonStrip->getRenderArea()->getWidget() );

    // Turn off the headlight since we create our own light
    viewer->setHeadlight( FALSE );

    // Create a worldInfo. It will contain a worldRoot, under
    // which is a tableTop followed by an empty scene (a single separator).
    theWorld = new WorldInfo;
    // Establish the minimum size for the table.
    theWorld->minTableSize.setBounds(-2,0,-2,2,4,2);
    // Read the input scene from the file(s)
    // The size of the table will adjust to fit the scene.
    theWorld->setScene (readScene(files));
    theWorld->setShadowOn( FALSE );

    // The lower left corner of the window is used to draw the Inventor logo
    setOverlayLogo(viewer);

    // We'll need a handy UNPICKABLE node so that users don't accidentally
    // manipulate the icons instead of pick them. Let them pick through
    // to the background instead.
    SoPickStyle *ps = new SoPickStyle;
    ps->ref();
    ps->style = SoPickStyle::UNPICKABLE;
    ps->setOverride(TRUE);

    // Add buttons to the strip.

    SoSeparator *rigidStrutIcon = new SoSeparator;
    rigidStrutIcon->ref();
    rigidStrutIcon->addChild(ps);
    SoTransform *rigidStrutXf = new SoTransform;
    rigidStrutIcon->addChild( rigidStrutXf );
    rigidStrutXf->scaleFactor.setValue( .4, .4, .4 );
    rigidStrutXf->translation.setValue( 0,  -.4, 0 );
    RigidStrut *myRigidStrut = new RigidStrut;
    rigidStrutIcon->addChild( myRigidStrut );

    SoSeparator *doubleStrutIcon = new SoSeparator;
    doubleStrutIcon->ref();
    doubleStrutIcon->addChild(ps);
    SoTransform *doubleStrutXf = new SoTransform;
    doubleStrutIcon->addChild( doubleStrutXf );
    doubleStrutXf->scaleFactor.setValue( .4, .4, .4 );
    doubleStrutXf->translation.setValue( 0,  -.4, 0 );
    DoubleStrut *myDoubleStrut = new DoubleStrut;
    doubleStrutIcon->addChild( myDoubleStrut );
    myDoubleStrut->set( "strut1.endPointAxle", "origin 0 2.732 0" );
    myDoubleStrut->size1 = 2;
    myDoubleStrut->size2 = 2;
    myDoubleStrut->set("strut1.originAxle", "origin -1.5 1 0");
    myDoubleStrut->set("strut2.endPointAxle", "origin 1.5 1 0");

    SoSeparator *wheelIcon = new SoSeparator;
    wheelIcon->ref();
    wheelIcon->addChild(ps);
    SoTransform *wheelXf = new SoTransform;
    wheelIcon->addChild( wheelXf );
    wheelXf->scaleFactor.setValue( .4, .4, .4 );
    wheelXf->translation.setValue( 0,  -.4, 0 );
    Wheel *myWheel = new Wheel;
    wheelIcon->addChild( myWheel );
    myWheel->set("originAxle", "origin 0 1.5 0");
    myWheel->radius = 1;
    myWheel->zRotation = 0;

    SoSeparator *arrowIcon = new SoSeparator;
    arrowIcon->ref();
    arrowIcon->addChild(ps);
    SoTransform *arrowXf = new SoTransform;
    arrowIcon->addChild( arrowXf );
    arrowXf->scaleFactor.setValue( .4, .4, .4 );
    arrowXf->translation.setValue( 0,  -.4, 0 );
    Arrow *myArrow = new Arrow;
    arrowIcon->addChild( myArrow );
    myArrow->set("originAxle", "origin -1 1 0");
    myArrow->set("dirPointAxle", "origin 0 2 0");
    myArrow->length = 3;

    SoSeparator *doubleArrowIcon = new SoSeparator;
    doubleArrowIcon->ref();
    doubleArrowIcon->addChild(ps);
    SoTransform *doubleArrowXf = new SoTransform;
    doubleArrowIcon->addChild( doubleArrowXf );
    doubleArrowXf->scaleFactor.setValue( .4, .4, .4 );
    doubleArrowXf->translation.setValue( 0,  -.4, 0 );
    DoubleArrow *myDoubleArrow = new DoubleArrow;
    doubleArrowIcon->addChild( myDoubleArrow );
    myDoubleArrow->set("arrow1.originAxle", "origin -1.5 1 0");
    myDoubleArrow->set("arrow2.originAxle", "origin 1.5 1 0");
    myDoubleArrow->set("arrow1", "length 1.5");
    myDoubleArrow->set("arrow2", "length 2.5");

    SoSeparator *bellIcon = new SoSeparator;
    bellIcon->ref();
    bellIcon->addChild(ps);
    SoTransform *bellXf = new SoTransform;
    bellIcon->addChild( bellXf );
    bellXf->scaleFactor.setValue( .4, .4, .4 );
    bellXf->translation.setValue( 0,  -.4, 0 );
    Bell *myBell = new Bell;
    bellIcon->addChild( myBell );
    myBell->set("arrow1.originAxle", "origin -2 1 0");
    myBell->set("arrow2.originAxle", "origin 1.5 1 0");
    myBell->set("arrow1", "length 1.5");
    myBell->set("arrow2", "length 1.0");

    SoSeparator *rotorMotorIcon = new SoSeparator;
    rotorMotorIcon->ref();
    rotorMotorIcon->addChild(ps);
    SoTransform *rotorMotorXf = new SoTransform;
    rotorMotorIcon->addChild( rotorMotorXf );
    rotorMotorXf->scaleFactor.setValue( .4, .4, .4 );
    rotorMotorXf->translation.setValue( 0,  -.4, 0 );
    RotorMotor *myRotorMotor = new RotorMotor;
    rotorMotorIcon->addChild( myRotorMotor );
    myRotorMotor->set("axle", "origin 0 1 0");
    myRotorMotor->on = FALSE;

    buttonStrip->addButton( rigidStrutIcon, rigidStrutIcon, 
			    &newRigidStrutCB, theWorld );
    buttonStrip->addButton( doubleStrutIcon, doubleStrutIcon, 
			    &newDoubleStrutCB, theWorld );
    buttonStrip->addButton( wheelIcon, wheelIcon, 
			    &newWheelCB, theWorld );
    buttonStrip->addButton( arrowIcon, arrowIcon, 
			    &newArrowCB, theWorld );
    buttonStrip->addButton( doubleArrowIcon, doubleArrowIcon, 
			    &newDoubleArrowCB, theWorld );
    buttonStrip->addButton( bellIcon, bellIcon, 
			    &newBellCB, theWorld );
    buttonStrip->addButton( rotorMotorIcon, rotorMotorIcon, 
			    &newRotorMotorCB, theWorld );
    ps->unref();

    // Display the viewer with the menus and loop forever
    buttonStrip->getRenderArea()->show();
    viewer->setSceneGraph(theWorld->getWorldRoot());
    viewer->show();

    // Initialize the sound effects
    Sound::init();
    Sound::enable(withSound);

    SoXt::show(mgrWidget);
    SoXt::show(mainWindow);
    SoXt::mainLoop();
}

