//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Description:
 |      Defines these classes for creating links to be used in linkages.
 |
 |      LinkBase   -
 |      SimpleLink - 
 |      Strut - ends defined by origin and endPoint fields.
 |              Has Axles to move the two ends.
 |      RigidStrut - ends defined by origin and offset fields.
 |                   Has Axles to move the two ends.
 |                   Only the origin can be connected. The endPoint moves
 |                   relative to it.
 |      DoubleStrut - Two Struts connected end to end.
 |      Wheel       - An axle in the middle, 
 |                    four read-only axles, 
 |                    a sizing ring and a rotator ring.
 |      Arrow         - Defined by origin, dirPoint, and length.
 |                      An axle at the origin,
 |                      an axle at a pivoting sleeve,
 |                      an inert axle at the end,
 |                      and a dragger to change the length.
 |      DoubleArrow - two arrows that point to each other. The
 |                    Control by two end axles and two length axles.
 |      Bell - Like a double arrow. When the two ends of the arrows 
 |             cross, it plays a sound file.
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_LINK_CLASSES_
#define  _SO_LINK_CLASSES_

#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodekits/SoInteractionKit.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include "MechPiece.h"
#include "Axle.h"
#include "LinkEngines.h"

//////////////////////////////////////////////////////////////////////////////
//  Class: LinkBase
//
//  Adds:
//        appearance part
//        isError field     (and support for displaying error appearance.)
//////////////////////////////////////////////////////////////////////////////

class LinkBase : public MechPiece {

    SO_KIT_HEADER(LinkBase);

    SO_KIT_CATALOG_ENTRY_HEADER(appearance);

  public:
    // Fields
    SoSFBool isError;

    // Constructor
    LinkBase();
    
    virtual void errorColor( SbBool useErrorColor );

    virtual SbPList *getAxlePtrs();

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void   setDefaultOnNonWritingFields();

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:
    SoFieldSensor *isErrorSensor;
    static void isErrorSensorCB( void *, SoSensor *);

  private:
    // Destructor
    ~LinkBase();

    SbBool isShowingErrorColor;
    SoNode *savedMaterial;
};

////////////////////////////////////////////////////////////////////
//    Class: SimpleLink - base class for 1-bar links
//    
//    Contains the parts, but no fields or connections are established,
//    other than between origin field and originTranslator.
//
//    New nodes in this subclass are:
//      originTranslator, originTranslateGeom,
//      angleRotator, angleRotateGeom,
//      endPointTranslateSeparator, endPointTranslator, endPointTranslateGeom,
//      oneDScaleSeparator, oneDScaler, oneDScaleGeom,
//      twoDScaleSeparator, twoDScaler, twoDScaleGeom,
//      threeDScaleSeparator, threeDScaler, threeDScaleGeom,
//
//    No New fields.
//
////////////////////////////////////////////////////////////////////
class SimpleLink : public LinkBase {

    SO_KIT_HEADER(SimpleLink);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(originTranslator);
    SO_KIT_CATALOG_ENTRY_HEADER(originTranslateGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(angleRotator);
    SO_KIT_CATALOG_ENTRY_HEADER(angleRotateGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(endPointTranslateSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(endPointTranslator);
    SO_KIT_CATALOG_ENTRY_HEADER(endPointTranslateGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(oneDScaleSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(oneDScaler);
    SO_KIT_CATALOG_ENTRY_HEADER(oneDScaleGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(twoDScaleSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(twoDScaler);
    SO_KIT_CATALOG_ENTRY_HEADER(twoDScaleGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(threeDScaleSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(threeDScaler);
    SO_KIT_CATALOG_ENTRY_HEADER(threeDScaleGeom);

  public:

    virtual void setZPlane( float newZ );
    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // constructor
    SimpleLink();

  SoINTERNAL public:
    static void initClass();

  protected:

  private:

    // destructor
    virtual ~SimpleLink();
};

////////////////////////////////////////////////////////////////////
//    Class: Strut - 'origin' and 'endPoint' control internal xfs.
//                 - originAxle and endPointAxle control the 
//  			'origin' and 'endPoint'
//
//    New fields in this subclass are:
//         endPoint         - location of second link point
//
////////////////////////////////////////////////////////////////////
class Strut : public SimpleLink {

    SO_KIT_HEADER(Strut);

    SO_KIT_CATALOG_ENTRY_HEADER(originAxle);
    SO_KIT_CATALOG_ENTRY_HEADER(endPointAxle);

  public:

    virtual SbPList *getAxlePtrs();
    virtual void setZPlane( float newZ );

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFVec3f    endPoint;      // far link point (default = (1,0,0)

    // constructor
    Strut();

  SoINTERNAL public:
    static void initClass();

  protected:

    StrutEngine *strutEngine;

  private:

    // destructor
    virtual ~Strut();
};

////////////////////////////////////////////////////////////////////
//    Class: RigidStrut - 'origin' and 'endPoint' control internal xfs.
//                      - originAxle and endPointAxle control the 
//  			  'origin' and 'endPoint'
//                   Only the originAxle can be connected. The endPoint moves
//                   relative to it.
//
//    New fields in this subclass are:
//         endPoint         - location of second link point
//
////////////////////////////////////////////////////////////////////
class RigidStrut : public SimpleLink {

    SO_KIT_HEADER(RigidStrut);

    SO_KIT_CATALOG_ENTRY_HEADER(originAxle);
    SO_KIT_CATALOG_ENTRY_HEADER(endPointAxle);

  public:

    virtual SbPList *getAxlePtrs();

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // constructor
    RigidStrut();

  SoINTERNAL public:
    static void initClass();

  protected:

    RigidStrutEngine *rigidStrutEngine;

    // When endPointAxle starts moving, we connect it to the engine.
    // and go into endpoint mode.
    // When it's done, we change back to offset mode.
    static void endAxleStartCB( void *, SoDragger *);
    static void endAxleFinishCB( void *, SoDragger *);

  private:

    // destructor
    virtual ~RigidStrut();
};

////////////////////////////////////////////////////////////////////
//    Class: DoubleStrut - a class that contains a two-bar linkage.
//
//    New nodes in this subclass are:
//      originAxle, endPointAxle,
//      sharedPointAxle - this one has no legs and will not
//                        constrain to other axles.
//      strut1, strut2
//
//    New fields in this subclass are:
//         endPoint       - endPoint of second link
//         sharedPoint    - common end of the two links.
//         size1         - size of first link
//         size2         - size of second link
//
////////////////////////////////////////////////////////////////////
class DoubleStrut : public LinkBase {

    SO_KIT_HEADER(DoubleStrut);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(strut1);
    SO_KIT_CATALOG_ENTRY_HEADER(strut2);

  public:

    virtual void setWorld( WorldInfo *newWorld );
    virtual void setZPlane( float newZ );
    virtual void undoAxleDependencies();
    virtual SbPList *getAxlePtrs();

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFVec3f    endPoint;     // endPoint of link2
    SoSFFloat    size1;       // size of link1
    SoSFFloat    size2;       // size of link2

    // constructor
    DoubleStrut();

    virtual void errorColor( SbBool useErrorColor );

  SoINTERNAL public:
    static void initClass();

  protected:

    DoubleStrutMoveEndEngine     *moveEndEngine;
    DoubleStrutMoveMiddleEngine  *moveMiddleEngine;

    void setConnectedEngine( SbBool yesOrNo );

    // When sharedAxle starts moving, we switch to the moveMiddleEngine.
    // When it's done, we change back.
    static void sharedAxleStartCB( void *, SoDragger *);
    static void sharedAxleFinishCB( void *, SoDragger *);

  private:

    // destructor
    virtual ~DoubleStrut();
};

////////////////////////////////////////////////////////////////////
//    Class: Wheel - 'origin' 'radius', and 'zRotation' control 
//                       internal xfs.
//                  - An axle in the middle, 
//                    a read-only (inert) axle, 
//                    a sizing ring and a rotator ring.
//
//    New fields in this subclass are:
//         radius         
//         zRotation         
//
////////////////////////////////////////////////////////////////////
class Wheel : public SimpleLink {

    SO_KIT_HEADER(Wheel);

    SO_KIT_CATALOG_ENTRY_HEADER(originAxle);
    SO_KIT_CATALOG_ENTRY_HEADER(inertAxle);
    SO_KIT_CATALOG_ENTRY_HEADER(offsetAngleRotator);
    SO_KIT_CATALOG_ENTRY_HEADER(rotatorRingSep);
    SO_KIT_CATALOG_ENTRY_HEADER(rotatorRingScaler);
    SO_KIT_CATALOG_ENTRY_HEADER(rotatorRing);
    SO_KIT_CATALOG_ENTRY_HEADER(sizingRing);

    SoSFFloat    offsetRotation;   // Rotation of inertAxle as offset from
				   // main axle: Default = 0
  public:

    virtual SbPList *getAxlePtrs();
    virtual void setZPlane( float newZ );

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFFloat    radius;           // Default = 1
    SoSFFloat    zRotation;        // Rotation of axle: Default = 0

    // Default is .35
    static void setMinRadius( float newMin ) { minRadius = newMin; }
    static float getMinRadius() { return minRadius; }


    // constructor
    Wheel();

  SoINTERNAL public:
    static void initClass();

  protected:
    WheelEngine              *wheelEngine;
    ZAngleFromRotationEngine *zAngleFromRotEngine;

    static void sizingRingCB( void *, SoDragger *);

  private:

    // destructor
    virtual ~Wheel();

    static float minRadius;
};

////////////////////////////////////////////////////////////////////
//    Class: Arrow     - defined by origin, dirPoint, and length.
//                    - An axle at the origin,
//                      an axle at a pivoting sleeve,
//                      an inert axle at the end,
//                      and a dragger to change the length.
//
//    New fields in this subclass are:
//         dirPoint         - location of point that determines direction.
//         length           - length of arrow.
//
////////////////////////////////////////////////////////////////////
class Arrow : public SimpleLink {

    SO_KIT_HEADER(Arrow);

    SO_KIT_CATALOG_ENTRY_HEADER(originAxle);
    SO_KIT_CATALOG_ENTRY_HEADER(dirPointAxle);
    SO_KIT_CATALOG_ENTRY_HEADER(inertAxle);
    SO_KIT_CATALOG_ENTRY_HEADER(sizingDragger);

  public:

    virtual SbPList *getAxlePtrs();
    virtual void setZPlane( float newZ );

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFVec3f    dirPoint;      // (default = (1,0,0))
    SoSFFloat    length;        // (default = 1.5)

    // constructor
    Arrow();

  SoINTERNAL public:
    static void initClass();

  protected:

    ArrowEngine *arrowEngine;

    static void sizingDraggerStartCB( void *, SoDragger *);
    static void sizingDraggerCB( void *, SoDragger *);
    static void sizingDraggerFinishCB( void *, SoDragger *);

  private:

    // destructor
    virtual ~Arrow();
};

////////////////////////////////////////////////////////////////////
//    Class: DoubleArrow
//
//    contains two Arrow nodes.
//
//    New nodes in this subclass are:
//      originAxle, endPointAxle
//      arrow1, arrow2
//
//    New fields in this subclass are:
//         endPoint   - location of second arrows's origin
//         size1      - size of the first arrow
//         size2      - size of the second arrow
//
////////////////////////////////////////////////////////////////////
class DoubleArrow : public LinkBase {

    SO_KIT_HEADER(DoubleArrow);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(arrow1);
    SO_KIT_CATALOG_ENTRY_HEADER(arrow2);

  public:

    virtual void setWorld( WorldInfo *newWorld );
    virtual void setZPlane( float newZ );
    virtual void undoAxleDependencies();
    virtual SbPList *getAxlePtrs();

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFVec3f    endPoint;

    // constructor
    DoubleArrow();

    virtual void errorColor( SbBool useErrorColor );

  SoINTERNAL public:
    static void initClass();

  protected:

  private:

    // destructor
    virtual ~DoubleArrow();
};

////////////////////////////////////////////////////////////////////
//    Class: Bell
//
//    Adds contains two Arrow nodes.
//
//    New nodes in this subclass are:
//      originAxle, endPointAxle
//      arrow1, arrow2
//
//    New fields in this subclass are:
//         endPoint   - location of second arrows's origin
//         size1      - size of the first arrow
//         size2      - size of the second arrow
//
////////////////////////////////////////////////////////////////////
class Bell : public DoubleArrow {

    SO_KIT_HEADER(Bell);

  public:

    SoSFString soundFileName;
    SoSFBool   overlapping;

    virtual SbBool undoConnections( SbBool doItAlways = FALSE );
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);

    // constructor
    Bell();

  SoINTERNAL public:
    static void initClass();

  protected:

    BellEngine *bellEngine;

    SoFieldSensor *overlappingSensor;
    static void overlappingSensorCB( void *, SoSensor *);

  private:
    SbBool wasOverlapping;

    // destructor
    virtual ~Bell();
};
#endif /* _SO_LINK_CLASSES_ */

