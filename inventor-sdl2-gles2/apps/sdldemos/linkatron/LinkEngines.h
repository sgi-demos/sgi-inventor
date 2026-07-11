//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Description:
 |	This file contains the declaration of the engines used by 
 |      linkages
 |
 |   Classes:
 |	EncloseEngine
 |	AxleFollowEngine
 |	StrutEngine
 |	RigidStrutEngine
 |	DoubleStrutMoveEndEngine
 |	DoubleStrutMoveMiddleEngine
 |	WheelEngine
 |	ArrowEngine
 |	DoubleArrowEngine
 |	BellEngine
 |	ZAngleFromRotationEngine
 |
 |   Author(s)		: Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_LINK_ENGINES_
#define  _SO_LINK_ENGINES_

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFLong.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFString.h>

/////////////////////////////////////////////////////////////
//  CheckingEngine
//
// Checks that values differ before sending to output.
//
/////////////////////////////////////////////////////////////
class CheckingEngine : public SoEngine {
    SO_ENGINE_ABSTRACT_HEADER(CheckingEngine);	// SDL port: abstract base
  public:

    static void sendOutBool( SoEngineOutput &outEngine, SbBool outVal );
    static void sendOutFloat( SoEngineOutput &outEngine, float outVal );
    static void sendOutLong( SoEngineOutput &outEngine, long outVal );
    static void sendOutRotation( SoEngineOutput &outEngine, SbRotation outVal );
    static void sendOutVec2f( SoEngineOutput &outEngine, SbVec2f outVal );
    static void sendOutVec3f( SoEngineOutput &outEngine, SbVec3f outVal );

    CheckingEngine();

  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~CheckingEngine();
};

/////////////////////////////////////////////////////////////
//  EncloseEngine
//
// Outpuyts the translation and scaleFactor to make a 2x2x2 cube
// fit within the given boundaries.
//
// If relativeToMax is set, then translation will be relative to 
// xMax, yMax, zMax.
// Else if relativeToMin is set, then translation will be relative to 
// xMin, yMin, zMin.
//
/////////////////////////////////////////////////////////////
class EncloseEngine : public CheckingEngine {
    SO_ENGINE_HEADER(EncloseEngine);
  public:

    // Inputs
    // Default: all enabled, ranges (-1,1)
    SoSFFloat	xMin, xMax, yMin, yMax, zMin, zMax;
    SoSFBool  	xEnabled,   yEnabled,   zEnabled;
    SoSFBool  	relativeToMax, relativeToMin;
    
    // Output
    // The translation and scaleFactor to make a 2x2x2 cube
    // fit within the given boundaries.
    SoEngineOutput translation;	     // (SoSFVec3f) 
    SoEngineOutput scaleFactor;	     // (SoSFVec3f) 
    //Puts out either SO_SWITCH_ALL if every enabled Max > its Min,
    // else SO_SWITCH_NONE
    SoEngineOutput switcher;	     // (SoSFLong) 
    
    // Constructor
    EncloseEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~EncloseEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  AxleFollowEngine
//
/////////////////////////////////////////////////////////////
class AxleFollowEngine : public CheckingEngine {
    SO_ENGINE_HEADER(AxleFollowEngine);
  public:

    // Inputs
    SoSFVec3f	leaderOrigin;
    
    // Outputs
    SoEngineOutput newFollowerOrigin;	         // (SoSFVec3f) 
    
    // Constructor
    AxleFollowEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor

    ~AxleFollowEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  StrutEngine
//
/////////////////////////////////////////////////////////////
class StrutEngine : public CheckingEngine {
    SO_ENGINE_HEADER(StrutEngine);
  public:

    // Inputs
    SoSFVec3f	origin;
    SoSFVec3f	endPoint;
    
    // Outputs
    SoEngineOutput angle;	         // (SoSFFloat) 
    SoEngineOutput endPointOffset;	 // (SoSFVec3f) 
    SoEngineOutput oneDScale;	         // (SoSFVec3f) 
    SoEngineOutput twoDScale;	         // (SoSFVec3f) 
    SoEngineOutput threeDScale;	         // (SoSFVec3f) 
    
    // Constructor
    StrutEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~StrutEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  RigidStrutEngine
//
/////////////////////////////////////////////////////////////
class RigidStrutEngine : public CheckingEngine {
    SO_ENGINE_HEADER(RigidStrutEngine);
  public:

    // Inputs
    SoSFVec3f	origin;
    SoSFVec3f	offset;
    SoSFVec3f	endPoint;
    SoSFBool	isOffsetMode; // (default = TRUE)
    
    // Outputs
    SoEngineOutput angle;	         // (SoSFFloat) 
    SoEngineOutput endPointOffset;	 // (SoSFVec3f) 
    SoEngineOutput oneDScale;	         // (SoSFVec3f) 
    SoEngineOutput twoDScale;	         // (SoSFVec3f) 
    SoEngineOutput threeDScale;	         // (SoSFVec3f) 
    SoEngineOutput outEndPoint;	         // (SoSFVec3f) 
    
    // Constructor
    RigidStrutEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~RigidStrutEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  DoubleStrutMoveEndEngine
//
/////////////////////////////////////////////////////////////
class DoubleStrutMoveEndEngine : public CheckingEngine {
    SO_ENGINE_HEADER(DoubleStrutMoveEndEngine);
  public:

    // Inputs
    SoSFVec3f	origin;
    SoSFVec3f	endPoint;
    SoSFFloat	size1;
    SoSFFloat	size2;
    SoSFVec3f	sharedPoint;
    
    // Outputs
    // outError is TRUE if size is too short to reach the hinge point.
    SoEngineOutput outSharedPoint;   // (SoSFVec3f) 
    SoEngineOutput outError;	     // (SoSFBool) 
    
    // Constructor
    DoubleStrutMoveEndEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:

  protected:		// SDL port: derived classes need the dtor
    ~DoubleStrutMoveEndEngine();
    virtual void evaluate();

    SbVec3f lastGoodSharedPoint;
};

/////////////////////////////////////////////////////////////
//  DoubleStrutMoveMiddleEngine
//
/////////////////////////////////////////////////////////////
class DoubleStrutMoveMiddleEngine : public CheckingEngine {
    SO_ENGINE_HEADER(DoubleStrutMoveMiddleEngine);
  public:

    // Inputs
    SoSFVec3f	origin;
    SoSFVec3f	endPoint;
    SoSFVec3f	sharedPoint;
    
    // Outputs
    // outError is TRUE if size is too short to reach the hinge point.
    SoEngineOutput outSize1;         // (SoSFFloat) 
    SoEngineOutput outSize2;         // (SoSFFloat) 
    SoEngineOutput outError;	     // (SoSFBool) 
    
    // Constructor
    DoubleStrutMoveMiddleEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~DoubleStrutMoveMiddleEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  WheelEngine
//
/////////////////////////////////////////////////////////////
class WheelEngine : public CheckingEngine {
    SO_ENGINE_HEADER(WheelEngine);
  public:

    // Inputs
    SoSFVec3f	origin;
    SoSFFloat	angle;
    SoSFFloat	offsetAngle;
    SoSFFloat	radius;
    
    // Outputs
    SoEngineOutput totalRot;	 // (SoSFFloat) 
    SoEngineOutput endPointOffset;	 // (SoSFVec3f) 
    SoEngineOutput oneDScale;	         // (SoSFVec3f) 
    SoEngineOutput twoDScale;	         // (SoSFVec3f) 
    SoEngineOutput threeDScale;	         // (SoSFVec3f) 
    SoEngineOutput endPointLoc;	         // (SoSFVec3f) 
    
    // Constructor
    WheelEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~WheelEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  ArrowEngine
//
/////////////////////////////////////////////////////////////
class ArrowEngine : public CheckingEngine {
    SO_ENGINE_HEADER(ArrowEngine);
  public:

    // Inputs
    SoSFVec3f	origin;
    SoSFVec3f	dirPoint;
    SoSFFloat	length;
    
    // Outputs
    SoEngineOutput angle;	         // (SoSFFloat) 
    SoEngineOutput endPointOffset;	 // (SoSFVec3f) 
    SoEngineOutput oneDScale;	         // (SoSFVec3f) 
    SoEngineOutput twoDScale;	         // (SoSFVec3f) 
    SoEngineOutput threeDScale;	         // (SoSFVec3f) 
    SoEngineOutput endPointLoc;	         // (SoSFVec3f) 
    
    // Constructor
    ArrowEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~ArrowEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  DoubleArrowEngine
//
/////////////////////////////////////////////////////////////
class DoubleArrowEngine : public CheckingEngine {
    SO_ENGINE_HEADER(DoubleArrowEngine);
  public:

    // Inputs
    SoSFVec3f	origin;
    SoSFVec3f	endPoint;
    SoSFFloat	size1;
    SoSFFloat	size2;
    
    // Outputs
    // outError is TRUE if origins are too far or too close to each other.
    SoEngineOutput outError;	     // (SoSFBool) 
    
    // Constructor
    DoubleArrowEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~DoubleArrowEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  BellEngine
//
/////////////////////////////////////////////////////////////
class BellEngine : public CheckingEngine {
    SO_ENGINE_HEADER(BellEngine);
  public:

    // Inputs
    SoSFVec3f	origin;
    SoSFVec3f	endPoint;
    SoSFFloat	size1;
    SoSFFloat	size2;
    
    // Outputs
    // none.
    SoEngineOutput overlapping;	     // (SoSFBool) 
    
    // Constructor
    BellEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~BellEngine();
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  ZAngleFromRotationEngine
//
/////////////////////////////////////////////////////////////
class ZAngleFromRotationEngine : public CheckingEngine {
    SO_ENGINE_HEADER(ZAngleFromRotationEngine);
  public:

    // Inputs
    SoSFRotation   inRotation;
    
    // Outputs
    SoEngineOutput outAngle;      // (SoSFFloat)
    
    // Constructor
    ZAngleFromRotationEngine();
    
  SoINTERNAL public:
    static void initClass();
  protected:		// SDL port: derived classes need the dtor
    ~ZAngleFromRotationEngine();
    virtual void evaluate();
};
#endif  /* _SO_LINK_ENGINES_ */
