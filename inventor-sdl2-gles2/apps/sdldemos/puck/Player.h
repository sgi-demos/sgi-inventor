//  -*- C++ -*-

/*
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.47 $
 |
 |   Description:
 |      Defines the Player class for crossfire.
 |
 |   Author(s): David Mott, Helga Thorvaldsdottir
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _CROSSFIRE_PLAYER_
#define _CROSSFIRE_PLAYER_

#ifdef SDL_PORT
#include <SDL2/SDL.h>
#include <SoSDL.h>
#include <SoSDLRenderArea.h>
// Source-compat aliases: the body of this class keeps SGI's names.
#define SoXtRenderArea SoSDLRenderArea
#define SoXtComponent  SoSDLRenderArea
typedef void *Widget;
typedef long KeySym;
typedef char Boolean;
// X event compat: translated SDL events feed the original X-typed
// handlers (see Player::handleSDLEvent). For the keys Puck uses, the
// Latin-1 X keysym values coincide with SDL's SDLK_* values.
struct XButtonEvent     { int x, y; unsigned int button; };
struct XPointerMovedEvent { int x, y; };
struct XKeyPressedEvent { long keysym; };
struct XAnyEvent        { int type; };
#define Button1 1
#define Button2 2
#define Button3 3
#define XK_space 0x20
#define XK_0     0x30
#define XK_1     0x31
#define XK_2     0x32
#define XK_G     0x47
#define XK_H     0x48
#define XK_g     0x67
#define XK_h     0x68
#else
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#endif
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/sensors/SoIdleSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoAlarmSensor.h>
#include "SoSocket.h"
#include "msg.h"
#include "Rink.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Player
//
//////////////////////////////////////////////////////////////////////////////

class Player {
  public:
    Player(const char *serverMachine, Widget window, SbBool useSound);
#ifdef SDL_PORT
    // SDL events, translated onto the original X-typed handlers.
    void		handleSDLEvent(const SDL_Event *e);
#endif
    ~Player();
    
    SoXtRenderArea *	getRenderArea() const { return renderArea; }
    
  protected:
    // Send a message to the server
    void		sendToServer(const Command &cmd);
    
    // This is called to read data from the socket.
    void		processSocket();

    // Called when server says we can start the game
    void		initGame(StartGame *cmd);
    void		initGraph();

    void		updateState(RenderState *state);
    void		scoreGoal(GoalScored *goal);
    void		renderScene();
    void		dropPuck(DropPuck *puck);
    void		gameOver(GameOver *cmd);
    void		collisionFeedback(CollisionState *);
    // Init pillar data pointers so we search for them once.
    void		initPillars();
    
    // Set the height of a pillar
    void		setPillarHeight(int whichPillar, float height);
    float		getPillarHeight(int whichPillar);
    
    // Called to start/finish animating a pillar up or down
    void		animatePillarStart(int whichPillar, const SbTime &downTime);
    void		animatePillars();
    
    // Called to get the crowd into the game
    void		restlessCrowd();
    
    // We need time stamps so that when a shot is fired, it is
    // inserted temporally-correct into the simulation state.
    
    // Time at which the simulation took place
    SbTime		simulationTimestamp;
    
    // Time at which the simulation was rendered for the player
    SbTime		renderTimestamp;

    // Called each time the gun is fired
    void		fireShot();

    // Called to update the view volume of the ice projector, to reflect
    // the current view volume of the camera
    void		updateProjector();

    // Routines that handle the different types of events
    void		handleButtonPress(XButtonEvent *);
    void		handleButtonRelease(XButtonEvent *);
    void		handleMouseMotion(XPointerMovedEvent *);
    void		handleKeyPress(XKeyPressedEvent *);
    
  private:
    void		initPuck();
    SoSeparator		*initGun(int whichPlayer);
    SoSeparator		*initShotGroup();

    // static callback routines
    static void		processSocketCB(XtPointer, int *, XtInputId *);
    static void		startSelectionCB(void *, SoPath *);
    static void 	leaveGameCB(void *, SoXtComponent *);
    static void		renderCB(void *, SoSensor *);
    static void		handleEvents(Widget, XtPointer, XAnyEvent *, Boolean *);

    // prints help message describing player user interface
    static void		printHelp();

    void		translateMyGun();

    SoSocket		*socket;
    struct sockaddr_in	server_addr;
    int                 id;     	// assigned by the server
    int			numPlayers;

    Widget		mainWindow;
    SoXtRenderArea	*renderArea;
    SbVec2s 		raSize; 
    SoIdleSensor	*renderSensor;

    SbBool		newGameRequested;
    RenderState		latestState;
    SbBool		newState;
    SbBool		noPuck;    
    SbBool		startScreen;

    // Info sent to server when a shot is fired
    SbVec2f		myGunDirection;
    SbVec3f		myGunPosition;

    // State variables set when the user interacts with the gun controls
    SbVec2f		prevMousePos;
    SbBool		moveGunUpDown;
    SbBool		moveGunLeft;
    SbBool		moveGunRight;
    int			moveLeftCount;
    int			moveRightCount;
    SbBool		shotFired;
    SbPlaneProjector	iceProjector;
    
    // Handles to frequently accessed nodes in the scene
    SoPerspectiveCamera *camera;
    SoTranslation	*puckLoc;
    SoRotationXYZ	*myGunRot;
    SoTranslation	*myGunTrans;
    SoRotationXYZ	*otherGunRot;
    SoTranslation	*otherGunTrans;

    // Nodes used to store shot information
    SoSeparator		*shots;
    SoSeparator  	*shotGroup[2][MAXSHOTS];
    SoTranslation 	*shotLoc[2][MAXSHOTS];
    SoRotationXYZ 	*shotDir[2][MAXSHOTS];
    SoSwitch		*firedShotSwitch;

    // Set when the scoreboard data changes => display needs to be updated
    SbBool		updateScoreboard;

    // Controls for the restless crowd sound effects
    static void		restlessCrowdCB(void *, SoSensor *);
    SoAlarmSensor	*restlessCrowdSensor;

    // Controls for the pillar animation
    static void		animatePillarCB(void *, SoSensor *);
    SoTimerSensor	*animatingPillarSensor;
    SbPList		pillarInfo;
};

#endif
