//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Description:
 |	The Axle class is an interactively moveable axle.
 |	It contains instances of three other new classes,
 |      AxleShaft, AxleLeg, and AxlePin.
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_AXLE_
#define  _SO_AXLE_

#include <Inventor/SbLinear.h>
#include <Inventor/SoLists.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec2f.h>

#include "LinkEngines.h"
#include "MechPiece.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: AxleShaft
//
//////////////////////////////////////////////////////////////////////////////

class AxleShaft : public SoTranslate2Dragger {

    SO_KIT_HEADER(AxleShaft);

    SO_KIT_CATALOG_ENTRY_HEADER(zRotator);

    SO_KIT_CATALOG_ENTRY_HEADER(pickStyle);

    SO_KIT_CATALOG_ENTRY_HEADER(translatorInert);

    SO_KIT_CATALOG_ENTRY_HEADER(zBoundsOkSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(zBoundsSep);
    SO_KIT_CATALOG_ENTRY_HEADER(zBoundsXf);
    SO_KIT_CATALOG_ENTRY_HEADER(zScaledTranslatorSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(zScaledTranslator);
    SO_KIT_CATALOG_ENTRY_HEADER(zScaledTranslatorActive);
    SO_KIT_CATALOG_ENTRY_HEADER(zScaledTranslatorInert);

  public:

    // Constructor
    AxleShaft();

    SoSFFloat zRotation;  // Default = 0
    SoSFFloat zMin;       // (relative to origin) Default = -.2
    SoSFFloat zMax;       // (relative to origin) Default = .2

    SbBool isDraggable();   // Can the pieces be moved?
    void   setDraggable( SbBool onOff );
    
  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

    // Sets the zScaledTranslatorSwitch on/off
    static void startCB( void *, SoDragger *);
    static void finishCB( void *, SoDragger *);

    // Sets the zBoundsXf based on zMin and zMax fields
    EncloseEngine *encloseEngine;

    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void   setDefaultOnNonWritingFields();

  private:

    // Destructor
    ~AxleShaft();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: AxleLeg
//
//////////////////////////////////////////////////////////////////////////////

class AxleLeg : public SoTranslate1Dragger {

    SO_KIT_HEADER(AxleLeg);

    SO_KIT_CATALOG_ENTRY_HEADER(pickStyle);

    SO_KIT_CATALOG_ENTRY_HEADER(translatorInert);

    SO_KIT_CATALOG_ENTRY_HEADER(yBoundsOkSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(yBoundsSep);
    SO_KIT_CATALOG_ENTRY_HEADER(yBoundsXf);
    SO_KIT_CATALOG_ENTRY_HEADER(yScaledTranslatorSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(yScaledTranslator);
    SO_KIT_CATALOG_ENTRY_HEADER(yScaledTranslatorActive);
    SO_KIT_CATALOG_ENTRY_HEADER(yScaledTranslatorInert);

  public:

    // Constructor
    AxleLeg();

    SoSFFloat yMin;       // (Relative to origin) Default = -1

    SbBool isDraggable();   // Can the pieces be moved?
    void   setDraggable( SbBool onOff );
    
  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

    // Sets the yScaledTranslatorSwitch on/off
    static void startCB( void *, SoDragger *);
    static void finishCB( void *, SoDragger *);

    // Extracts a value for the upper y value of the enclose engine
    // from the origin's y value.
    SoCalculator *calcEngine;

    // Sets the yBoundsXf based on yMin and yMax fields
    EncloseEngine *encloseEngine;

    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void   setDefaultOnNonWritingFields();

  private:

    // Destructor
    ~AxleLeg();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: AxlePin
//
//////////////////////////////////////////////////////////////////////////////

class AxlePin : public SoTranslate1Dragger {

    SO_KIT_HEADER(AxlePin);

    SO_KIT_CATALOG_ENTRY_HEADER(pickStyle);

    SO_KIT_CATALOG_ENTRY_HEADER(translatorInert);

  public:

    // Constructor
    AxlePin();

    SbBool isDraggable();   // Can the pieces be moved?
    void   setDraggable( SbBool onOff );
    
  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void   setDefaultOnNonWritingFields();

  private:

    // Destructor
    ~AxlePin();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Axle
//
//////////////////////////////////////////////////////////////////////////////

class Axle : public MechPiece {

    SO_KIT_HEADER(Axle);

    SO_KIT_CATALOG_ENTRY_HEADER(shaftAssemblySwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(shaft);
    SO_KIT_CATALOG_ENTRY_HEADER(leg);

    SO_KIT_CATALOG_ENTRY_HEADER(translateToOrigin);

    SO_KIT_CATALOG_ENTRY_HEADER(pinSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(pinSep);
    SO_KIT_CATALOG_ENTRY_HEADER(pinZTranslationOffset);
    SO_KIT_CATALOG_ENTRY_HEADER(pinZRotationOffset);
    SO_KIT_CATALOG_ENTRY_HEADER(pinZRotation);
    SO_KIT_CATALOG_ENTRY_HEADER(pin);

    SO_KIT_CATALOG_ENTRY_HEADER(feedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(movingFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(connectFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(disconnectFeedback);

  public:

    SbBool isShowingParts();   // Is shaftAssemblySwitch or pinSwitch on?
    void   setShowingParts( SbBool onOff );

    SbBool isDraggable();   // Can the pieces be moved?
    void   setDraggable( SbBool onOff );

    // Does this axle have a leg part displayed below the shaft?
    SbBool isWithLegs();
    void   setWithLegs(SbBool onOff);

    // These determine state of axle
    SoSFFloat zRotation;  // Rotation of axle (default = 0).

    // These determine placement of pieces, along with state.
    SoSFFloat shaftZDepth;  // Amount shaft extends in Z about origin.
			    // (Default = .3) (in each direction)
    SoSFFloat floorHeight;  // Height of floor. (default = 0) 
			    // Influences leg positioning.
    SoSFVec3f pinZTransOffset; // For locating pin relative to origin.
				// (default = .2)
    SoSFFloat pinZRotOffset;   // For rotating pin relative to zRotation.
				// (default = .7)
    SoSFFloat pinActivationDist;  // When pin slides this far from origin,
			    // we disconnect from leading axle. (default = .5)
			    // If negative, cannot connect/disconnect.
    SoSFBool  canFollowRot; // Motors can only follow translations of other
			    // axles, but not their rotations. They set this
			    // field to FALSE on their axles. Default is TRUE

    // Constructor
    Axle();
   
    // newLeader becomes the leadingAxle (protected field)
    // If NULL, doesn't follow any axle.
    Axle *getLeader();
    void follow( Axle *newLeader, SbBool forceIt = FALSE );

    virtual void setZPlane( float newZ );

    virtual void setWorld( WorldInfo *newWorld );
    virtual void undoAxleDependencies();
    virtual SbPList *getAxlePtrs();

    void    addShaftStartCallback(SoDraggerCB *f, void *userData = NULL);
    void removeShaftStartCallback(SoDraggerCB *f, void *userData = NULL);
    void    addShaftFinishCallback(SoDraggerCB *f, void *userData = NULL);
    void removeShaftFinishCallback(SoDraggerCB *f, void *userData = NULL);

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

    // To keep track of connections.
    SoSFNode  leadingAxle; // (default = NULL). Else, only
			    // pin is displayed. Moving pin beyond
			    // pinActivationDist makes it stop following,
			    // and then the shaftAssembly is shown.

    static void subDraggerStartCB( void *, SoDragger *);
    static void pinStartCB( void *, SoDragger *);
    static void shaftMovedCB( void *, SoDragger *);
    static void legMovedCB( void *, SoDragger *);
    static void pinMovedCB( void *, SoDragger *);
    static void shaftFinishCB( void *, SoDragger *);
    static void legFinishCB( void *, SoDragger *);
    static void pinFinishCB( void *, SoDragger *);

    Axle *findAxleToFollow();
    SbBool isPinReadyToUnlock();

    void transferFollowers( Axle *newLeader );
    SbPList *getRecursiveFollowersList();
    void     tellLeaderToUpdateFollowers();
    void     updateRecursiveFollowersList();
 
    SoFieldSensor *shaftZDepthFieldSensor;
    static void axleFieldSensorCB( void *, SoSensor * );

    // If following, just center about origin and update leader.
    // If leading, adjust to include all followers.
    void adjustShaftZMinMax( SbBool doLeader, SbBool checkZPlane = TRUE ); 

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void   setDefaultOnNonWritingFields();

    void setUpFollowConnections( SbBool onOff );

    AxleFollowEngine *axleFollowEngine;

  private:

    SbPList *recursiveFollowersList;

    float prevZPlane;

    // Destructor
    ~Axle();
};

#endif /* _SO_AXLE_ */
