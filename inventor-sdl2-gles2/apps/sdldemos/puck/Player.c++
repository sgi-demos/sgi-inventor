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
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.97 $
 |
 |   Description:
 |      Definition of the Player class
 | 
 |   Author(s): David Mott, Helga Thorvaldsdottir
 | ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef SDL_PORT
#include <SoSDL.h>
#define SoXt SoSDL
#else
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <Inventor/Xt/SoXt.h>
#endif
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoLists.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/engines/SoOneShot.h>
#include <Inventor/engines/SoInterpolate.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoBlinker.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoColorIndex.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/sensors/SoIdleSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>

#include "SoSocket.h"
#include "Player.h"
#include "Rink.h"
#include "msg.h"
#include "Sound.h"

static SbVec3f GunColor(1, 0.4, 0);
static SbVec3f GunSpecColor(0.5, 0.2, 0);
static SbVec3f IceColor(.90, .90, .90);
static SbVec3f Camera0(0, 50.08, 184.152);
static SbVec3f Camera1(0, 50.08, -184.152);

// Shot speed is distance per second
#define SHOT_SPEED 	   2*RINK_LENGTH
#define GUN_BARREL_LENGTH  8.0
#define GUN_BARREL_RADIUS  2.0 

typedef struct {
    SoSwitch	*sw;
    SoTransform	*xf;
    SbTime	downTime;   // What time the pillar should go down
    SbTime	duration;   // Amount of time pillar will be visible
} PillarInfo;

// 30 fps for the simulation
#define FRAME_RATE 1/30.0

// Number of seconds without scoring before crowd gets restless
// and starts chanting to fire up the players.
#define RESTLESS_CROWD_TIME 30.0

#ifdef SDL_PORT
//
// Render area subclass that hands every SDL event to the Player (the
// SDL equivalent of the XtAddEventHandler registration in the ctor).
//
class PuckRenderArea : public SoSDLRenderArea {
  public:
    PuckRenderArea(const char *title, int w, int h, Player *p)
	: SoSDLRenderArea(title, w, h), player(p) {}
    virtual void processEvent(const SDL_Event *e) {
	player->handleSDLEvent(e);
	SoSDLRenderArea::processEvent(e);
    }
  private:
    Player *player;
};
#endif

//////////////////////////////////////////////////////////////////////////////
//
// Constructor creates a socket, locates the server, and joins the game.
//
Player::Player(const char *serverMachine, Widget window, SbBool useSound)
//
//////////////////////////////////////////////////////////////////////////////
{
    int serverPortNum = SERVER_PORT;
    int playerPortNum = PLAYER_PORT;
    
    // Allow user to specify a different port number at run time
    char *portNumEnv = getenv("PUCK_SOCKET_PORT");
    if (portNumEnv != NULL) {
	serverPortNum = atoi(portNumEnv);
	playerPortNum = serverPortNum + 1;
    }
	
    // Create a socket
    socket = new SoSocket(playerPortNum, TRUE); // TRUE = non-blocking
    if (socket->getfd() == -1) {
	fprintf(stderr, "Cannot start game, sorry.\n");
	exit(0);
    }
    
    XtAppAddInput(
	SoXt::getAppContext(),
	socket->getfd(),
	(XtPointer) XtInputReadMask,
	Player::processSocketCB,
	(XtPointer) this);

    //??? can we tell if no server is running?
    SoSocket::getAddress(serverMachine, serverPortNum, &server_addr);
    
    // Initialize the sound effects
    Sound::init();
    Sound::enable(useSound);

    mainWindow = window;

#ifdef SDL_PORT
    // Initialize the render area; PuckRenderArea routes SDL events to
    // handleSDLEvent (the XtAddEventHandler equivalent).
    renderArea = new PuckRenderArea("Puck", 640, 512, this);
    renderArea->setBackgroundColor(IceColor);
    renderArea->setWindowCloseCallback(Player::leaveGameCB, this);
    raSize = renderArea->getSize();
#else
    // Initialize the render area 
    renderArea = new SoXtRenderArea(mainWindow, NULL, TRUE, FALSE, FALSE);
    renderArea->setTitle("Puck");
    renderArea->setBackgroundColor(IceColor);
    renderArea->setWindowCloseCallback(Player::leaveGameCB, this);
    renderArea->setSize(SbVec2s(640, 512));
    raSize = renderArea->getSize();

    // Initialize the event handler
    XtAddEventHandler(renderArea->getOverlayWidget(), 
		StructureNotifyMask|KeyPressMask|
		ButtonPressMask|ButtonReleaseMask|
		PointerMotionMask|ButtonMotionMask,
		FALSE, 
		(XtEventHandler) Player::handleEvents, 
		(XtPointer) this);
#endif

    // Display the start screen
    SoInput in;
    if (!in.openFile("data/startup.iv")) {
        fprintf(stderr, "can't open data/startup.iv");
        exit(0);
    }
    SoSeparator *startRoot = SoDB::readAll(&in);
    if (startRoot == NULL) {
        fprintf(stderr, "can't read file data/startup.iv");
        exit(0);
    }
    renderArea->setSceneGraph(startRoot);
    startScreen = TRUE;
#ifndef SDL_PORT
    renderArea->show();
#endif

    renderSensor = NULL;
    animatingPillarSensor = NULL;
    restlessCrowdSensor = new SoAlarmSensor(Player::restlessCrowdCB, this);

#ifndef SDL_PORT
    SoXt::show(mainWindow);
#endif
}


//////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
Player::~Player()
//
//////////////////////////////////////////////////////////////////////////////
{
    delete socket;
    delete renderArea;
    delete renderSensor;
    delete animatingPillarSensor;
    delete restlessCrowdSensor;
    firedShotSwitch->unref();
    camera->unref();
    puckLoc->unref();
    myGunRot->unref();
    myGunTrans->unref();
    otherGunRot->unref();
    otherGunTrans->unref();
}

//////////////////////////////////////////////////////////////////////////////
//
// Send a message to the server
//
void
Player::sendToServer(const Command &cmd)
//
//////////////////////////////////////////////////////////////////////////////
{
    socket->send((const void *) &cmd, cmd.size(), &server_addr);
}


//////////////////////////////////////////////////////////////////////////////
//
// Called when the socket should be read.
//
void
Player::processSocket()
//
//////////////////////////////////////////////////////////////////////////////
{
    Command *cmd = NULL;
    struct sockaddr_in addr;
    
    // Read the socket until it is drained of data
    SbBool gettingData = TRUE;
    
    while (gettingData) { 
	if ((cmd = (Command *) socket->get(&addr)) == NULL) {
	    // No data on socket - drop back to Xt main loop
	    gettingData = FALSE;
	}
	else { 
	    switch(cmd->command) {
		case MSG_GAMEFULL:
		    printf("sorry, game in progress.\n");
		    exit(0);
		    break;
		case MSG_PLAYER_LEFT:
		    printf("other player left the game!\n");
		    break;
		case MSG_STARTGAME:
		    printf("start the game!\n");
		    initGame((StartGame *) cmd);
		    break;
		case MSG_PLAYERID:
		    id = ((PlayerId *) cmd)->id;
		    printf("we can play! player id is %d\n", id);
		    break;
		case MSG_UPDATESTATE:
		    updateState((RenderState *) cmd);
		    break;
		case MSG_MOVE_OTHER_GUN:
		    otherGunRot->angle.setValue(((OtherGun *)cmd)->rotAngle);
		    otherGunTrans->translation.setValue(
					 ((OtherGun *)cmd)->translation);
		    break;
		case MSG_DROP_PUCK:
		    dropPuck((DropPuck *) cmd);
		    break;
		case MSG_GOAL_SCORED:
		    scoreGoal((GoalScored *)cmd);
		    break;
		case MSG_GAME_OVER:
		    gameOver((GameOver *)cmd);
		    break;  
		case MSG_PILLAR_UP:
		    animatePillarStart(
			((PillarUp *)cmd)->whichPillar, 
			((PillarUp *)cmd)->downTime);
		    break;
		case MSG_COLLISION:
		    collisionFeedback((CollisionState *) cmd);
		    break;
		case MSG_NEW_GAME_REQUEST:
		    printf("other player requested a new game\n");
		    break;
		default: 
		    printf("unknown msg received: %d\n", cmd);
		    break;
	    } // switch on command
	} // else got a command
    } // while getting data
}

//////////////////////////////////////////////////////////////////////////////
//
// Set the height of a pillar
//
void
Player::setPillarHeight(int whichPillar, float height)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Scale the pillar to the desired height
    PillarInfo *info = (PillarInfo *) pillarInfo[whichPillar];
    info->xf->scaleFactor.setValue(1, height, 1);
    
    // Translate so the base of the pillar is at ice level
    const SbVec3f &v = info->xf->translation.getValue();
    info->xf->translation.setValue(v[0], height/2.0, v[2]);
}

//////////////////////////////////////////////////////////////////////////////
//
// Get the height of a pillar
//
float
Player::getPillarHeight(int whichPillar)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Scale the pillar to the desired height
    PillarInfo *info = (PillarInfo *) pillarInfo[whichPillar];
    const SbVec3f &v = info->xf->scaleFactor.getValue();
    
    return v[1];
}

//////////////////////////////////////////////////////////////////////////////
//
// Static callback, called by Xt when data is received on the socket
//
void
Player::processSocketCB(XtPointer userData, int *, XtInputId *)
//
//////////////////////////////////////////////////////////////////////////////
{
    Player *player = (Player *) userData;
    player->processSocket();
}

//////////////////////////////////////////////////////////////////////////////
//
// Called when the game starts.
//
void
Player::initGame(StartGame *cmd)
//
//////////////////////////////////////////////////////////////////////////////
{
    static SbBool initialized = FALSE;

    // The following need to be initialized for every game

    newGameRequested = FALSE;
    newState = FALSE;
    noPuck = TRUE;
    numPlayers = cmd->numPlayers;
    updateScoreboard = TRUE;

    if (initialized) {
	// Reset the score
        SoText2 *score = (SoText2 *) SoNode::getByName("Team0Score");
	assert (score != NULL);
	score->string.setValue("0");

        score = (SoText2 *) SoNode::getByName("Team1Score");
	assert (score != NULL);
	score->string.setValue("0");

        Sound::playOnce(Sound::CHANT);
    }
    else {
	// The following need only be initialized once
    
	// we will do our own redraws during idle time
	renderArea->setAutoRedraw(FALSE);

	// Remove the start scene graph and initialize the game scene graph
        if (renderArea->getSceneGraph() != NULL)
	    renderArea->getSceneGraph()->unref(); 
	initGraph();

        moveGunUpDown = FALSE;
        moveGunLeft = FALSE;
        moveGunRight = FALSE;

        // Initialize projector used to determine where the cursor is on the ice
        iceProjector.setPlane(SbPlane(SbVec3f(0,1,0), 0));

	// Play the initial sound
	Sound::playOnce(Sound::CHANT);
    
	// Initialize the render sensor
	renderSensor = new SoIdleSensor(Player::renderCB, this);
	renderSensor->schedule();
	shotFired = FALSE;
    
	// Intialize the sensor that animates pillars going 
	// up and down on the ice
	animatingPillarSensor = 
		new SoTimerSensor(Player::animatePillarCB, this);
	animatingPillarSensor->setInterval(FRAME_RATE);
    
	initialized = TRUE;
    }

    // Initialize the names on the scoreboard.
    // We do this every time because the other player can change from
    // game to game.
    SoText2 *Team0 = (SoText2 *) SoNode::getByName("Team0");
    if (Team0 != NULL)	
	Team0->string.setValue(cmd->names[0]);

    SoText2 *Team1 = (SoText2 *) SoNode::getByName("Team1");
    if (Team1 != NULL)
	Team1->string.setValue(cmd->names[1]);

    // Make sure the victory puck from the last game isn't still visible
    SoSwitch *victorySwitch = (SoSwitch *) SoNode::getByName("victoryStages");
    assert (victorySwitch != NULL);
    victorySwitch->whichChild = -1;
	
    // Initialize time stamps
    simulationTimestamp = cmd->timestamp;
    renderTimestamp = SbTime::getTimeOfDay();
}

//////////////////////////////////////////////////////////////////////////////
//
// Creates the main scene graph.
// Called when the game is initialized.
//
void
Player::initGraph() 
// 
////////////////////////////////////////////////////////////////////////////// 
{ 
    // Create the scene root node 
    SoSeparator *root = new SoSeparator; 
    root->ref(); 
    root->enableNotify(FALSE);
    renderArea->setSceneGraph(root); 
    
    // Add the camera and a light
    // A handle to the camera is stored in the private data so we
    // don't have to look it up by name all the time.
    SbRotation cameraOrientation;
    camera = new SoPerspectiveCamera;
    camera->nearDistance = 100.;
    camera->farDistance = 500.;
    camera->heightAngle = 0.349066;
    
    if (id == 0) {
        camera->position.setValue(Camera0); 
        cameraOrientation = SbRotation(SbVec3f(-1,0,0), 0.314159);
    }
    else {
        camera->position.setValue(Camera1); 
        cameraOrientation = SbRotation(SbVec3f(-1,0,0), 0.314159) *
        			SbRotation(SbVec3f(0,1,0), M_PI); 
    } 
    camera->orientation.setValue(cameraOrientation); 
    root->addChild(camera);

    // Set the view volume of the ice projector to reflect the camera's volume
    updateProjector();

    // Read the arena geometry from a file and add to the scene
    SoInput in;
    if (!in.openFile("data/arena.iv")) {
        fprintf(stderr, "can't open data/arena.iv"); 
        exit(0);
    }
    SoSeparator *arena = SoDB::readAll(&in);
    if (arena == NULL) {
        fprintf(stderr, "can't read file data/arena.iv"); 
	exit(0);
    }
    root->addChild(arena);

    // Position the goals
    // The default position is for player 0 => need to rotate it for player 1
    if (id == 1) {
	SoRotationXYZ *oppGoalRotation = 
			(SoRotationXYZ *) SoNode::getByName("oppGoalRotation");
	assert(oppGoalRotation != NULL);
	oppGoalRotation->angle = M_PI;

	SoRotationXYZ *myGoalRotation = 
			(SoRotationXYZ *) SoNode::getByName("myGoalRotation");
	assert(myGoalRotation != NULL);
	myGoalRotation->angle = M_PI;
    }

    // Initialize the puck values
    initPuck();

    // Add the energy gun for both players
    root->addChild((SoNode *)initGun(0));
    root->addChild((SoNode *)initGun(1));
    
    // Add a group that will contain all the shots.
    root->addChild((SoNode *)initShotGroup());

    // Find all the pillars
    initPillars();
    
    // Read in the scoreboard geometry and add it to the overlay scene
    if (!in.openFile("data/scoreboard.iv")) {
        fprintf(stderr, "can't open data/scoreboard.iv"); 
        exit(0);
    }
    SoSeparator *scoreboard = SoDB::readAll(&in);
    if (scoreboard == NULL) {
        fprintf(stderr, "can't read file data/scoreboard.iv"); 
	exit(0);
    }
    scoreboard->ref();
    SoSeparator *overlayRoot = 
		(SoSeparator *) renderArea->getOverlaySceneGraph();

    if (overlayRoot == NULL)
	renderArea->setOverlaySceneGraph(scoreboard);
    else
	overlayRoot->insertChild(scoreboard, 0);
}


/////////////////////////////////////////////////////////////////////////////
//
// Init pillar data pointers so we search for them once.
//
void
Player::initPillars()
//
/////////////////////////////////////////////////////////////////////////////
{
    SoSwitch *pillarLight = (SoSwitch *)SoNode::getByName("pillarLight");
    assert (pillarLight != NULL);
    pillarLight->whichChild = id;

    SoSeparator *pillarRoot = (SoSeparator *)SoNode::getByName("pillars");
    assert (pillarRoot != NULL);

    for (int i = 0; i < NUM_PILLARS; i++) {
	PillarInfo *info = new PillarInfo;
	
	// Layout of the graph is in data/pillars.iv
	info->sw = (SoSwitch *) pillarRoot->getChild(i);
	SoSeparator *sep = (SoSeparator *) info->sw->getChild(0);
	info->xf = (SoTransform *) sep->getChild(0);
	
	// Add the on/off switch and the transform to our pillar lists
	pillarInfo.append(info);
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// Creates and positions the gun.
// Called when the scene is initialized.
//
SoSeparator *
Player::initGun(int whichPlayer)
//
/////////////////////////////////////////////////////////////////////////////
{
    // Initialize the values for gun direction and position (in rink space).
    // These will be sent to the server every time a shot is fired.
    if (whichPlayer == id) {
 	if (id == 0) 
	    myGunDirection.setValue(0., 1.0);
        else 
            myGunDirection.setValue(0., -1.);
    }
    
    SbVec3f gunPosition;
    if (whichPlayer == 0) 
        gunPosition.setValue(0., GUN_BARREL_RADIUS, RINK_Y);
    else 
        gunPosition.setValue(0., GUN_BARREL_RADIUS, -RINK_Y);
    if (whichPlayer == id) 
	myGunPosition = gunPosition;

    // Create all the nodes needed for the gun
    SoSeparator *energyGun = new SoSeparator;
    SoDirectionalLight *gunLgt = new SoDirectionalLight;
    SoLightModel *gunLgtModel = new SoLightModel;
    SoMaterial *gunMtl = new SoMaterial;
    SoShapeHints *shapeHints = new SoShapeHints;
    SoComplexity *complexity = new SoComplexity;
    SoTranslation *gunBarrelTrans = new SoTranslation;
    SoRotationXYZ *gunInitialRot0 = new SoRotationXYZ;
    SoCone *gunBarrel = new SoCone;
    
    // Pointers to the gun translation and rotation nodes are stored in the 
    // object private data so we don't have to search for them every update.
    SoTranslation *gunTranslation;
    SoRotationXYZ *gunRotation;
    if (id == whichPlayer) {
	myGunTrans = new SoTranslation;
        myGunRot = new SoRotationXYZ;
	myGunTrans->ref();   
    	myGunRot->ref();
	gunTranslation = myGunTrans;
	gunRotation = myGunRot;
    } else {
	otherGunTrans = new SoTranslation;
        otherGunRot = new SoRotationXYZ;
	otherGunTrans->ref();   
    	otherGunRot->ref();
	gunTranslation = otherGunTrans;
	gunRotation = otherGunRot;
    }

    // Initialize all the node values for the gun
    gunLgtModel->model = SoLightModel::PHONG;
    SbVec3f lightDir;
    if (id == 0)
    	gunLgt->direction.setValue(0, -1, -.25);
    else
    	gunLgt->direction.setValue(0, -1, .25);
    gunMtl->diffuseColor.setValue(GunColor);
    gunMtl->ambientColor.setValue(GunColor);
    gunMtl->specularColor.setValue(GunSpecColor);
    shapeHints->shapeType = SoShapeHints::SOLID;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    complexity->value =  0.2;

    gunTranslation->translation.setValue(gunPosition);
    gunRotation->axis = SoRotationXYZ::Y;
    if (whichPlayer == 0)
        gunBarrelTrans->translation.setValue(0,0,-GUN_BARREL_LENGTH/2.);
    else
        gunBarrelTrans->translation.setValue(0,0,GUN_BARREL_LENGTH/2.);
    gunInitialRot0->axis = SoRotationXYZ::X;
    gunInitialRot0->angle = -M_PI_2;
    gunBarrel->bottomRadius = GUN_BARREL_RADIUS;
    gunBarrel->height = GUN_BARREL_LENGTH;

    // Collect all the nodes that make up the gun under one separator.
    energyGun->addChild(gunLgt);
    energyGun->addChild(gunLgtModel);
    energyGun->addChild(gunMtl);
    energyGun->addChild(shapeHints);
    energyGun->addChild(complexity);
    energyGun->addChild(gunTranslation);
    energyGun->addChild(gunRotation);
    energyGun->addChild(gunBarrelTrans);
    if (whichPlayer == 1) {
    	SoRotationXYZ *gunInitialRot1 = new SoRotationXYZ;
    	gunInitialRot1->axis = SoRotationXYZ::Y;
    	gunInitialRot1->angle = M_PI;
    	energyGun->addChild(gunInitialRot1);
    }
    energyGun->addChild(gunInitialRot0);
    energyGun->addChild(gunBarrel);
    
    return(energyGun);
}

//////////////////////////////////////////////////////////////////////////////
//
// Creates the puck.
// Called when the scene is initialized.
//
void
Player::initPuck()
//
//////////////////////////////////////////////////////////////////////////////
{
    // Get a pointer to the puck scene graph
    SoSeparator *puck = (SoSeparator *) SoNode::getByName("thePuck");
    assert(puck != NULL);

    // Set up the lighting, depending on which player we are
    SoSwitch *puckLight = (SoSwitch *) SoNode::getByName("puckLight");
    assert(puckLight != NULL);
    puckLight->whichChild = id;

    SoSwitch *puckShadowXform = 
		(SoSwitch *) SoNode::getByName("puckShadowXform");
    assert(puckShadowXform != NULL);
    puckShadowXform->whichChild = id;

    SoSwitch *pillarShadowXform = 
		(SoSwitch *) SoNode::getByName("pillarShadowXform");
    assert(pillarShadowXform != NULL);
    pillarShadowXform->whichChild = id;


    // Set up the puck size and initial location
    // A pointer to the puck location is stored with the player private
    // data so we don't have to look it up every update.
    puckLoc = (SoTranslation *) SoNode::getByName("puckLoc");
    assert(puckLoc != NULL);
    puckLoc->ref();
    puckLoc->translation.setValue(0, PUCK_HEIGHT/2, 0);

    SoCylinder *puckGeom = (SoCylinder *) SoNode::getByName("puckGeom");
    assert(puckGeom != NULL);
    puckGeom->radius.setValue(PUCK_RADIUS);
    puckGeom->height.setValue(PUCK_HEIGHT);
  
    SoCylinder *puckShadow = (SoCylinder *) SoNode::getByName("puckShadow");
    assert(puckShadow != NULL);
    puckShadow->radius.setValue(PUCK_RADIUS);
    puckShadow->height.setValue(PUCK_HEIGHT);
}

//////////////////////////////////////////////////////////////////////////////
//
// Creates a group that will contain all the shots.
// All the nodes we might need are created, and later they are instanced
// appropriately when we're notified of a state update from the server.
// Called when the scene is initialized.
//
SoSeparator *
Player::initShotGroup()
//
//////////////////////////////////////////////////////////////////////////////
{
    SoSeparator *shotRoot = new SoSeparator;

    SoDirectionalLight *shotLgt = new SoDirectionalLight;
    SoLightModel *shotLgtModel = new SoLightModel;
    SoMaterial *shotColor = new SoMaterial;
    shots = new SoSeparator;
    shots->setName("shots");

    shotLgt->direction.setValue(0, -1, 0);
    shotLgtModel->model = SoLightModel::PHONG;
    shotColor->diffuseColor.setValue(GunColor);

    shotRoot->addChild(shotLgt);
    shotRoot->addChild(shotLgtModel);
    shotRoot->addChild(shotColor);
    shotRoot->addChild(shots);

    // Create the geometry for one shot.
    SoGroup *oneShot = new SoGroup;
    SoComplexity *shotCmplx = new SoComplexity;
    SoShapeHints *shotHints = new SoShapeHints;
    SoScale *shotScale = new SoScale;
    SoSphere *shotGeom = new SoSphere;

    shotCmplx->value = 0.2;
    shotHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shotHints->shapeType = SoShapeHints::SOLID;
    shotScale->scaleFactor.setValue(1,1,10);
    shotGeom->radius = .2;

    oneShot->addChild(shotHints);
    oneShot->addChild(shotCmplx);
    oneShot->addChild(shotScale);
    oneShot->addChild(shotGeom);

    // Create all nodes we'll need for all possible shots with 
    // directions and positions.  We can instance the shot geometry,
    // but the translations and rotations must be unique.
    for (int i=0; i<2; i++) {
	for (int j=0; j<MAXSHOTS; j++) {
	    shotGroup[i][j] = new SoSeparator;	
	    shotLoc[i][j] = new SoTranslation;	
	    shotDir[i][j] = new SoRotationXYZ;	
	    shotDir[i][j]->axis = SoRotationXYZ::Y;
	    shotGroup[i][j]->addChild(shotLoc[i][j]);
	    shotGroup[i][j]->addChild(shotDir[i][j]);
	    shotGroup[i][j]->addChild(oneShot);
	    shotGroup[i][j]->ref();
	}
    }

    // Create the node for the shot that's visible when the gun is fired
    SoSeparator *firedShot = new SoSeparator;
    SoTranslation *firedShotLoc = new SoTranslation;
    firedShotLoc->setName("firedShotLoc");
    SoRotationXYZ *firedShotDir = new SoRotationXYZ;
    firedShotDir->setName("firedShotRot");
  
    // the switch is stored in the player private data so we don't have
    // to look it up by name very time.
    firedShotSwitch = new SoSwitch;
    firedShotSwitch->ref();

    firedShotDir->axis = SoRotationXYZ::Y;
    firedShotSwitch->whichChild = -1;

    firedShot->addChild(shotColor);
    firedShot->addChild(firedShotLoc);
    firedShot->addChild(firedShotDir);
    firedShotSwitch->addChild(oneShot);
    firedShot->addChild(firedShotSwitch);
    shotRoot->addChild(firedShot);

    return (shotRoot);
}

#ifndef SDL_PORT
void 
Player::handleEvents(Widget, XtPointer userData, XAnyEvent *xe, Boolean *)
{
    Player *player = (Player *) userData;

    // We're still in the start screen.
    if (player->startScreen) {

	if (xe->type == ButtonPress) {
	    // See if the start button was picked.  
 	    XButtonEvent *ev = (XButtonEvent *)xe;
	    SoRayPickAction pa(player->renderArea->getViewportRegion());
	    pa.setPoint(SbVec2s(ev->x, player->raSize[1]-ev->y));
    	    pa.apply((SoNode *)player->renderArea->getSceneGraph());
       	    SoPickedPoint *pp = pa.getPickedPoint();

            if (pp != NULL) {
		// The start button is the only pickable object, so it
		// must have been the one picked.
		char *name = cuserid(NULL);
		player->sendToServer(JoinGame(name));
		player->startScreen = FALSE;
	    }
	}

	else if (xe->type == ConfigureNotify) {
    	    player->raSize = player->renderArea->getSize();
	}

	return;
    }
    
    // We're playing the game
    switch (xe->type) {

	case ConfigureNotify:
    	    player->raSize = player->renderArea->getSize();
	    player->updateProjector();
	    break;

	case KeyPress:
	    player->handleKeyPress((XKeyPressedEvent *)xe);
	    break;

	case MotionNotify:
	    player->handleMouseMotion((XPointerMovedEvent *)xe);
	    break;

   	case ButtonPress:
	    player->handleButtonPress((XButtonEvent *)xe);
	    break;

   	case ButtonRelease:
	    player->handleButtonRelease((XButtonEvent *)xe);
	    break;

	default:
	    break;
    }
}
#else /* SDL_PORT */

//////////////////////////////////////////////////////////////////////////////
//
// SDL events, translated onto the original X-typed handlers. Mirrors
// handleEvents() above.
//
void
Player::handleSDLEvent(const SDL_Event *e)
{
    // We're still in the start screen.
    if (startScreen) {
	if (e->type == SDL_MOUSEBUTTONDOWN) {
	    // See if the start button was picked.
	    SoRayPickAction pa(renderArea->getViewportRegion());
	    pa.setPoint(SbVec2s(e->button.x, raSize[1] - e->button.y));
	    pa.apply((SoNode *) renderArea->getSceneGraph());
	    SoPickedPoint *pp = pa.getPickedPoint();

	    if (pp != NULL) {
		// The start button is the only pickable object, so it
		// must have been the one picked.
		const char *name = getenv("USER");
		sendToServer(JoinGame((char *)(name ? name : "player")));
		startScreen = FALSE;
	    }
	}
	else if (e->type == SDL_WINDOWEVENT &&
		 e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
	    raSize = renderArea->getSize();
	}
	return;
    }

    // We're playing the game
    switch (e->type) {

	case SDL_WINDOWEVENT:
	    if (e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
		raSize = renderArea->getSize();
		updateProjector();
	    }
	    break;

	case SDL_KEYDOWN: {
	    XKeyPressedEvent kev;
	    kev.keysym = e->key.keysym.sym;
	    handleKeyPress(&kev);
	    break;
	}

	case SDL_MOUSEMOTION: {
	    XPointerMovedEvent mev;
	    mev.x = e->motion.x;
	    mev.y = e->motion.y;
	    handleMouseMotion(&mev);
	    break;
	}

	case SDL_MOUSEBUTTONDOWN: {
	    XButtonEvent bev;
	    bev.x = e->button.x;
	    bev.y = e->button.y;
	    bev.button = e->button.button;  // SDL 1/2/3 == Button1/2/3
	    handleButtonPress(&bev);
	    break;
	}

	case SDL_MOUSEBUTTONUP: {
	    XButtonEvent bev;
	    bev.x = e->button.x;
	    bev.y = e->button.y;
	    bev.button = e->button.button;
	    handleButtonRelease(&bev);
	    break;
	}

	default:
	    break;
    }
}
#endif /* SDL_PORT */

//////////////////////////////////////////////////////////////////////////////
//
// Static callback, called when the window is closed, i.e the
// player quits the game.
//
void
Player::leaveGameCB(void *userData, SoXtComponent *)
//
//////////////////////////////////////////////////////////////////////////////
{
    Player *player = (Player *) userData;

    player->sendToServer(LeaveGame(player->id));
    exit (0);
}


//////////////////////////////////////////////////////////////////////////////
//
// Static callback, called when a key is pressed.
//
void
Player::handleKeyPress(XKeyPressedEvent *event)
//
//////////////////////////////////////////////////////////////////////////////
{
#ifdef SDL_PORT
    KeySym keysym = event->keysym;
#else
    char buf[100];
    int rv;
    KeySym keysym;

    rv = XLookupString(event, buf, sizeof(buf), &keysym, 0);
#endif
    switch (keysym) {
	case XK_space:
            if (!noPuck) 
	        fireShot();
	    break;

    	case XK_g:
    	case XK_G:
	    // Ask for a new game
	    sendToServer(NewGameRequest(id));
	    printf("you requested a new game\n");
	    break;

	case XK_1:
	    // Set the game level to beginner
	    sendToServer(GameLevel(1));
    	    printf("game level set to: Beginner (slow)\n");
	    break;

	case XK_2:
	    // Set the game level to advanced
	    sendToServer(GameLevel(2));
	    printf("game level set to: Advanced (fast)\n");
	    break;

	case XK_0:
	    // Set the game level to practice
	    sendToServer(GameLevel(0));
	    printf("game level set to: Pracice (very slow)\n");
	    break;

	case XK_h:
	case XK_H:
	    // Print out a help message
	    Player::printHelp();
	    break;

	default:
	    break;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void
Player::fireShot()
{
    float fromX, fromY;
    float dirX, dirY;

    dirX = myGunDirection[0] * SHOT_SPEED;
    dirY = myGunDirection[1] * SHOT_SPEED;
    fromX = myGunPosition[0] + myGunDirection[0] * GUN_BARREL_LENGTH;
    fromY = -myGunPosition[2] + myGunDirection[1] * GUN_BARREL_LENGTH;

    shotFired = TRUE;

    // Figure out when the shot fired relative to when
    // the simulation was rendered, then add this to when
    // the simulation took place for an accurate timestamp.
    SbTime timeOfShot = SbTime::getTimeOfDay() - renderTimestamp;
    timeOfShot += simulationTimestamp;
    sendToServer(Shot(id, fromX, fromY, dirX, dirY, timeOfShot));
}


//////////////////////////////////////////////////////////////////////////////
//
// Called when the mouse is moved.
// Updates the gun position.
//
void
Player::handleMouseMotion(XPointerMovedEvent *ev)
//
//////////////////////////////////////////////////////////////////////////////
{
    SbVec2f mousePos;
    static SbBool definedCursor = FALSE;
 
    // Define crosshair cursor
    if (!definedCursor) {
#ifdef SDL_PORT
	SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR));
#else
	static Cursor crosshairCursor = 
		XCreateFontCursor(XtDisplay(mainWindow), XC_crosshair);
	XDefineCursor(XtDisplay(mainWindow), 
			XtWindow(mainWindow), 
			crosshairCursor);	
#endif
	definedCursor = TRUE;
    }


    // Determine where the cursor is.  
    // That's where we should aim the gun.
    
    // First normalize the mouse position
    if (raSize[0] == 0)
        mousePos[0] = 0.0;
    else
        mousePos[0] = 1.0 - ((float) (raSize[0] - ev->x)) / (float) raSize[0];

    if (raSize[1] == 0)
        mousePos[1] = 0.0;
    else
        mousePos[1] = ((float) (raSize[1] - ev->y)) / (float) raSize[1];

    // Then project the normalized mouse pos onto the ice surface
    // and determine the direction the gun is pointing
    SbVec3f icePos = iceProjector.project(mousePos);
    SbVec2f iceDir;
    iceDir[0] = icePos[0] - myGunPosition[0];
    iceDir[1] = icePos[2] - myGunPosition[2];
    iceDir.normalize();

    myGunDirection[0] = iceDir[0];
    myGunDirection[1] = -iceDir[1];

    // Move the gun forward/backward if the middle mouse button is down
    if (moveGunUpDown) {
	float newGunPos;
	float delta = 1.75 * RINK_Y * (mousePos[1] - prevMousePos[1]);

	if (id == 0) {
	    newGunPos = myGunPosition[2] - delta;
	    if (newGunPos < -RINK_Y/3)
		newGunPos = -RINK_Y/3;
	    else if (newGunPos > RINK_Y)
		newGunPos = RINK_Y;

	} else {    // id == 1
	    newGunPos = myGunPosition[2] + delta;
	    if (newGunPos < -RINK_Y)
		newGunPos = -RINK_Y;
	    else if (newGunPos >     RINK_Y/3)
		newGunPos = RINK_Y/3;
	}

	if (newGunPos != myGunPosition[2]) {
	    SbVec3f newCameraPos = camera->position.getValue();
	    newCameraPos[2] += newGunPos - myGunPosition[2];
	    camera->position.setValue(newCameraPos);
	    myGunPosition[2] = newGunPos;
	    myGunTrans->translation.setValue(myGunPosition);
	    updateProjector();
	}	
    }

    prevMousePos = mousePos;
}


//////////////////////////////////////////////////////////////////////////////
//
// Called when mouse button press events are detected.
//
void
Player::handleButtonPress(XButtonEvent *event)
//
//////////////////////////////////////////////////////////////////////////////
{

    switch (event->button) {
	case Button1:
	    moveGunLeft = TRUE;
	    moveLeftCount = 0;
	    break;

	case Button2:
	    moveGunUpDown = TRUE;
	    break;

	case Button3:
	    moveGunRight = TRUE;
	    moveRightCount = 0;
	    break;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Called when mouse button release events are detected.
//
void
Player::handleButtonRelease(XButtonEvent *event)
//
//////////////////////////////////////////////////////////////////////////////
{

    switch (event->button) {
	case Button1:
	    moveGunLeft = FALSE;
	    break;

	case Button2:
	    moveGunUpDown = FALSE;
	    break;

	case Button3:
	    moveGunRight = FALSE;
	    break;
    }
}



//////////////////////////////////////////////////////////////////////////////
//
void
Player::dropPuck(DropPuck *puck)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Bell sounds when puck is dropped
    Sound::playOnce(Sound::BELL);

    // Crowd will get restless if no one scores...
    restlessCrowdSensor->setTimeFromNow(RESTLESS_CROWD_TIME);
    restlessCrowdSensor->schedule();
    	
    // Reinitialize time stamps
    simulationTimestamp = puck->timestamp;
    renderTimestamp = puck->timestamp;

    latestState.puckLoc = puck->where;
    newState = TRUE;
    noPuck = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//
// Update the score.
// Called when we get notified of a goal
//
void
Player::scoreGoal(GoalScored *goal)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Crowd will be cheering, so turn off the restless crowd noise
    restlessCrowdSensor->unschedule();

    // Update the scoreboard
    SoText2 *scoreText;
    if (goal->whoScored == 0) 
        scoreText = (SoText2 *) SoNode::getByName("Team0Score");
    else
        scoreText = (SoText2 *) SoNode::getByName("Team1Score");
    assert(scoreText != NULL);
    char score[100];
    sprintf(score, "%d", goal->score);
    scoreText->string.setValue(score);
	
    // Did we score, or was it the other team?
    if (goal->whoScored == id) {
	// Flash the goal light
        SoOneShot *goalTrigger = 
			(SoOneShot *) SoEngine::getByName("GoalTrigger");
	assert(goalTrigger != NULL);
        goalTrigger->trigger.touch();

	// Play cheering sounds
	Sound::playOnce(Sound::CHEER);

    } else {
	// Play booing sounds
	Sound::playOnce(Sound::BOO);
    }

    // Make the puck disappear
    noPuck = TRUE;

    // Remove shots from play
    shots->removeAllChildren();
    latestState.numShots[0] = 0;
    latestState.numShots[1] = 0;

    newState = TRUE;
    updateScoreboard = TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
//
void
Player::restlessCrowd()
//
//////////////////////////////////////////////////////////////////////////////
{
    Sound::playOnce(Sound::CHANT);
    
    // They will wait a little longer before getting restless next time
    restlessCrowdSensor->setTimeFromNow(2.0*RESTLESS_CROWD_TIME);
    restlessCrowdSensor->schedule();
}

//////////////////////////////////////////////////////////////////////////////
//
//
void
Player::gameOver(GameOver *cmd)
//
//////////////////////////////////////////////////////////////////////////////
{
    renderArea->getSceneGraph()->enableNotify(TRUE);
    SoSwitch *endGameLight = (SoSwitch *) SoNode::getByName("endGameLight");
    assert (endGameLight != NULL);
    endGameLight->whichChild = id;

    if (cmd->whoWon == id) {
	Sound::playForDuration(Sound::VICTORY, 14.46); // play twice...

	// The victory dance
    	SoTranslation *danceInitPos = 
		(SoTranslation *) SoNode::getByName("dancePuckPos");
    	assert (danceInitPos != NULL);
    	SbVec2f lastPuckPos = latestState.puckLoc.getValue();
    	danceInitPos->translation.setValue(
			lastPuckPos[0], PUCK_HEIGHT/2, -lastPuckPos[1]);

	if (id == 1) {
    	    SoInterpolateFloat *stageZero = 
		    (SoInterpolateFloat *) SoEngine::getByName("stageZero");
    	    assert (stageZero != NULL);
	    stageZero->input1 = -1.57079;
	    
    	    SoInterpolateVec3f *stageOneTrans = 
		    (SoInterpolateVec3f *) SoEngine::getByName("stageOneTrans");
    	    assert (stageOneTrans != NULL);
	    stageOneTrans->input1.setValue(0,0,-30);

    	    SoInterpolateFloat *stageOneRot = 
		    (SoInterpolateFloat *) SoEngine::getByName("stageOneRot");
    	    assert (stageOneRot != NULL);
	    stageOneRot->input0 = 3.14159;
	    stageOneRot->input1 = -3.14159;
	    
    	    SoInterpolateVec3f *stageTwoTrans = 
		    (SoInterpolateVec3f *) SoEngine::getByName("stageTwoTrans");
    	    assert (stageTwoTrans != NULL);
	    stageTwoTrans->input0.setValue(0,0,-30);
	    stageTwoTrans->input1.setValue(0,0,-60);
	    
    	    SoInterpolateFloat *stageTwoRot = 
		    (SoInterpolateFloat *) SoEngine::getByName("stageTwoRot");
    	    assert (stageTwoRot != NULL);
	    stageTwoRot->input0 = 3.14159;
	    stageTwoRot->input1 = -3.14159;
	    
    	    SoInterpolateVec3f *stageThreeTrans = 
		    (SoInterpolateVec3f *) SoEngine::getByName("stageThreeTrans");
    	    assert (stageThreeTrans != NULL);
	    stageThreeTrans->input0.setValue(0,0,-60);
	    stageThreeTrans->input1.setValue(0,0,-90);

    	    SoInterpolateFloat *stageThreeRot = 
		    (SoInterpolateFloat *) SoEngine::getByName("stageThreeRot");
    	    assert (stageThreeRot != NULL);
	    stageThreeRot->input0 = 3.14159;
	    stageThreeRot->input1 = -3.14159;
	}

    	SoOneShot *victoryDance = 
		(SoOneShot *) SoEngine::getByName("victoryOneShot");
    	assert (victoryDance != NULL);
    	victoryDance->trigger.touch();

    } else {
	Sound::playOnce(Sound::LOSER);

	// "you lost" message drops down from the sky
    	SoTransform *loserXform = 
	    	(SoTransform *) SoNode::getByName("loserTransform");
    	assert (loserXform != NULL);

	if (id == 0) {
	    loserXform->translation.setValue(
			myGunPosition[0], 0, myGunPosition[2]-60);

	 } else {
	    loserXform->translation.setValue(
			myGunPosition[0], 0, myGunPosition[2]+60);
	    loserXform->rotation.setValue(SbRotation(SbVec3f(0,1,0), 3.14159));

	    SoSwitch *loserShadow =
		    (SoSwitch *) SoNode::getByName("loserShadow");
    	    assert (loserShadow != NULL);
	    loserShadow->whichChild = 1;
  	}

    	SoOneShot *loserOneShot = 
		(SoOneShot *) SoEngine::getByName("loserOneShot");
    	assert (loserOneShot != NULL);
    	loserOneShot->trigger.touch();
    }

    // Make sure the play puck isn't visible
    noPuck = TRUE;
    newState = TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
// Update the state.
// Called when we get new state from the server.
//
void
Player::updateState(RenderState *state)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (simulationTimestamp > state->timestamp) 
	return;

    latestState = *state;
    newState = TRUE;

    // Grab the timestamp, which is used to sync shots to what
    // the player is actually looking at.
    simulationTimestamp = state->timestamp;
    renderTimestamp = SbTime::getTimeOfDay();
}


//////////////////////////////////////////////////////////////////////////////
//
// Update the view volume of the projector used to determine where
// the cursor is located on the ice
//
void
Player::updateProjector()
//
//////////////////////////////////////////////////////////////////////////////
{
    SbVec2s size = renderArea->getSize();
    float fx = size[0];
    float fy = size[1];
    iceProjector.setViewVolume(camera->getViewVolume(fx/fy));
}


//////////////////////////////////////////////////////////////////////////////
//
// Provide feedback for the passed collision.
//
void
Player::collisionFeedback(CollisionState *state)
//
//////////////////////////////////////////////////////////////////////////////
{
    SbBool doVisualFeedback = FALSE;
    
    if (state->type == COLLISION_SHOT_HIT_PUCK) {
	Sound::playOnce(Sound::SHOT_PUCK);
    }
    
    else if (state->type == COLLISION_PUCK_HIT_WALL) {
	Sound::playOnce(Sound::PUCK_WALL);
	doVisualFeedback = TRUE;
    }
    
    else if (state->type == COLLISION_PUCK_HIT_GOALPOST) {
	Sound::playOnce(Sound::PUCK_POST);

	// Play ahh sound if we nearly scored, but not if the 
	// other team nearly scored.
 	if ((id == 0 && state->loc[1] > 0) ||
 	    (id == 1 && state->loc[1] < 0)) {
	    Sound::playOnce(Sound::AHH);
	}
	     
	doVisualFeedback = TRUE;
    } 
    
    else if (state->type == COLLISION_PUCK_HIT_PILLAR) {
	Sound::playOnce(Sound::PUCK_PILLAR);
	doVisualFeedback = TRUE;
    }
    
    // Provide some visual feedback
    if (doVisualFeedback) {
	SbVec2f loc = state->loc;
	SbVec2f norm = state->norm;
    
	// Draw cartoon collision effect
	SoRotationXYZ *collRot = 
		    (SoRotationXYZ *) SoNode::getByName("CollisionRot");
	assert (collRot != NULL);
	
	// Get the angle
	float angle = acos(norm.dot(SbVec2f(0, -1)));
	if (norm[0] < 0) angle = -angle;
	collRot->angle = angle;

	SoTranslation *collLoc = 
		    (SoTranslation *) SoNode::getByName("CollisionTrans");
	assert (collLoc != NULL);
	collLoc->translation.setValue(loc[0], PUCK_HEIGHT, -loc[1]);
    
	SoOneShot *collTrigger = 
		    (SoOneShot *) SoEngine::getByName("CollisionTrigger");
	assert (collTrigger != NULL);
	collTrigger->trigger.touch();
    }
}

// static callback
void
Player::renderCB(void *userData, SoSensor *)
{ ((Player *) userData)->renderScene(); }

//////////////////////////////////////////////////////////////////////////////
//
// Render the scene using the latest state.
// Called by the idle sensor.
//
void
Player::renderScene()
//
//////////////////////////////////////////////////////////////////////////////
{
    RenderState *state = &latestState;
    SbVec2f *loc, *dir;

    // Update the puck and shots from the latest render state.
    if (newState) {

    	// Update the puck position
	if (noPuck) {
	    if (id==0)
    	        puckLoc->translation.setValue(0, 0, RINK_LENGTH);
	    else
    	        puckLoc->translation.setValue(0, 0, -RINK_LENGTH);
	} else {
    	    loc = &state->puckLoc;
    	    puckLoc->translation.setValue((*loc)[0], PUCK_HEIGHT/2, -(*loc)[1]);
  	}
	
    	// Update the shot positions and rotations
    	shots->removeAllChildren();
    	for (int i=0; i<2; i++) {
    	    for (int j=0; j<state->numShots[i]; j++) {
	    	loc = &state->shotLoc[i][j];
	    	dir = &state->shotDir[i][j];
            	shotLoc[i][j]->translation.setValue(
					(*loc)[0], GUN_BARREL_RADIUS, -(*loc)[1]);
            	shotDir[i][j]->angle = atanf((*dir)[0] / -(*dir)[1]);
	    	shots->addChild(shotGroup[i][j]);
	    }
    	}

	newState = FALSE;
    }

    // Render the initial shot if the gun has just been fired
    if (shotFired) {
        SbVec3f loc;
        SoTranslation *firedShotLoc = 
			(SoTranslation *) SoNode::getByName("firedShotLoc");
        assert(firedShotLoc != NULL);
   	loc[0] = myGunPosition[0] + myGunDirection[0] * GUN_BARREL_LENGTH * 1.2;
	loc[1] = GUN_BARREL_RADIUS;
   	loc[2] = myGunPosition[2] - myGunDirection[1] * GUN_BARREL_LENGTH * 1.2;
    	firedShotLoc->translation.setValue(loc);

        SoRotationXYZ *firedShotRot = 
			(SoRotationXYZ *) SoNode::getByName("firedShotRot");
        assert(firedShotRot != NULL);
	firedShotRot->angle = atanf(myGunDirection[0] / -myGunDirection[1]);

	firedShotSwitch->whichChild = 0;

	shotFired = FALSE;

    } else {
	firedShotSwitch->whichChild = -1;
    }

    // Rotate my gun
    float oldAngle = myGunRot->angle.getValue();
    myGunRot->angle = atanf(myGunDirection[0] / -myGunDirection[1]);

    // Translate my gun.
    SbVec3f oldTranslation = myGunTrans->translation.getValue();
    translateMyGun();
    
    // Send current gun rotation and translation values to the server,
    // but only if we're playing against somebody and the values have
    // changed since last time.
    if (numPlayers == 2 && 
	(oldAngle != myGunRot->angle.getValue() ||
         oldTranslation != myGunTrans->translation.getValue())) {

    	sendToServer(
	    Gun(id, myGunTrans->translation.getValue(),
		    myGunRot->angle.getValue()));
    }

    // Redraw now!!! (we turned auto redraw off)
    renderArea->render();

    if (updateScoreboard) {
	renderArea->renderOverlay();
	updateScoreboard = FALSE;
    }

    // Reschedule the idle sensor
    renderSensor->schedule();
}


//////////////////////////////////////////////////////////////////////////////
//
// Called to translate the gun.  When the gun moves, so does the camera.
// Called every render update.
//
void
Player::translateMyGun()
//
//////////////////////////////////////////////////////////////////////////////
{ 
    if (moveGunLeft || moveGunRight) {
	SbVec3f newCameraPos = camera->position.getValue();

	float moveGun;
	float newPos = myGunPosition[0];
	if (moveGunLeft) {
	    if (moveLeftCount < 3)
		moveGun = -2.;
	    else
		moveGun = -4.;
	    if (id == 1)
		moveGun = -moveGun;
	    newPos += moveGun;
	    moveLeftCount++;
	}

        if (moveGunRight) {
	    if (moveRightCount < 3)
		moveGun = 2.;
	    else
		moveGun = 4.;
	    if (id == 1)
		moveGun = -moveGun;
	    newPos += moveGun;
	    moveRightCount++;
	}
	 
	if (fabsf(newPos) < RINK_X) {
    	    newCameraPos[0] = newPos; 
    	    myGunPosition[0] = newPos;
	    camera->position.setValue(newCameraPos);
	    myGunTrans->translation.setValue(myGunPosition);  
	    updateProjector();
	}
    }    
}


//////////////////////////////////////////////////////////////////////////////
//
// Called to start animating a pillar up.
// downTime is the time the pillar should go back down.
//
void
Player::animatePillarStart(int whichPillar, const SbTime &downTime)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Make the pillar visible with some initial size
    PillarInfo *info = (PillarInfo *) pillarInfo[whichPillar];
    info->sw->whichChild = SO_SWITCH_ALL;
    
    // Save the downTime so we know when to put the pillar down
    info->downTime = downTime;
    
    // See how long the pillar will actually be up, to help us animate it.
    info->duration = downTime - SbTime::getTimeOfDay();

    // Make sure our timer is ticking...
    if (! animatingPillarSensor->isScheduled())
	animatingPillarSensor->schedule();
}

//////////////////////////////////////////////////////////////////////////////
//
// Animate the pillars up and down.
// Called via a timer sensor.
//
void
Player::animatePillars()
//
//////////////////////////////////////////////////////////////////////////////
{
    // Run through all the pillars. If a pillar is up, animate it,
    // or if it should be down, remove it.
    SbTime now = SbTime::getTimeOfDay();
    for (int which = 0; which < NUM_PILLARS; which++) {
	PillarInfo *info = (PillarInfo *) pillarInfo[which];
	
	// Is pillar up (i.e. is it visible?)
	SoSwitch *sw = info->sw;
	if (sw->whichChild.getValue() == SO_SWITCH_ALL) {
	
	    // If pillar should be down, hide it.
	    // Compare with renderTimeStamp, which is the last rendered
	    // state that the server told us about, as opposed to the
	    // time of day, just to make sure we are in sync with the
	    // server.
	    if (info->downTime < now) {
		sw->whichChild = SO_SWITCH_NONE;
	    }
	    
	    // Else, animate the pillar so it goes up during the first
	    // half of its duration time, and down during the last half.
	    else {
		// t goes from 0 to 1
		float t = 1.0 - (info->downTime - now)/info->duration;

		// use and ease-in ease-out approach.
		// height will go from 0 to MAX_HEIGHT to 0, while t
		// goes from 0 to 1.
#define MAX_PILLAR_HEIGHT 7.0		
		float height = MAX_PILLAR_HEIGHT * sinf(t * M_PI);
		setPillarHeight(which, height);
	    }
	}
    }
}

// static callback
void Player::animatePillarCB(void *userData, SoSensor *)
{ ((Player *) userData)->animatePillars(); }

void Player::restlessCrowdCB(void *userData, SoSensor *)
{ ((Player *) userData)->restlessCrowd(); }

void Player::printHelp()
{
    printf("\n***** Puck player interface *****\n\n");
    printf("To aim the gun:\n");
    printf("\tmove the mouse cursor\n");
    printf("To move the gun left/right:\n");
    printf("\thold down the left/right mouse button\n");
    printf("To move the gun up/down:\n");
    printf("\thold down the middle mouse button and move the mouse\n");
    printf("To fire a shot:\n");
    printf("\thit the spacebar\n");
    printf("To request a new game:\n");
    printf("\thit the 'g' key\n");
    printf("To set the game level in a single player game:\n");
    printf("\thit the '1' key for a beginner level game (slow opponent)\n");
    printf("\thit the '2' key for an advanced level game (fast opponent)\n");
    printf("\thit the '0' key for a practice session (very slow opponent)\n");
    printf("To print this help message:\n");
    printf("\thit the 'h' key\n");
    
}

