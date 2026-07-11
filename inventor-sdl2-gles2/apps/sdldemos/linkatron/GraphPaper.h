//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Description:
 |	This file defines the GraphPaper class.
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GRAPH_PAPER_
#define  _SO_GRAPH_PAPER_

#include <Inventor/SbLinear.h>
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFFloat.h>

class SoFieldSensor;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: GraphPaper
//
//////////////////////////////////////////////////////////////////////////////

class GraphPaper : public SoBaseKit {

    SO_KIT_HEADER(GraphPaper);

    SO_KIT_CATALOG_ENTRY_HEADER(paperSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(paperHeightPlacer);
    SO_KIT_CATALOG_ENTRY_HEADER(paperAppearance);
    SO_KIT_CATALOG_ENTRY_HEADER(paperCoordinate3);
    SO_KIT_CATALOG_ENTRY_HEADER(paperFaceSet);

    SO_KIT_CATALOG_ENTRY_HEADER(lineSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(lineHeightPlacer);
    SO_KIT_CATALOG_ENTRY_HEADER(lineAppearance);
    SO_KIT_CATALOG_ENTRY_HEADER(lineNormalBinding);
    SO_KIT_CATALOG_ENTRY_HEADER(lineNormal);
    SO_KIT_CATALOG_ENTRY_HEADER(lineCoordinate3);
    SO_KIT_CATALOG_ENTRY_HEADER(lineLineSet);

  public:
    // Constructor
    GraphPaper();
    
    SoSFFloat    left;        // Default = -4
    SoSFFloat    right;       // Default = 4
    SoSFFloat    near;        // Default = 4
    SoSFFloat    far;         // Default = -4
    SoSFFloat    paperHeight; // Default = 0
    SoSFFloat    lineHeight;  // Default = 0.01

    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

    SoFieldSensor *leftFieldSensor;
    SoFieldSensor *rightFieldSensor;
    SoFieldSensor *nearFieldSensor;
    SoFieldSensor *farFieldSensor;
    SoFieldSensor *paperHeightFieldSensor;
    SoFieldSensor *lineHeightFieldSensor;

    static void   fieldSensorCB( void *, SoSensor * );

  private:
    
  protected:	// SDL port: derived classes need the dtor
    // Destructor
    ~GraphPaper();
  private:
};

#endif /* _SO_GRAPH_PAPER_ */
