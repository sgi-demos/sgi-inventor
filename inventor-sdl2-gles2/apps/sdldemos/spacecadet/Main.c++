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
#define DEBUGGING 0
#define TIMING    0

#include <getopt.h>
#include <time.h>
#include <unistd.h>
#include <Inventor/SbTime.h>
#include <Inventor/SoDB.h>
#ifdef SDL_PORT
#include <SoSDL.h>
#include <SoSDLRenderArea.h>
// Source-compat aliases: the body of this file keeps SGI's names.
#define SoXt SoSDL
#define SoXtRenderArea SoSDLRenderArea
typedef void *Widget;
#else
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#endif
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/engines/SoOneShot.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/sensors/SoAlarmSensor.h>
#include <Inventor/sensors/SoOneShotSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include "Common.h"
#include "Blaster.h"
#include "Chamber.h"
#include "HighScore.h"
#include "Panel.h"
#include "Scene.h"
#include "Setup.h"
#include "Ship.h"
#include "Sound.h"
#include "Tunnel.h"
#include "XStuff.h"

#if DEBUGGING
#include <Inventor/actions/SoWriteAction.h>
#endif

static int		level;		// Current level
static Tunnel		*tunnel;	// Current tunnel
static SoTimerSensor	*sensor;	// Sensor that makes it all happen
static SbTime		lastTime;	// Time previous sensor went off
static SoAlarmSensor	*transitionSensor = NULL; // Sensor for transitions
static SoXtRenderArea	*view;		// Viewing area
static SoOneShotSensor	*texSensor;	// Used to time texture rendering
static SoNode		*mainRoot;	// Root of main graph
static SoNode		*overRoot;	// Root of overlay graph

// Command-line options
static char		*playerName;	// Name of player
static int		startLevel;	// Starting level
static long		randomSeed;	// Random number generator seed
static SbBool		printSeed;	// TRUE if seed should be printed
static SbBool		useSound;	// TRUE if sound should be on
static SbBool		airplaneMode;	// TRUE if mouse motion down goes up
static SbBool		darkMode;	// TRUE if lights are out
static SbBool		skipStartingSequence; // Skip initial chamber sequence

////////////////////////////////////////////////////////////////////////////
//
// Prints usage message.
//
////////////////////////////////////////////////////////////////////////////

static void
printUsage(const char *progName)
{
    fprintf(stderr, "Usage: %s [-cflnqsS]\n", progName);
    fprintf(stderr,
	    "\t-c      Skip initial animation sequence\n"
	    "\t-d      Fly in the dark - scores are doubled\n"
	    "\t-f      Use airplane-style motion (move mouse down to go up)\n"
	    "\t-h      Help - prints this message\n"
	    "\t-l N    Start at level N\n"
	    "\t-n name Set player name\n"
	    "\t-q      Don't play game sounds\n"
	    "\t-r N    Use N to seed the random number generator\n"
	    "\t-R      Print the seed used for the random number generator\n");
}

////////////////////////////////////////////////////////////////////////////
//
// Parses command line arguments, setting options.
//
////////////////////////////////////////////////////////////////////////////

static SbBool
parseArgs(int argc, char *argv[])
{
    SbBool	ok = TRUE;
    int		c;

    // Initialize options
    playerName	 = NULL;
    startLevel	 = 1;
    randomSeed	 = (long) time(NULL);
    useSound	 = TRUE;
    airplaneMode = FALSE;
    darkMode	 = FALSE;
    printSeed	 = FALSE;
    skipStartingSequence = FALSE;

    while ((c = getopt(argc, argv, "acdhl:n:qr:R")) != -1) {
	switch (c) {
	  case 'a':
	    airplaneMode = TRUE;
	    break;
	  case 'c':
	    skipStartingSequence = TRUE;
	    break;
	  case 'd':
	    darkMode = TRUE;
	    break;
	  case 'h':
	    ok = FALSE;	// Will cause usage to be printed
	    break;
	  case 'l':
	    startLevel = atoi(optarg);
	    break;
	  case 'n':
	    playerName = strdup(optarg);
	    break;
	  case 'q':
	    useSound = FALSE;
	    break;
	  case 'r':
	    randomSeed = atol(optarg);
	    break;
	  case 'R':
	    printSeed = TRUE;
	    break;
	  default:
	    ok = FALSE;
	    break;
	}
    }

    // Extra arguments? Error!
    if (optind < argc)
	ok = FALSE;

    return ok;
}

////////////////////////////////////////////////////////////////////////////
//
// Initiates interaction.
//
////////////////////////////////////////////////////////////////////////////

static void
initInteraction()
{
    lastTime = SbTime::getTimeOfDay();

    // Disable real-time clock updates (for speed)
    SoDB::setRealTimeInterval(0.0);

    sensor->schedule();

    XStuff::setCursor(XStuff::DIRECTION);

    SbVec2s center = view->getSize() / 2;

    // Move the cursor to the center of the window
    XStuff::movePointer(center);

    // Use the center as the cursor base
    Ship::setBaseCursor(center);

    // Inform the player
    Sound::playOnce("goSignal");
}

////////////////////////////////////////////////////////////////////////////
//
// Cleans up.
//
////////////////////////////////////////////////////////////////////////////

static void
cleanUp()
{
    // Stop the ship
    Ship::stop();
}

////////////////////////////////////////////////////////////////////////////
//
// Ends the game, showing the final display.
//
////////////////////////////////////////////////////////////////////////////

static void
endGame()
{
    // Stop the ship
    Ship::stop();

    // Unschedule timer sensor
    sensor->unschedule();

    // Play run-down sound
    Sound::playOnce("runDown");

    // Freeze for a second
    sleep(1);

    // Turn off tunnel graphics
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("SCMainSwitch");
    mainSwitch->whichChild = SO_SWITCH_NONE;

    // Get rid of overlay
    view->setOverlaySceneGraph(NULL);

    // Display score for a couple of seconds
    Panel::displayEnd();
    view->render();			// Force redraw
    view->renderOverlay();		// Force redraw
    sleep(2);
    Panel::displayNothing();

    // Display high score list
    Setup::displayHighScores();
    view->setSceneGraph(Setup::getGraph());
}

////////////////////////////////////////////////////////////////////////////
//
// Creates and installs a new tunnel for the current level.
//
////////////////////////////////////////////////////////////////////////////

static void
createTunnel()
{
    SoSeparator *tunSep = (SoSeparator *) Scene::find("SCTunnelSep");

    // Free up any old tunnel and remove its graph
    if (tunnel != NULL) {
	tunSep->removeChild(tunSep->getNumChildren() - 1);
	delete tunnel;
    }

    // Create a new tunnel and stick the tunnel graph in the correct place
    tunnel = new Tunnel(level);
    tunSep->addChild(tunnel->getGraph());
    Ship::setTunnel(tunnel);

    // Restore tunnel graphics
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("SCMainSwitch");
    mainSwitch->whichChild = SO_SWITCH_ALL;
}

#if DEBUGGING
////////////////////////////////////////////////////////////////////////////
//
// Sets up for given level. (For switching levels quickly.)
//
////////////////////////////////////////////////////////////////////////////

static void
setUpLevel(int newLevel)
{
    level = newLevel;

    // Unschedule timer sensor
    sensor->unschedule();

    createTunnel();

    Panel::reset();
    Panel::setLevel(level);

    // Wait for user to start
    XStuff::setCursor(XStuff::PAUSE);
}
#endif

////////////////////////////////////////////////////////////////////////////
//
// Completes change of level. Called from alarm sensor.
//
////////////////////////////////////////////////////////////////////////////

static void
endLevelChange(void *stagePtr, SoSensor *)
{
    int stage = (int) (long) stagePtr;   // SDL port: LP64-safe

    switch (stage) {

      case 0:	// Done with showing end-of-level scores
	{
	    // Turn off end-of-level scores
	    Panel::displayEndLevel(FALSE);

	    // Enter central chamber
	    SoSwitch *chamberSw = (SoSwitch *) Scene::find("SCChamberSwitch");
	    chamberSw->whichChild = SO_SWITCH_ALL;

	    // Activate chamber animation
	    Chamber::animateMain(level);

	    // Set up sensor for next stage
	    transitionSensor->setData((void *) 1);
	    transitionSensor->setTimeFromNow(Chamber::getMainAnimationTime());
	    transitionSensor->schedule();
	}
	break;

      case 1:	// Leaving central chamber into next tunnel
	{
	    createTunnel();

	    // Activate chamber door opening
	    Chamber::animateDoor();

	    // Set up sensor for next stage
	    transitionSensor->setData((void *) 2);
	    transitionSensor->setTimeFromNow(Chamber::getDoorAnimationTime());
	    transitionSensor->schedule();
	}
	break;

      case 2:	// Beginning flight through tunnel
	{
	    // Disable chamber graphics
	    SoSwitch *chamberSw = (SoSwitch *) Scene::find("SCChamberSwitch");
	    chamberSw->whichChild = SO_SWITCH_NONE;

	    // Restore panel display
	    Panel::reset();
	    Panel::setLevel(level);
	    Panel::display(TRUE);

	    // Start the level
	    initInteraction();
	}
	break;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Begins change of level.
//
////////////////////////////////////////////////////////////////////////////

static void
beginLevelChange(int newLevel, SbBool showScores)
{
    level = newLevel;

    // Unschedule timer sensor
    sensor->unschedule();

    // Re-enable real-time clock updates
    SoDB::setRealTimeInterval(SENSOR_INTERVAL);

    // Turn off tunnel graphics
    SoSwitch *mainSwitch = (SoSwitch *) Scene::find("SCMainSwitch");
    mainSwitch->whichChild = SO_SWITCH_NONE;

    Panel::display(FALSE);

    // Set up an alarm sensor to begin next level
    if (transitionSensor == NULL)
	transitionSensor = new SoAlarmSensor;

    transitionSensor->setFunction(endLevelChange);
    transitionSensor->setData((void *) 0);

    // Display end-of-level scores
    if (showScores) {
	Panel::displayEndLevel(TRUE);
	transitionSensor->setTimeFromNow(SbTime(3.0));
    }
    else
	transitionSensor->setTimeFromNow(SbTime(1.0));

    transitionSensor->schedule();
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for a keyboard button press or release.
//
////////////////////////////////////////////////////////////////////////////

static void
keyboardCB(void *, SoEventCallback *ecb)
{
    const SoKeyboardEvent *kbe = (const SoKeyboardEvent *) ecb->getEvent();

    if (SO_KEY_PRESS_EVENT(kbe, ANY)) {
	switch (kbe->getKey()) {

	  case SoKeyboardEvent::ESCAPE: // Quit
	  case SoKeyboardEvent::Q:	// Quit
	    cleanUp();
	    exit(0);

	  case SoKeyboardEvent::P:	// Pause
	    // Don't allow pausing while chamber sequence is animating
	    if (! transitionSensor->isScheduled()) {
		if (sensor->isScheduled()) {
		    sensor->unschedule();
		    XStuff::setCursor(XStuff::PAUSE);
		    Ship::stop();
		}
		else {
		    sensor->schedule();
		    lastTime = SbTime::getTimeOfDay();
		    XStuff::setCursor(XStuff::DIRECTION);
		    Ship::start();
		}
	    }
	    break;

	  case SoKeyboardEvent::R:	// Print random seed
	    printf("Random seed is %ld\n", randomSeed); 
	    break;

	  case SoKeyboardEvent::S:	// Toggle sound, if it was on
	    if (Setup::isSound())
		Sound::enable(! Sound::isEnabled());
	    break;

#if DEBUGGING
	  case SoKeyboardEvent::A:	// Abort (dump core)
	    abort();

	  case SoKeyboardEvent::B:	// Begin
	    initInteraction();
	    break;

	  case SoKeyboardEvent::D:	// Deplete energy
	    Panel::setEnergy(0.0);
	    break;

	  case SoKeyboardEvent::E:	// End current level
	    beginLevelChange(level + 1, FALSE);
	    break;

	  case SoKeyboardEvent::U:	// Up level
	    setUpLevel(level + 1);
	    break;

	  case SoKeyboardEvent::W:	// Write graph
	    {
		SoWriteAction wa;
		wa.getOutput()->openFile("debug.iv");
		wa.apply(view->getSceneGraph());
		wa.getOutput()->closeFile();
	    }
	    break;

	  case SoKeyboardEvent::NUMBER_1: // Single buffer
	    view->setDoubleBuffer(FALSE);
	    break;

	  case SoKeyboardEvent::NUMBER_2: // Double buffer
	    view->setDoubleBuffer(TRUE);
	    break;
#endif

	  default:
	    break;
	}
    }

    ecb->setHandled();
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for a mouse button press or release.
//
////////////////////////////////////////////////////////////////////////////

static void
buttonCB(void *, SoEventCallback *ecb)
{
    // Don't do anything if we're not running
    if (! sensor->isScheduled())
	return;

    const SoMouseButtonEvent *mbe =
	(const SoMouseButtonEvent *) ecb->getEvent();

    if (SO_MOUSE_PRESS_EVENT(mbe, ANY)) {
	switch (mbe->getButton()) {
	  case SoMouseButtonEvent::BUTTON1:
	    Ship::setState(Ship::ACCELERATE);
	    break;
	  case SoMouseButtonEvent::BUTTON2:
	    Ship::setState(Ship::DECELERATE);
	    break;
	  case SoMouseButtonEvent::BUTTON3:
	    XStuff::setCursor(XStuff::TARGET);
	    Ship::setFiring(TRUE);
	    break;
	  default:
	    break;
	}
    }
    else {
	switch (mbe->getButton()) {
	  case SoMouseButtonEvent::BUTTON1:
	    if (Ship::getState() == Ship::ACCELERATE)
		Ship::setState(Ship::COAST);
	    break;
	  case SoMouseButtonEvent::BUTTON2:
	    if (Ship::getState() == Ship::DECELERATE)
		Ship::setState(Ship::COAST);
	    break;
	  case SoMouseButtonEvent::BUTTON3:
	    Ship::setFiring(FALSE);
	    XStuff::setCursor(XStuff::DIRECTION);
	    break;
	  default:
	    break;
	}
    }

    ecb->setHandled();
}

////////////////////////////////////////////////////////////////////////////
//
// Callback for mouse motion.
//
////////////////////////////////////////////////////////////////////////////

static void
motionCB(void *, SoEventCallback *ecb)
{
    if (sensor->isScheduled()) {
	const SoLocation2Event *l = (const SoLocation2Event *) ecb->getEvent();
	Ship::setCursor(l->getPosition());
    }

    ecb->setHandled();
}

////////////////////////////////////////////////////////////////////////////
//
// Timer sensor callback.
//
////////////////////////////////////////////////////////////////////////////

static void
sensorCB(void *, SoSensor *)
{
    // See how long it has been since we last did anything
    SbTime curTime  = SbTime::getTimeOfDay();
    SbTime timeDiff = curTime - lastTime;
    lastTime = curTime;
    float	numSeconds = timeDiff.getValue();

    // Reset the cursor base to the center of the window, in case the
    // window's size changed. This is pretty cheap, and a hell of a
    // lot easier than trying to do this only when the window is resized.
    Ship::setBaseCursor(view->getSize() / 2);

    // Update the ship's position and all else. See if we reached the
    // end of the tunnel
    if (Ship::update(numSeconds))
	beginLevelChange(level + 1, TRUE);

    // Otherwise, add the time elapsed to the display and check for
    // zero energy
    else {
	Panel::addElapsedTime(timeDiff);

	if (Panel::getEnergy() <= 0.0)
	    endGame();
    }

#if TIMING
    // Print out timing information every 2 seconds
    static float accumTime = 0.0;
    static int accumFrames = 0;
    accumTime += numSeconds;
    accumFrames++;
    if (accumTime >= 2.0) {
	fprintf(stderr, "%6.4f frames per second\n",
		(float) accumFrames / accumTime);
	accumTime = 0.0;
	accumFrames = 0;
    }
#endif
}

////////////////////////////////////////////////////////////////////////////
//
// Builds and returns scene graph.
//
////////////////////////////////////////////////////////////////////////////

static SoNode *
buildGraph()
{
    SoNode *root = Scene::readFile("SpaceCadet.iv");
    root->ref();

    // The root is really a separator
    SoSeparator *rootSep = (SoSeparator *) root;

    // Set up event callback node
    SoEventCallback *ecb = (SoEventCallback *) Scene::find("SCECB");
    ecb->addEventCallback(SoKeyboardEvent::getClassTypeId(), keyboardCB);
    ecb->addEventCallback(SoLocation2Event::getClassTypeId(), motionCB);
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), buttonCB);

    // Insert ship graph (cameras and lights)
    SoGroup *shipGroup = (SoGroup *) Scene::find("SCShip");
    shipGroup->addChild(Ship::getGraph());

    // Append the panel display graph as the last child of the root
    rootSep->addChild(Panel::getGraph());

    // Insert the chamber graph in the correct place
    SoSwitch *chamberSw = (SoSwitch *) Scene::find("SCChamberSwitch");
    chamberSw->addChild(Chamber::getGraph());

    // And the same for the blaster graph
    SoSeparator *blasterSep = (SoSeparator *) Scene::find("SCBlasterSep");
    blasterSep->addChild(Ship::getBlaster()->getGraph());

    return root;
}

////////////////////////////////////////////////////////////////////////////
//
// Builds and returns overlay scene graph.
//
////////////////////////////////////////////////////////////////////////////

static SoNode *
buildOverlayGraph()
{
    SoNode *root = Scene::readFile("Overlay.iv");
    root->ref();

    return root;
}

////////////////////////////////////////////////////////////////////////////
//
// Sets up the color map for the overlay graph.
//
////////////////////////////////////////////////////////////////////////////

static void
setOverlayColors()
{
    // Find the overlay colors and copy them
    SoBaseColor	*baseColor = (SoBaseColor *) Scene::find("OverlayColors");

    // Find the overlay colors and set up the overlay color table in
    // the render area
    view->setOverlayColorMap(1, 3, baseColor->rgb.getValues(0));
}

////////////////////////////////////////////////////////////////////////////
//
// Callback that is called when the setup panel is exited. Initiates
// game play.
//
////////////////////////////////////////////////////////////////////////////

static void
startCB()
{
    static SbBool firstTime = TRUE;

    level = Setup::getStartLevel();

    // Initialize static classes the first time
    if (firstTime) {
	Sound::init();		// Must be done before Ship::init()
	Chamber::init(level);
	HighScore::init(HIGH_SCORE_FILE);
	Panel::init();
	Ship::init();

	// Build a scene graph
	mainRoot = buildGraph();
	mainRoot->ref();

	// And an overlay graph
	overRoot = buildOverlayGraph();
	overRoot->ref();

	// Set up the overlay colors
	setOverlayColors();

	sensor = new SoTimerSensor(sensorCB, NULL);
	sensor->setInterval(SENSOR_INTERVAL);

	tunnel = NULL;

	firstTime = FALSE;
    }

    view->setSceneGraph(mainRoot);
    view->setOverlaySceneGraph(overRoot);

    // Initialize panel display (the rest is done when the level is set)
    Panel::setScore(0);

    Sound::enable(Setup::isSound());
    Ship::setAirplaneMode(Setup::isAirplaneMode());
    Ship::setDarkMode(Setup::isDarkMode());
    Ship::start();

    // Flying in the dark gets you higher scores, since it's a lot harder
    if (Setup::isDarkMode())
	Panel::setScoreMult(2.0);

    if (skipStartingSequence) {
	createTunnel();
	Panel::reset();
	Panel::setLevel(level);
	Panel::display(TRUE);
	initInteraction();
    }
    else
	beginLevelChange(level, FALSE);
}

////////////////////////////////////////////////////////////////////////////
//
// This callback is called after the window is displayed and the first
// rendering has finished. It times texture rendering to see if it's
// fast enough for real-time play.
//
////////////////////////////////////////////////////////////////////////////

static void
timeTexturesCB(void *, SoSensor *)
{
    // Read and set up a scene graph with a small textured square
    view->setSceneGraph(Scene::readFile("TextureTime.iv"));

    // Scale the square so that it takes up about 200 x 200 pixels.
    // (The square by default takes up the entire window.)
    float scale = 200.0 / (float) view->getSize()[1];
    SoScale *texScale = (SoScale *) Scene::find("TextureTimeScale");
    texScale->scaleFactor.setValue(scale, scale, scale);

    // Draw the scene once to make sure everything is loaded
    view->render();

    // Draw the scene again and time the results
    SbTime startTime, endTime;
    startTime.setToTimeOfDay();
    view->render();
    endTime.setToTimeOfDay();

    // See if it takes too much time (more than 1/5 second). If so,
    // disable texturing.
    double elapsed = (endTime - startTime).getValue();
    if (elapsed > .2)
	Setup::setTextureEnabled(FALSE);

    // Install the correct scene graph and let it go
    view->setAutoRedraw(TRUE);
    view->setSceneGraph(Setup::getGraph());
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
	texSensor = new SoOneShotSensor(timeTexturesCB, NULL);
	texSensor->setPriority(1);
	texSensor->schedule();
    }
}

//--------------------------------------------------------------------------
//
// Class: SCRenderArea
//
// This class is derived from SoXtRenderArea and makes one change: it
// redefines windowCloseAction() to allow us to detect when the user
// closed the window through the window manager. When we detect this,
// we can make sure to stop the program, including any child processes
// playing sounds.
//
//--------------------------------------------------------------------------

#ifdef SDL_PORT
class SCRenderArea : public SoSDLRenderArea {
  public:
    SCRenderArea(Widget) : SoSDLRenderArea("Space Cadet", 900, 720) {
	setWindowCloseCallback(closeCB, NULL);
    }
  private:
    static void closeCB(void *, SoSDLRenderArea *) {
	cleanUp();
	exit(0);
    }
};
#else
class SCRenderArea : public SoXtRenderArea {
  public:
    SCRenderArea(Widget parent);
  private:
    virtual void	windowCloseAction();
};

SCRenderArea::SCRenderArea(Widget parent) : SoXtRenderArea(parent)
{
}

void
SCRenderArea::windowCloseAction()
{
    cleanUp();
    SoXtRenderArea::windowCloseAction();
}
#endif /* !SDL_PORT */

//--------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
//
// Mainline.
//
////////////////////////////////////////////////////////////////////////////

#ifdef __EMSCRIPTEN__
// Zero-arg main (same workaround as the other games).
int
main()
{
    int argc = 1;
    static char *argv[] = { (char *)"spacecadet", NULL };
#else
int
main(int argc, char **argv)
{
#endif
    // Parse arguments
    if (! parseArgs(argc, argv)) {
	printUsage(argv[0]);
	return 1;
    }
    
#ifdef SDL_PORT
    // Bundled substitute fonts for the scenes' Font nodes (see
    // data/fonts/README.txt).
    setenv("FL_FONT_PATH", "fonts", 0);
    SoSDL::init();
    Widget window = NULL;
#else
    Widget window = SoXt::init(argv[0]);
    if (window == NULL)
	return 1;
#endif
    
    // Disable real-time clock updates (for speed)
    SoDB::setRealTimeInterval(0.0);
    
    // Randomize random numbers
    srand48(randomSeed);
    if (printSeed)
	printf("Random seed is %ld\n", randomSeed); 
    
    // Set player name to login id if not specified by option
    if (playerName == NULL) {
#ifdef SDL_PORT
	const char *user = getenv("USER");
	playerName = strdup(user ? user : "cadet");
#else
	playerName = strdup(cuserid(NULL));
#endif
    }
    
    // Initialize setup panel
    Setup::init();
    Setup::setPlayerName(playerName);
    Setup::setSound(useSound);
    Setup::setAirplaneMode(airplaneMode);
    Setup::setDarkMode(darkMode);
    Setup::setStartLevel(startLevel);
    Setup::setStartCallback(startCB);
    
    // Initialize the viewing area
    view = new SCRenderArea(window);
#ifndef SDL_PORT
    view->setSize(SbVec2s(900, 720));
#endif
    view->setTitle("Space Cadet");
    view->setBackgroundColor(SbVec3f(0.0, 0.0, 0.0));
    view->setTransparencyType(SoGLRenderAction::BLEND);
    
    // The first thing we want to do is to figure out whether we can
    // use textures. We'll do this by drawing and timing a scene graph
    // containing a small textured polygon. We have to make sure the
    // window is mapped first, though, so we set up a callback node to
    // do that.
    SoCallback *mapCB = new SoCallback;
    mapCB->setCallback(waitForWindowCB, NULL);
    // Disable auto-redraw so that we render only when we get the
    // expose event for the window - this means we will render this
    // node only once.
    view->setAutoRedraw(FALSE);
    view->setSceneGraph(mapCB);

#ifdef SDL_PORT
    XStuff::setComponent(view);
    SoSDL::mainLoop();
#else
    view->show();
    SoXt::show(window);
    XStuff::setComponent(view);
    
    SoXt::mainLoop();
#endif
    return 0;
}
