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
/*
 * Copyright (C) 1990,91,92,93,94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.48 $
 |
 |   Classes:
 |      main()
 |
 |   Author(s)  : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

// This define turns on the performance meter.  Comment this line out
// to make it go away
// #define PERF_METER

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#ifdef SDL_PORT
#include <SoSDL.h>
#include <SoSDLRenderArea.h>
// Source-compat aliases: the body of this file keeps SGI's names.
#define SoXtRenderArea SoSDLRenderArea
#define SoXtComponent  SoSDLRenderArea
#else
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#endif
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransformSeparator.h>
#include <Inventor/sensors/SoIdleSensor.h>
#include <Inventor/sensors/SoOneShotSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#ifdef PERF_METER
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/nodes/SoColorIndex.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoText2.h>
#endif
#include "../../samples/common/InventorLogo.h"
#include <getopt.h>
#ifdef SDL_PORT
#include <GL/gl.h>
#include <unistd.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFLong.h>
#endif
#include <math.h>
#include "RaceCar.h"
#include "RobotCar.h"
#include "State.h"
#include "Packet.h"
#include "Network.h"
#include "Track.h"
#include "Sound.h"
#include "Curve.h"
#include "Straight.h"
#include "StartFinish.h"
#include "Sky.h"
#include "LODD.h"

#define SHORT_START_DELAY 15.0
#define LONG_START_DELAY  45.0

#ifndef SDL_PORT
// Xt Input callback for receiving packets from the network
extern void packetCB( XtPointer, int *, XtInputId * );
#endif

// Idle Sensor callback for running the simulation
extern void runSimulation( void *, SoSensor * );

// Callback to handle mouse input
extern void mouseInputCB(void *car, SoEventCallback *eventCBnode);

static void setupGame(SbBool);

static SoOneShotSensor *polySensor = NULL;
static SoXtRenderArea  *myRenderArea = NULL;
static GameState *gameState = NULL;
static int numRobotCars = 0;
static int gameNum      = 0;

#ifdef PERF_METER
static int numFramesRendered = 0;
static void initPerfMeter(SoGroup *);
static void countUpdatesCB(void *, SoAction *);
static void perfSensorCB(void *, SoSensor *);
static SoSeparator *addUpdateCounter(SoNode *root);
#endif

////////////////////////////////////////////////////////////////////////
//
//  Set the Inventor logo on the player's screen.
//
static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
	glViewport(0, 0, 80, 80);
    }
}

static void
setOverlayLogo(SoSeparator *overRoot)
{
    static SoSeparator *logo = NULL;
    
    if (logo == NULL) {
	SoInput in;
	in.setBuffer((void *)ivLogo, ivLogoSize);
	logo = SoDB::readAll(&in);
	logo->ref();
	in.closeFile();
	
	// Add a callback node which will set the viewport
	SoCallback *cb = new SoCallback;
	cb->setCallback(logoCB);
	logo->insertChild(cb, 0);
    }
    
//    SbColor col(0, 0, 0);
//    ra->setOverlayColorMap(0, 1, &col);
    overRoot->addChild(logo);	
}
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Print out a usage message.
//

void
usage()

//
////////////////////////////////////////////////////////////////////////
{
    fprintf(stderr, "\nUsage:  slotcar [-c 1-8] [-g 1-100] [-h] "
	    	    "[-n name] [-q] [-s] [-r 1-8] \n");
    fprintf(stderr, "    [-r 1-8]  :  Number of robot cars (default 0).\n");
    fprintf(stderr, "    [-n name] :  Your name (maximum 15 characters, "
		    "default is login).\n");
    fprintf(stderr, "    [-c 1-8]  :  Desired car number "
	    	    "(chosen randomly by default)\n");
    fprintf(stderr, "    [-g 1-100]:  Private game from 1 to 100.\n");
    fprintf(stderr, "    [-s]      :  Single-player mode "
	    	    "(no network stuff).\n");
    fprintf(stderr, "    [-q]      :  Play the game quietly (no sound).\n");
    fprintf(stderr, "    [-h]      :  Print out this message.\n");
    exit(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Exit the program.
//

void
exitCB(void *userData, SoXtComponent *)

//
////////////////////////////////////////////////////////////////////////
{
    GameState *state = (GameState *)userData;

    // Send exit packets for all cars we're simulating:
    for (int i = 0; i < state->getNumCars(); i++) {
	Car *car = state->getCar(i);
	CarExit exitPacket(car->getCarNum());
	state->network->send(exitPacket, sizeof(CarExit));
    }
    exit(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Enable the z-buffer.
//

void
zEnableCB(void *, SoAction *)

//
////////////////////////////////////////////////////////////////////////
{
    glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Disable the z-buffer the program.
//

void
zDisableCB(void *, SoAction *)

//
////////////////////////////////////////////////////////////////////////
{
    glDisable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////////
//
// This callback is called after the window is displayed and the first
// rendering has finished. It times polygon rendering to see if it's
// fast enough for real-time play.
//
////////////////////////////////////////////////////////////////////////////

static void
timePolygonCB(void *, SoSensor *)
{
    SoInput reader;
    reader.openFile("PolygonTime.iv");
    SoSeparator *testRoot = SoDB::readAll(&reader);
    reader.closeFile();
    testRoot->ref();
    myRenderArea->setSceneGraph(testRoot);

    // Scale the square so that it takes up about 200 x 200 pixels.
    // (The square by default takes up the entire window.)
    float scale = 200.0 / (float) myRenderArea->getSize()[1];
    SoScale *polyScale = (SoScale *) SoNode::getByName("PolygonTimeScale");
    polyScale->scaleFactor.setValue(scale, scale, scale);

    // Render the scene once to make sure everything is loaded
    myRenderArea->render();

    // Draw the scene three times and time the results
    SbTime startTime, endTime;
    startTime.setToTimeOfDay();
    for (int i=0; i<3; i++) 
        myRenderArea->render();
    endTime.setToTimeOfDay();
    testRoot->unref();

    // See if it takes too much time. If so,
    // setup the simpler scene.
    double elapsed = (endTime - startTime).getValue();
    if (elapsed > .09) {
	setupGame(TRUE);
    }
    else {
        setupGame(FALSE);
    }

    delete polySensor;
}

////////////////////////////////////////////////////////////////////////////
//
// This callback is called when the render area window becomes mapped
// and it does its first render. It schedules a sensor that will do
// the texture timing stuff.
//
////////////////////////////////////////////////////////////////////////////

static void
waitForWindowCB(void *, SoAction *action)
{
    if (action->getTypeId() == SoGLRenderAction::getClassTypeId()) {
	polySensor = new SoOneShotSensor(timePolygonCB, NULL);
	polySensor->setPriority(1);
	polySensor->schedule();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is the mainline program for the slotcar game.
//

static int
slotcar_main( int argc, char *argv[] )

//
////////////////////////////////////////////////////////////////////////
{
#ifdef SDL_PORT
    // Bundled Utopia fonts (Adobe's freely redistributable Type1 set,
    // from TeX Live) for the HUD and startup-screen text; libFL looks
    // for files named exactly as the scene's Font nodes request.
    setenv("FL_FONT_PATH", "fonts", 0);
    // Initialize Inventor and SDL
    SoSDL::init();
#else
    // Initialize Inventor and Xt
    Widget myWindow = SoXt::init(argv[0]);  
    if (myWindow == NULL) exit(1);     
#endif

    // Initialize new classes:
    Sky::initClass();
    TrackSegment::initClass();
    Straight::initClass();
    StartFinish::initClass();
    Curve::initClass();
    LODD::initClass();

    // Parse the command line
    int carNum       = -1;
    SbBool quietGame  = FALSE;
#ifdef SDL_PORT
    const char *userName = getenv("USER");
    SbString playerName(userName ? userName : "player");
#else
    SbString playerName(cuserid(NULL));
#endif

    int err = 0;	// Flag: error in options?
    int c;

    // Note: optind and optarg are declared in getopt.h
    while ((c = getopt(argc, argv, "c:g:n:r:sqh")) != -1)
    {
	switch(c)
	{
	  case 'c':
	    carNum = atoi(optarg);
            if ((carNum < 1) || (carNum > 8)) err = 1;
	    break;
	  case 'g':
	    if (gameNum != 0) {
		fprintf(stderr, "Only one -g or -s option allowed\n");
		err = 1;
	    }
	    gameNum = atoi(optarg);
            if ((gameNum < 0) || (gameNum > 100)) err = 1;
	    break;
	  case 'n':
	    playerName = optarg;
	    if (playerName.getLength() > 15) err = 1;
	    break;
	  case 'q':
	    quietGame = 1;
	    break;
	  case 'r':
	    numRobotCars = atoi(optarg);
            if ((numRobotCars < 0) || (numRobotCars > 8)) err = 1;
	    break;
	  case 's':
	    if (gameNum != 0) {
		fprintf(stderr, "Only one -g or -s option allowed\n");
		err = 1;
	    }
	    gameNum = DISABLE_NETWORK_GAMEID;
	    break;
	  case 'h':	// Help
	  default:
	    err = 1;
	    break;
	}
    }
    if (err)   {
	usage();
	exit(1);
    }

#ifdef SDL_PORT
    // Single-player port: force the (existing) disabled-network path.
    if (gameNum != DISABLE_NETWORK_GAMEID && gameNum != 0)
	fprintf(stderr, "slotcar: network play not supported in this port; "
		"running single-player (-s). Use -r N for robot opponents.\n");
    gameNum = DISABLE_NETWORK_GAMEID;
#endif
    // Init random-number generator:
    srand48((long)getpid());

    // Initialize the sound class
    Sound::init();

    // Initialize the game state
    gameState = new GameState;
    gameState->playQuietly    = quietGame;
    gameState->playerName     = playerName;
    gameState->myCarNumber    = carNum;
    if (gameNum%2)
        gameState->startDelay = SHORT_START_DELAY;
    else
        gameState->startDelay = LONG_START_DELAY;

    //Create the render area
#ifdef SDL_PORT
    // 2x the original 650x400 window. The startup screen's 3D text is
    // framed by an ortho camera designed for the 1.625:1 window aspect;
    // a 1280x1024 (1.25:1) window crops the view horizontally and the
    // title/instructions overflow. Doubling preserves the designed
    // framing while giving modern-display pixel density (the original
    // ran windowed on a 1280x1024 SGI monitor).
    myRenderArea = new SoSDLRenderArea("Slotcar Race", 1300, 800);
    myRenderArea->setWindowCloseCallback(exitCB, gameState);
#else
    myRenderArea = new SoXtRenderArea(myWindow);
    myRenderArea->setSize(SbVec2s(650, 400));
    myRenderArea->setTitle("Slotcar Race");
    myRenderArea->setWindowCloseCallback(exitCB, gameState);
#endif

    // The first thing we want to do is to figure out whether to use 
    // a simplified scene or not. We'll do this by drawing and timing
    // a scene graph containing a  polygon. We have to make sure the
    // window is mapped first, though, so we set up a callback node to
    // do that.
    SoCallback *mapCB = new SoCallback;
    mapCB->setCallback(waitForWindowCB, NULL);
    // Disable auto-redraw so that we render only when we get the
    // expose event for the window - this means we will render this
    // node only once.
    myRenderArea->setAutoRedraw(FALSE);
    myRenderArea->setSceneGraph(mapCB);

    SbColor overlayColors[3];
    overlayColors[0].setValue(0.0, 0.00, .25);
    overlayColors[1].setValue(.9, .0, .0);
    overlayColors[2].setValue(.9, .0, .0);
    myRenderArea->setOverlayColorMap(1, 3, overlayColors);

#ifdef SDL_PORT
    SoSDL::mainLoop();
#else
    myRenderArea->show();
    SoXt::show(myWindow);

    SoXt::mainLoop();
#endif
    return 0;
}

#ifdef __EMSCRIPTEN__
// emcc 3.1.6 + clang 15 disagree on the two-arg main symbol name
// (__main_argc_argv), which silently strips the program; use a zero-arg
// main (same workaround as maze). The browser supplies no arguments:
// default to single player with three robot opponents.
int
main()
{
    static char *argv[] = {
	(char *)"slotcar", (char *)"-s", (char *)"-r", (char *)"3", NULL };
    return slotcar_main(4, argv);
}
#else
int
main( int argc, char *argv[] )
{
    return slotcar_main(argc, argv);
}
#endif

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Setup the scene and the game state.
//

void
setupGame(SbBool useAbbrev)

//
////////////////////////////////////////////////////////////////////////
{
    SoSeparator *root = new SoSeparator;
    root->ref();
    root->renderCaching = SoSeparator::OFF;
   
#ifdef PERF_METER  
    numFramesRendered = 0;
#endif

    // Switch, used to change between modes (racing, practicing,
    // startup screen, etc):
    SoSwitch *modeSwitch = new SoSwitch;
    root->addChild(modeSwitch);

    // This global field is changed to switch between modes.  By using
    // a global field, we can automatically change the scene graphs
    // read in without writing any code.
    SoSFLong *modeField = (SoSFLong *)
	SoDB::createGlobalField("GameMode",
				SoSFLong::getClassTypeId());
    if (modeField == NULL) {
	fprintf(stderr, "Couldn't find GameMode global field\n");
	exit(1);
    }
    modeSwitch->whichChild.connectFrom(modeField);
    
    // Read in start screens
    SoInput reader;
    reader.openFile("Startup.iv");
    SoSeparator *startScreen = SoDB::readAll(&reader);
    reader.closeFile();
    if (startScreen == NULL ) {
	fprintf(stderr, "Couldn't read startup screens\n");
	exit(1);
    }
    modeSwitch->addChild(startScreen);

    // And read in the overlay heads-up display:
    reader.openFile("HeadsUp.iv");
    SoSeparator *headsUpGraph = SoDB::readAll(&reader);
    reader.closeFile();
    if (headsUpGraph == NULL) {
	fprintf(stderr, "Couldn't read overlay screen\n");
	exit(1);
    }
    headsUpGraph->ref();

    // Read the track scenery geometry from a file and add to the scene
    // Instanced five times, because the track looks the same in
    // Practicing/Starting/Racing/After-Race/Race-Over modes:
    Track *theTrack = new Track;
    SoSeparator *trackGeom = theTrack->getTrack(gameNum%2, useAbbrev);
    modeSwitch->addChild(trackGeom);
    modeSwitch->addChild(trackGeom);
    modeSwitch->addChild(trackGeom);
    modeSwitch->addChild(trackGeom);
    modeSwitch->addChild(trackGeom);

    // Create and initialize the state structure for storing the
    // simulation data and the network for sending and receiving data
    // from other processes running the game.
    gameState->network = new Network(gameNum);
    gameState->packetTime.setToTimeOfDay();
    gameState->startTime.setToTimeOfDay();
    gameState->gameTime       = SbTime::zero();
    gameState->scene          = root;
    gameState->track          = theTrack;
    gameState->mode           = modeField;
    gameState->mode->setValue(GameState::START_SCREEN);
    gameState->carBox[0].setValue(CAR_X_MIN, CAR_Y_MIN, CAR_Z_MIN);
    gameState->carBox[1].setValue(CAR_X_MIN, CAR_Y_MIN, CAR_Z_MAX);
    gameState->carBox[2].setValue(CAR_X_MAX, CAR_Y_MIN, CAR_Z_MIN);
    gameState->carBox[3].setValue(CAR_X_MAX, CAR_Y_MIN, CAR_Z_MAX);
    gameState->carBox[4].setValue(CAR_X_MIN, CAR_Y_MAX, CAR_Z_MIN);
    gameState->carBox[5].setValue(CAR_X_MIN, CAR_Y_MAX, CAR_Z_MAX);
    gameState->carBox[6].setValue(CAR_X_MAX, CAR_Y_MAX, CAR_Z_MIN);
    gameState->carBox[7].setValue(CAR_X_MAX, CAR_Y_MAX, CAR_Z_MAX);
    gameState->cameraUpDown = 10;
    gameState->cameraBackForth = 10;

    // Create robot cars, and add them to the track:
    for (int i = 0; i < numRobotCars; i++) {
	RobotCar *car = new RobotCar;
	// old: trackGeom->addChild(car->getCarRoot());
	theTrack->addCar(car);
	gameState->addCar(car, FALSE);
    }

    // Add event callback AS FIRST CHILD for handling mouse events:
    SoEventCallback *evCB = new SoEventCallback;
    root->insertChild(evCB, 0); 	// old: root->addChild(evCB);
    evCB->addEventCallback(SoLocation2Event::getClassTypeId(),
			   mouseInputCB, gameState);
    evCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
			   mouseInputCB, gameState);
    evCB->addEventCallback(SoKeyboardEvent::getClassTypeId(),
			   mouseInputCB, gameState);

    // Setup the camera to be parented to the car.  
    SoPerspectiveCamera  *camera    = new SoPerspectiveCamera;
    camera->position.setValue(SbVec3f(0.0, 1.5, 0.0));
    camera->nearDistance = 1.0;
    trackGeom->insertChild(camera, 0);
    gameState->camera = camera;

    // Search for zbuffer callback nodes and setup their callbacks
    SoCallback *cbNode = (SoCallback *)SoNode::getByName("Z_BUFFER_ENABLE");
    if (cbNode != NULL)
        cbNode->setCallback(zEnableCB);
    cbNode = (SoCallback *)SoNode::getByName("Z_BUFFER_DISABLE");
    if (cbNode != NULL)
        cbNode->setCallback(zDisableCB);
    
    // Simulated sunlight:
    SoDirectionalLight   *sun       = new SoDirectionalLight;
    // old: sun->direction.setValue(SbVec3f(0.1, -1.0, 0.1));
    sun->direction.setValue(SbVec3f(-0.5, -1.0, -0.5));
    trackGeom->insertChild(sun, 1);

    // Set farDistance for the camera.
    // Currently, this is fixed, but should be dynamically calculated.
    //camera->farDistance = 1200;
    camera->farDistance = 5000;
    
    // Initialize the idle sensor for running the simulation
    gameState->simulationSensor = new SoIdleSensor(runSimulation,
						   gameState);

#ifdef PERF_METER
    // Create a performance meter for showing frames per second
    SoSeparator *oldRoot = root;
    root = addUpdateCounter(oldRoot);
#endif

#ifdef PERF_METER
    initPerfMeter(headsUpGraph);
#endif

    // Add the Inventor Logo to the headsup graph
    setOverlayLogo(headsUpGraph);

    myRenderArea->setAutoRedraw(TRUE);
    myRenderArea->setSceneGraph(root);
    myRenderArea->setOverlaySceneGraph(headsUpGraph);
    myRenderArea->setBackgroundColor( SbColor(.25, .45, .25) );

    // Start the game!!
    SoDB::setDelaySensorTimeout(SbTime(.03));

#ifdef SDL_PORT
    // Network play needs an input hook on the packet socket; not wired
    // up in the SDL port yet. Network was created disabled (see below).
#else
    if (gameNum != DISABLE_NETWORK_GAMEID) {
	XtAppAddInput(
	    SoXt::getAppContext(),
	    gameState->network->getfd(),
	    (XtPointer) XtInputReadMask,
	    packetCB,
	    (XtPointer) gameState);
    }
#endif

}


#ifdef PERF_METER
////////////////////////////////////////////////////////////////////
//
// Initialize the performance meter in the overlay planes
//
static void
initPerfMeter(SoGroup *overlayGroup)
{
    //
    // Create the overlay scene    
    //
    SoSeparator *perfMeter = new SoSeparator;
    perfMeter->ref();
    perfMeter->renderCaching = SoSeparator::OFF;

    // Add the text string to display the tris per second
    char str[150];
    SoFont *perfMeterFont = new SoFont;
    SoColorIndex *cind    = new SoColorIndex;
    cind->index = 3;
    perfMeterFont->size = 18;
    perfMeterFont->name = "Helvetica";
    perfMeter->addChild(perfMeterFont);
    perfMeter->addChild(cind);
    SoText2 *perfMeterText = new SoText2;
    SoTranslation *trans = new SoTranslation;
    trans->translation.setValue(-0.8, -0.95, 0.0);
    perfMeter->addChild(trans);
    perfMeterText->justification = SoText2::LEFT;
    perfMeter->addChild(perfMeterText);

    overlayGroup->addChild(perfMeter);

    // Update the performance meter every two seconds
    SoTimerSensor *perfSensor = 
		new SoTimerSensor(perfSensorCB, perfMeterText);
    perfSensor->setInterval(2.0);
    perfSensor->schedule();
}


////////////////////////////////////////////////////////////////////
//
// A sensor callback in the overlay scene updates the
// performance meter while the object is animating.
// 
static void
perfSensorCB(void *data, SoSensor *)
{
    SoText2 *perfText = (SoText2 *) data;
    static SbBool initialized = FALSE;
    static SbTime startTime; 
    static double lastFrames = 0;
    char str[100];
    
    if (initialized) {
	SbTime timeDiff = SbTime::getTimeOfDay() - startTime;
	double frames = 
	    numFramesRendered * 1000.0 / timeDiff.getMsecValue();
	frames = floor(frames * 100.0 + 0.5) / 100.0;

	// Update the perf meter display if the numbers have changed
	if (frames != lastFrames) {
	    sprintf (str, "%.2lf frames/sec", frames);
	    perfText->string.setValue(str);
	    lastFrames = frames;
	}
	        
	numFramesRendered = 0;
	startTime = SbTime::getTimeOfDay();
    }
    else {
	// Reset the timer when we detect the object has
	// started animating.
	numFramesRendered = 0;
	startTime = SbTime::getTimeOfDay();
	initialized = TRUE;
    }
}

////////////////////////////////////////////////////////////////////
//
// Add a callback node to the main scene which will count the number 
// of frames that have been rendered (to compute the tris/sec).
// Make sure the callback doesn't get cached or ruin the caching for
// the rest of the scene
//
static SoSeparator *
addUpdateCounter(SoNode *root)
{
    SoSeparator *newRoot = new SoSeparator;
    newRoot->ref();
    newRoot->renderCaching = SoSeparator::OFF;

    SoCallback *countUpdates = new SoCallback;
    countUpdates->setCallback(countUpdatesCB);
    newRoot->addChild(countUpdates);

    newRoot->addChild(root);

    return (newRoot);
}
	
static void
countUpdatesCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
        SoCacheElement::invalidate(action->getState());
	numFramesRendered++;
    }
}

#endif
