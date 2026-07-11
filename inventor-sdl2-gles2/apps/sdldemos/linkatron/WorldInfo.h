//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Description:
 |	This file defines the WorldInfo class. This is a separator
 |      with a tableTop and a scene in it.
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_WORLD_INFO_
#define  _SO_WORLD_INFO_

#include <Inventor/SoLists.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/SbBox.h>

class ShadowKit;
class TableTop;
class MechPiece;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: WorldInfo
//
//////////////////////////////////////////////////////////////////////////////

class WorldInfo {
    
  public:

    WorldInfo();
    ~WorldInfo();

    SoSeparator *getWorldRoot() { return worldRoot; }

    // Tabletop
    void   setTableOn( SbBool onOff );
    SbBool isTableOn();
    void setTableTop( TableTop *newTableTop ); // NULL removes table entirely
    TableTop *getTableTop() { return tableTop; }

    // Adjusting table top based on size of scene.
    SbBox3f minTableSize;
    void sizeTableTop( SbBool includeShadow = FALSE );
    void expandTableTopToFitScene( SbBool includeShadow = FALSE );
    void constrainTableTopToFitScene( SbBool includeShadow = FALSE );

    // An argument of NULL will create an empty separator for the scene.
    // setScene also recalculates the axleList.
    void setScene( SoSeparator *newScene );
    SoSeparator *getScene() { return sceneRoot; }
    SbBool isSceneEmpty();

    // Shadows
    void   setShadowOn( SbBool onOff );
    SbBool isShadowOn();
    ShadowKit *getShadowKit() { return shadowKit; }

    // Pieces - these may add or delete from the axleList
    void addPiece( SoNode *newPiece );
    void deletePiece( SoNode *pieceToRemove );

    void deleteCurrentPiece();
    void undeletePiece();

    // Return a list of the axles in the scene.
    SbPList * getAxleList() { return axleList; }
    SbPList * getAxleOwnerList() { return axleOwnerList; }

    void addNewAxle();
    void addNewRigidStrut();
    void addNewDoubleStrut();
    void addNewWheel();
    void addNewArrow();
    void addNewDoubleArrow();
    void addNewBell();
    void addNewRotorMotor();

  protected:

    SoSeparator  *worldRoot;
    SoDirectionalLight *dirLight;
    SoShapeHints *shapeHints;
    SoSwitch    *tableTopSwitch;
    TableTop    *tableTop;
    SoSeparator *sceneAndShadow;
    // catches mouse presses and sets currentPiece without setting event handled
    SoEventCallback *piecePicker;
    SoSeparator *sceneRoot;
    ShadowKit   *shadowKit;

    const SbBox3f &getSceneBounds( SbBool includeShadow );

    static void activePlaneDraggerCB( void *, SoDragger * );
    static void bottomDraggerCB( void *, SoDragger * );
    static void mousePressCB( void *userData, SoEventCallback *eventCB);

    void setCurrentPiece( MechPiece *newPiece );
    MechPiece *getCurrentPiece() { return currentPiece; }

    // Finds all MechPieces under scene and adds their axles to the list.
    // Also makes sure that all axles have setWorld(this) 
    void calculateAxleList();

    // Finds all axles in the piece and adds them to the list.
    void addAxlesToList( MechPiece *thePiece );
    void removeAxlesFromList( MechPiece *thePiece );


  private:
    SbBox3f sceneBounds;
    SbPList *axleList;
    SbPList *axleOwnerList;
    MechPiece *currentPiece;

    // Stores deleted pieces, most recently deleted added to end.
    // When undelete() is called, last entry is removed and put into scene.
    SoNodeList *deletedPieces;
};

#endif /* _SO_WORLD_INFO_ */
