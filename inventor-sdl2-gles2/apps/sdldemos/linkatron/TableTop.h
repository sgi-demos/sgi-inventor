//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Description:
 |	This file defines the table top class.
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TABLE_TOP_
#define  _SO_TABLE_TOP_

#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>

class SoFieldSensor;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TableTop
//
//////////////////////////////////////////////////////////////////////////////

class TableTop : public SoBaseKit {

    SO_KIT_HEADER(TableTop);

    SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);

    SO_KIT_CATALOG_ENTRY_HEADER(nearLeftSep);
    SO_KIT_CATALOG_ENTRY_HEADER(nearLeftDraggerRot);
    SO_KIT_CATALOG_ENTRY_HEADER(nearLeftDragger);

    SO_KIT_CATALOG_ENTRY_HEADER(farLeftSep);
    SO_KIT_CATALOG_ENTRY_HEADER(farLeftDraggerRot);
    SO_KIT_CATALOG_ENTRY_HEADER(farLeftDragger);

    SO_KIT_CATALOG_ENTRY_HEADER(nearRightSep);
    SO_KIT_CATALOG_ENTRY_HEADER(nearRightDraggerRot);
    SO_KIT_CATALOG_ENTRY_HEADER(nearRightDragger);

    SO_KIT_CATALOG_ENTRY_HEADER(farRightSep);
    SO_KIT_CATALOG_ENTRY_HEADER(farRightDraggerRot);
    SO_KIT_CATALOG_ENTRY_HEADER(farRightDragger);

    SO_KIT_CATALOG_ENTRY_HEADER(surfaceSep);

    // This 'GraphPaper' node gets placed just above bottomDragger.
    SO_KIT_CATALOG_ENTRY_HEADER(graphPaper);  

    // The fieldSensorCB insures that this SoTranslation  moves the table to
    // the center of (near,far,left,right). It doesn't translate in Y.
    SO_KIT_CATALOG_ENTRY_HEADER(bottomDraggerPlacer);

    // The fieldSensorCB insures that this scales a 2x2 (in x,z) square 
    // to the size of (right-left) by (near-far). It leave the y-scale alone
    SO_KIT_CATALOG_ENTRY_HEADER(bottomDraggerScale);
    SO_KIT_CATALOG_ENTRY_HEADER(bottomDraggerRot);
    SO_KIT_CATALOG_ENTRY_HEADER(bottomDragger);


    SO_KIT_CATALOG_ENTRY_HEADER(activePlaneDraggerSep);
    SO_KIT_CATALOG_ENTRY_HEADER(activePlaneDraggerPlacer);
    SO_KIT_CATALOG_ENTRY_HEADER(activePlaneDragger); 

    SO_KIT_CATALOG_ENTRY_HEADER(activePlaneGeomSep);
    SO_KIT_CATALOG_ENTRY_HEADER(activePlaneGeomFitter); // Fits within left,
							// right, bottom, top
    SO_KIT_CATALOG_ENTRY_HEADER(activePlaneGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(topSep);
    SO_KIT_CATALOG_ENTRY_HEADER(topDraggerPlacer);
    SO_KIT_CATALOG_ENTRY_HEADER(topDragger);

  public:
    // Constructor
    TableTop();
    
    // These fields can be changed interactively with draggers.
    SoSFFloat    activePlane; // Default = 0

    SoSFFloat    left;        // Default = -5
    SoSFFloat    right;       // Default = 5
    SoSFFloat    near;        // Default = 5
    SoSFFloat    far;         // Default = -5
    SoSFFloat    bottom;      // Default = 0
    SoSFFloat    top;         // Default = 10

    // Constraints prevent table from getting too small.
    SoSFVec3f    smallestBoxMin;   // Default = (-2,  0, -2)
    SoSFVec3f    smallestBoxMax;   // Default = ( 2,  4,  2)

    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

    SoFieldSensor *activePlaneFieldSensor;
    SoFieldSensor *leftFieldSensor;
    SoFieldSensor *rightFieldSensor;
    SoFieldSensor *nearFieldSensor;
    SoFieldSensor *farFieldSensor;
    SoFieldSensor *bottomFieldSensor;
    SoFieldSensor *topFieldSensor;
    SoFieldSensor *smallestBoxMinFieldSensor;
    SoFieldSensor *smallestBoxMaxFieldSensor;

    static void   fieldSensorCB( void *, SoSensor * );

    static void   nearLeftVCCB( void *, SoDragger * );
    static void   farLeftVCCB( void *, SoDragger * );
    static void   nearRightVCCB( void *, SoDragger * );
    static void   farRightVCCB( void *, SoDragger * );
    static void   bottomVCCB( void *, SoDragger * );
    static void   activePlaneVCCB( void *, SoDragger * );
    static void   topVCCB( void *, SoDragger * );

  private:
    
    void constrainToLimits();
    void getLimits( float &, float &, float &, float &, float &, float  & );

  protected:	// SDL port: derived classes need the dtor
    // Destructor
    ~TableTop();
  private:
};

#endif /* _SO_TABLE_TOP_ */
