//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Description:
 |	This is the base class for all mechanical pieces.
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MECH_PIECE_
#define  _SO_MECH_PIECE_

#include <Inventor/SbLinear.h>
#include <Inventor/SoLists.h>
#include <Inventor/nodekits/SoInteractionKit.h>

#include <Inventor/fields/SoSFVec3f.h>

class SoWriteAction;
class WorldInfo;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: MechPiece
//
//////////////////////////////////////////////////////////////////////////////

class MechPiece : public SoInteractionKit {

    SO_KIT_HEADER(MechPiece);

  public:
    // Constructor
    MechPiece();
    
    SoSFVec3f    origin;   // Default = (0, 0, 0)

    virtual void setZPlane( float newZ );

    // Allows axles to compare locations with others in scene.
    // Sets floor height.
    virtual void setWorld( WorldInfo *newWorld );
    WorldInfo *getWorld() { return myWorld; }

    float getZPlane();  // Can always get from origin.
    virtual void undoAxleDependencies();
    virtual SbPList *getAxlePtrs();

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void   setDefaultOnNonWritingFields();


    virtual void write( SoWriteAction *action );

    static void connectFromField( SoField *fromField, SoField *toField );
    static void connectFromEngine(SoEngineOutput *fromEngine, SoField *toField);
    static void disconnectField( SoField &theField, 
				 SoField *ifConnectedFrom = NULL );

    // This is a total kludge used to make the nodekits more efficient.
    static void makeNodeFieldStopNotifying( SoSFNode &theField);

    // This disables notification, sets the field, then returns 
    // notification to original state.
    static void setVec3fDisabled( SoSFVec3f &theField, const SbVec3f &theVal );

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

  private:

    // List of all axles in this piece.
    SbPList *axlePtrs;
    
  protected:	// SDL port: derived classes need the dtor
    // Destructor
    ~MechPiece();
  private:

    WorldInfo *myWorld;
};

#endif /* _SO_MECH_PIECE_ */
