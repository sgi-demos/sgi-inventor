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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
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
 |      ZAngleFromRotationEngine
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/nodes/SoSwitch.h>

#include "LinkEngines.h"
#include "Sound.h"

/////////////////////////////////////////////////////////////
//  CheckingEngine
//
/////////////////////////////////////////////////////////////
SO_ENGINE_SOURCE(CheckingEngine);


void
CheckingEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(CheckingEngine, SoEngine, "Engine" );
}

CheckingEngine::CheckingEngine()
{
    SO_ENGINE_CONSTRUCTOR(CheckingEngine);

    isBuiltIn = TRUE;
}

CheckingEngine::~CheckingEngine() 
{
}
void 
CheckingEngine::sendOutBool( SoEngineOutput &outEngine, SbBool outVal )
{
    int num;
    SoFieldList fList;
    if ( (num = outEngine.getForwardConnections( fList )) )
	for ( int i = 0; i < num; i++ )
	    if ( ((SoSFBool *)fList[i])->getValue() != outVal ) {
		SO_ENGINE_OUTPUT(outEngine, SoSFBool, setValue( outVal ) );
		return;
	    }
}
void 
CheckingEngine::sendOutFloat( SoEngineOutput &outEngine, float outVal )
{
#define __SEND_SMALL_ .001
    int num;
    SoFieldList fList;
    if ( (num = outEngine.getForwardConnections( fList )) )
	for ( int i = 0; i < num; i++ )
	    if ( fabs(((SoSFFloat *)fList[i])->getValue() - outVal) 
		 > __SEND_SMALL_ ) {
		SO_ENGINE_OUTPUT(outEngine, SoSFFloat, setValue( outVal ) );
		return;
	    }
#undef __SEND_SMALL_
}
void 
CheckingEngine::sendOutLong( SoEngineOutput &outEngine, long outVal )
{
    int num;
    SoFieldList fList;
    if ( (num = outEngine.getForwardConnections( fList )) )
	for ( int i = 0; i < num; i++ )
	    if ( ((SoSFLong *)fList[i])->getValue() != outVal ) {
		SO_ENGINE_OUTPUT(outEngine, SoSFLong, setValue( outVal ) );
		return;
	    }
}
void 
CheckingEngine::sendOutRotation( SoEngineOutput &outEngine, SbRotation outVal )
{
#define __SEND_SMALL_ .001
    int num;
    SoFieldList fList;
    if ( (num = outEngine.getForwardConnections( fList )) )
	for ( int i = 0; i < num; i++ )
	    if ( !(((SoSFRotation *)fList[i])->getValue().equals(
						outVal,__SEND_SMALL_)) ) {
		SO_ENGINE_OUTPUT(outEngine, SoSFRotation, setValue( outVal ) );
		return;
	    }
#undef __SEND_SMALL_
}
void 
CheckingEngine::sendOutVec2f( SoEngineOutput &outEngine, SbVec2f outVal )
{
#define __SEND_SMALL_ .001
    int num;
    SoFieldList fList;
    if ( (num = outEngine.getForwardConnections( fList )) )
	for ( int i = 0; i < num; i++ )
	    if ( !(((SoSFVec2f *)fList[i])->getValue().equals(
						outVal,__SEND_SMALL_))) {
		SO_ENGINE_OUTPUT(outEngine, SoSFVec2f, setValue( outVal ) );
		return;
	    }
#undef __SEND_SMALL_
}
void 
CheckingEngine::sendOutVec3f( SoEngineOutput &outEngine, SbVec3f outVal )
{
#define __SEND_SMALL_ .001
    int num;
    SoFieldList fList;
    if ( (num = outEngine.getForwardConnections( fList )) )
	for ( int i = 0; i < num; i++ )
	    if ( !(((SoSFVec3f *)fList[i])->getValue().equals(
						outVal,__SEND_SMALL_))) {
		SO_ENGINE_OUTPUT(outEngine, SoSFVec3f, setValue( outVal ) );
		return;
	    }
#undef __SEND_SMALL_
}

/////////////////////////////////////////////////////////////
//  EncloseEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(EncloseEngine);

void
EncloseEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(EncloseEngine, CheckingEngine, "CheckingEngine" );
}

EncloseEngine::EncloseEngine()
{
    SO_ENGINE_CONSTRUCTOR(EncloseEngine);
    SO_ENGINE_ADD_INPUT(xMin,     (-1));
    SO_ENGINE_ADD_INPUT(xMax,      (1));
    SO_ENGINE_ADD_INPUT(yMin,     (-1));
    SO_ENGINE_ADD_INPUT(yMax,      (1));
    SO_ENGINE_ADD_INPUT(zMin,     (-1));
    SO_ENGINE_ADD_INPUT(zMax,      (1));
    SO_ENGINE_ADD_INPUT(xEnabled,  (1));
    SO_ENGINE_ADD_INPUT(yEnabled,  (1));
    SO_ENGINE_ADD_INPUT(zEnabled,  (1));
    SO_ENGINE_ADD_INPUT(relativeToMax,  (0));
    SO_ENGINE_ADD_INPUT(relativeToMin,  (0));

    SO_ENGINE_ADD_OUTPUT(translation,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(scaleFactor,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(switcher,         SoSFLong);
    isBuiltIn = TRUE;
}

EncloseEngine::~EncloseEngine()
{
}

void
EncloseEngine::evaluate()
{
    float xT, yT, zT, xS, yS, zS;
    xT = yT = zT = 0.0;
    xS = yS = zS = 1.0;

    SbBool scalesOk = TRUE;

    if ( xEnabled.getValue() ) {
	float min = xMin.getValue();
	float max = xMax.getValue();
	xT = (max + min) / 2.0;
	if (relativeToMin.getValue())
	    xT -= min;
	else if (relativeToMax.getValue())
	    xT -= max;
	xS = (max - min) / 2.0;
	if (max <= min) 
	    scalesOk = FALSE;
    }
    if ( yEnabled.getValue() ) {
	float min = yMin.getValue();
	float max = yMax.getValue();
	yT = (max + min) / 2.0;
	if (relativeToMin.getValue())
	    yT -= min;
	else if (relativeToMax.getValue())
	    yT -= max;
	yS = (max - min) / 2.0;
	if (max <= min) 
	    scalesOk = FALSE;
    }
    if ( zEnabled.getValue() ) {
	float min = zMin.getValue();
	float max = zMax.getValue();
	zT = (max + min) / 2.0;
	if (relativeToMin.getValue())
	    zT -= min;
	else if (relativeToMax.getValue())
	    zT -= max;
	zS = (max - min) / 2.0;
	if (max <= min) 
	    scalesOk = FALSE;
    }

    sendOutVec3f( translation, SbVec3f( xT, yT, zT) );

    if (xS == 0.0) xS = 1.0;
    if (yS == 0.0) yS = 1.0;
    if (zS == 0.0) zS = 1.0;
    sendOutVec3f( scaleFactor, SbVec3f( xS, yS, zS ) );

    // Can be used to enable/disable display of the bounded geometry.
    long newSwitchOut;
    if (scalesOk == TRUE)
	newSwitchOut = SO_SWITCH_ALL;
    else
	newSwitchOut = SO_SWITCH_NONE;
    sendOutLong(switcher,newSwitchOut);
}

/////////////////////////////////////////////////////////////
//  AxleFollowEngine
//
/////////////////////////////////////////////////////////////
SO_ENGINE_SOURCE(AxleFollowEngine);

void
AxleFollowEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(AxleFollowEngine, CheckingEngine, "CheckingEngine" );
}

AxleFollowEngine::AxleFollowEngine()
{
    SO_ENGINE_CONSTRUCTOR(AxleFollowEngine);
    SO_ENGINE_ADD_INPUT(leaderOrigin,     (SbVec3f(0,0,0)));

    SO_ENGINE_ADD_OUTPUT(newFollowerOrigin,      SoSFVec3f);
    isBuiltIn = TRUE;
}

AxleFollowEngine::~AxleFollowEngine()
{
}

void
AxleFollowEngine::evaluate()
{
    SbVec3f ldr = leaderOrigin.getValue();

    int num;
    SoFieldList fList;
    if ( (num = newFollowerOrigin.getForwardConnections( fList )) )
	for ( int i = 0; i < num; i++ ) {
	    SbVec3f curVal = ((SoSFVec3f *)fList[i])->getValue();
	    if ( curVal[0] != ldr[0] || curVal[1] != ldr[1] ) {
		SbVec3f outVal( ldr[0], ldr[1], curVal[2] );
		SO_ENGINE_OUTPUT(newFollowerOrigin, SoSFVec3f,setValue(outVal));
		return;
	    }
	}
}

/////////////////////////////////////////////////////////////
//  StrutEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(StrutEngine);

void
StrutEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(StrutEngine, CheckingEngine, "CheckingEngine" );
}

StrutEngine::StrutEngine()
{
    SO_ENGINE_CONSTRUCTOR(StrutEngine);
    SO_ENGINE_ADD_INPUT(origin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(endPoint,   (SbVec3f(1,0,0)));

    SO_ENGINE_ADD_OUTPUT(angle,      SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(endPointOffset,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(oneDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(twoDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(threeDScale,      SoSFVec3f);
    isBuiltIn = TRUE;
}

StrutEngine::~StrutEngine()
{
}

void
StrutEngine::evaluate()
{
    SbVec3f endPointDir = endPoint.getValue() - origin.getValue();

    // We are dealing only in the xy plane here. So zero out the z component
    // of the direction.
    endPointDir.setValue(endPointDir[0], endPointDir[1], 0.0);
    float endPointDist = endPointDir.normalize();

    // angle is the angle between (endPoint - origin) and (1,0,0)
    // It must be in the sense of rotation about (0,0,1), not (0,0,-1)
    // Find the rotation...
    SbRotation theRot( SbVec3f(1,0,0), endPointDir );

    // Decompose the rotation into angle and axis.
    SbVec3f theAxis;
    float   theAngle;
    theRot.getValue( theAxis, theAngle );

    // We want an angle about the positive Z axis.
    // If the axis is negative, we need to flip the angle.
    if ( theAxis[2] < 0 )
    theAngle *= -1.0;

    // Set angle based on our findings...
    sendOutFloat( angle, theAngle );

    float d = endPointDist;
    sendOutVec3f(endPointOffset, SbVec3f(d,0,0));
    sendOutVec3f(oneDScale, SbVec3f(d,1,1));
    sendOutVec3f(twoDScale, SbVec3f(d,d,1));
    sendOutVec3f(threeDScale, SbVec3f(d,d,d));
}

/////////////////////////////////////////////////////////////
//  RigidStrutEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(RigidStrutEngine);

void
RigidStrutEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(RigidStrutEngine, CheckingEngine, "CheckingEngine" );
}

RigidStrutEngine::RigidStrutEngine()
{
    SO_ENGINE_CONSTRUCTOR(RigidStrutEngine);
    SO_ENGINE_ADD_INPUT(origin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(offset,     (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(endPoint,   (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(isOffsetMode, (TRUE));

    SO_ENGINE_ADD_OUTPUT(angle,      SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(endPointOffset,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(oneDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(twoDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(threeDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(outEndPoint,      SoSFVec3f);
    isBuiltIn = TRUE;
}

RigidStrutEngine::~RigidStrutEngine()
{
}

void
RigidStrutEngine::evaluate()
{
    // If we are not in offset mode, recalculate offset based on 
    // the origin and endPoints
//XXX    if ( !isOffsetMode.getValue())
//XXX	offset = endPoint.getValue() - origin.getValue();
//XXX
//XXX    SbVec3f endPointDir = offset.getValue();
//XXX
    SbVec3f endPointDir;
    if ( !isOffsetMode.getValue())
        endPointDir = endPoint.getValue() - origin.getValue();
    else
        endPointDir = offset.getValue();
//XXX

    // We are dealing only in the xy plane here. So zero out the z component
    // of the direction.
    endPointDir.setValue(endPointDir[0], endPointDir[1], 0.0);
    float endPointDist = endPointDir.normalize();

    // angle is the angle between (endPoint - origin) and (1,0,0)
    // It must be in the sense of rotation about (0,0,1), not (0,0,-1)
    // Find the rotation...
    SbRotation theRot( SbVec3f(1,0,0), endPointDir );

    // Decompose the rotation into angle and axis.
    SbVec3f theAxis;
    float   theAngle;
    theRot.getValue( theAxis, theAngle );

    // We want an angle about the positive Z axis.
    // If the axis is negative, we need to flip the angle.
    if ( theAxis[2] < 0 )
    theAngle *= -1.0;

    // Set angle based on our findings...
    sendOutFloat( angle, theAngle );

    float d = endPointDist;
    sendOutVec3f(endPointOffset, SbVec3f(d,0,0));
    sendOutVec3f(oneDScale, SbVec3f(d,1,1));
    sendOutVec3f(twoDScale, SbVec3f(d,d,1));
    sendOutVec3f(threeDScale, SbVec3f(d,d,d));

    sendOutVec3f(outEndPoint, origin.getValue() + offset.getValue());
}

/////////////////////////////////////////////////////////////
//  DoubleStrutMoveEndEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(DoubleStrutMoveEndEngine);

void
DoubleStrutMoveEndEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(DoubleStrutMoveEndEngine, CheckingEngine, "CheckingEngine");
}

DoubleStrutMoveEndEngine::DoubleStrutMoveEndEngine()
{
    SO_ENGINE_CONSTRUCTOR(DoubleStrutMoveEndEngine);
    SO_ENGINE_ADD_INPUT(origin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(endPoint,     (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(size1,	      (1.0));
    SO_ENGINE_ADD_INPUT(size2,	      (1.0));
    SO_ENGINE_ADD_INPUT(sharedPoint,	      (SbVec3f(0,.5,0)));

    SO_ENGINE_ADD_OUTPUT(outSharedPoint,   SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(outError,         SoSFBool);
    isBuiltIn = TRUE;

    lastGoodSharedPoint.setValue( 0, .5, 0);
}

DoubleStrutMoveEndEngine::~DoubleStrutMoveEndEngine()
{
}

void
DoubleStrutMoveEndEngine::evaluate()
{
    // Find the outSharedPoint...

    //             sharedPoint
    //
    //         \./
    //         ...
    //     s1 . . . s2
    //       .  .  .
    //      .  h.   .
    //     .    .    .
    //   \............./
    //    o1    |    o2
    //    |     |      |
    //    |< a1>|< a2 >|
    //    |            |
    //    |<--  D ---->|
    // 
    // Derivation of h in terms of s1, s2, and D
    // (where s1 = size1, s2 = size2, and D = (o2-o1).normalize())
    //
    // s1*s1 = a1*a1 + h*h;   s2*s2 = a2*a2 + h*h;
    // D = a1 + a2;       a2 = D - a1;
    // s1*s1 = a1*a1 + h*h;   s2*s2 = (D-a1)*(D-a1) + h*h
    // a1 = (+/-) sqrt( s1*s1 - h*h);
    // s2*s2 = (D (+/-) sqrt( s1*s1 - h*h))*(D (+/-) sqrt( s1*s1 - h*h)) + h*h
    // s2*s2 = D*D (+/-) 2*D*sqrt(s1*s1-h*h) + (s1*s1-h*h) +h*h
    // s2*s2 = D*D (+/-) 2*D*sqrt(s1*s1-h*h) + s1*s1
    // 2*D*sqrt(s1*s1-h*h) = (+/-) (D*D - s2*s2 + s1*s1)
    //  sqrt( s1*s1 - h*h) = (+/-) ((D*D - s2*s2 + s1*s1) / (2*D))
    //  s1*s1 - h*h = (D*D - s2*s2 + s1*s1)* (D*D - s2*s2 + s1*s1) / (4*D*D)
    //  h*h = s1*s1 - ((D*D - s2*s2 + s1*s1)* (D*D - s2*s2 + s1*s1) / (4*D*D))
    // So, let:
    //    k1 = D*D - s2*s2 + s1*s1;
    //    k2 = 4*D*D;
    //    k3 = s1*s1;
    // Then,
    //    h*h = k3 - (k1*k1 / k2 );
    //    h = (+/-) sqrt( k3 - k1*k1/k2);

    float s1 = size1.getValue();    
    float s2 = size2.getValue();    

    SbVec3f DVec = endPoint.getValue() - origin.getValue();
    float D = DVec.normalize();

    float    k1 = D*D - s2*s2 + s1*s1;
    float    k2 = 4*D*D;
    float    k3 = s1*s1;

    SbBool gotError = FALSE;
    SbVec3f newSharedPoint;

    if (k2 == 0.0) {
	// Both origins are same location.  This is an error if (size1 != size2)
	if ( s1 != s2) 
	    gotError = TRUE;

	// Make newSharedPoint be a distance of size1 from origin.
	// For direction, use current direction to sharedPoint.
	SbVec3f myDir = sharedPoint.getValue() - origin.getValue();
	if (myDir == SbVec3f(0,0,0))
	    newSharedPoint = sharedPoint.getValue();
	else {
	    myDir.normalize();
	    newSharedPoint = origin.getValue() + s1 * myDir;
	}
    }
    else {
        float hSquared;
	hSquared = k3 - (k1 * k1 ) / k2;

	// We'll need a point to 'get closest to' if there is more than
	// one acceptable solution.
	int num;
	SoFieldList fList;
	SbVec3f comparePoint;
	if ( (num = outSharedPoint.getForwardConnections( fList )) ) {
	    // If there's an output, use:   curOut + (curOut - lastGood)
	    comparePoint = ((SoSFVec3f *)fList[0])->getValue();
	    comparePoint += (comparePoint - lastGoodSharedPoint);
	}
	else  {
	    // Otherwise, just stick close to the input.
	    comparePoint = sharedPoint.getValue();
	}

	if (hSquared >= 0.0) {

	    gotError = FALSE;

	    float h = sqrt( hSquared );
	    float a1 = sqrt( s1*s1 - hSquared);

	    SbVec3f zVec(0,0,1);
	    SbVec3f perpVec = zVec.cross( DVec );

	    // Determine if we want to use plus a1 or minus a1
	    // For this test, the sign of h doesn't matter.
		// Get the sharedPoints we find using pos and neg a1
		// By leaving origin1 and travelling along DVec and perpVec
		SbVec3f posA1Pt =origin.getValue() + DVec * a1 + perpVec * h;
		SbVec3f negA1Pt =origin.getValue() - DVec * a1 + perpVec * h;
		// How far is each from origin2?  Use the square of the dist.
		SbVec3f posDiff = posA1Pt - endPoint.getValue();
		SbVec3f negDiff = negA1Pt - endPoint.getValue();
		float posSq = posDiff.dot(posDiff);
		float negSq = negDiff.dot(negDiff);

		// Which is closer to square of size2?
		// If value from negative test is closer, use -a1
		float s2sq = s2 * s2;
		if ( fabs(negSq - s2sq) < fabs(posSq - s2sq) )
		    a1 *= -1;

	    // Get answers for both +h and -h. Both are correct.
	    // Use the one closer to the current value of the output.
	    // Or, if output differs from input, add in a 
	    // delta term base on change since last time for better feel.

		SbVec3f choice1 =origin.getValue() + DVec * a1 + perpVec * h;
		SbVec3f choice2 =origin.getValue() + DVec * a1 - perpVec * h;

		// Which choice is closer to the old value?
		SbVec3f diff1 = choice1 - comparePoint;
		SbVec3f diff2 = choice2 - comparePoint;
		float val1 = diff1[0] * diff1[0] + diff1[1] * diff1[1];
		float val2 = diff2[0] * diff2[0] + diff2[1] * diff2[1];
		newSharedPoint = (val1 < val2 ) ? choice1 : choice2;
	}
	else {
	    gotError = TRUE;
	    // Error of some sort. Make sharedPoint lie on line connecting 
	    // the points, at a distance of size1 from origin1.
	    float a1 = s1;
	    SbVec3f choice1, choice2;
	    choice1 = origin.getValue() + DVec * a1;
	    choice2 = origin.getValue() - DVec * a1;

	    // Which choice is closer to the comparePoint?

	    SbVec3f diff1 = choice1 - comparePoint;
	    SbVec3f diff2 = choice2 - comparePoint;
	    float val1 = diff1[0] * diff1[0] + diff1[1] * diff1[1];
	    float val2 = diff2[0] * diff2[0] + diff2[1] * diff2[1];
	    newSharedPoint = (val1 < val2 ) ? choice1 : choice2;
	}
    }

    if (gotError == FALSE && lastGoodSharedPoint != newSharedPoint)
	lastGoodSharedPoint = newSharedPoint;

    sendOutVec3f(outSharedPoint, newSharedPoint );
    sendOutBool(outError, gotError );
}

/////////////////////////////////////////////////////////////
//  DoubleStrutMoveMiddleEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(DoubleStrutMoveMiddleEngine);

void
DoubleStrutMoveMiddleEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(DoubleStrutMoveMiddleEngine, CheckingEngine, "CheckingEngine" );
}

DoubleStrutMoveMiddleEngine::DoubleStrutMoveMiddleEngine()
{
    SO_ENGINE_CONSTRUCTOR(DoubleStrutMoveMiddleEngine);
    SO_ENGINE_ADD_INPUT(origin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(endPoint,     (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(sharedPoint, (SbVec3f(0,.5,0)));

    SO_ENGINE_ADD_OUTPUT(outSize1,         SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(outSize2,         SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(outError,         SoSFBool);
    isBuiltIn = TRUE;
}

DoubleStrutMoveMiddleEngine::~DoubleStrutMoveMiddleEngine()
{
}

void
DoubleStrutMoveMiddleEngine::evaluate()
{
    // Figure out sizes based on distances between points.
    SbVec3f diff1 = sharedPoint.getValue() - origin.getValue();
    SbVec3f diff2 = sharedPoint.getValue() - endPoint.getValue();
    diff1.setValue( diff1[0], diff1[1], 0);
    diff2.setValue( diff2[0], diff2[1], 0);
    float newSize1 = diff1.length();
    float newSize2 = diff2.length();

    sendOutFloat( outSize1, newSize1 );
    sendOutFloat( outSize2, newSize2 );
    sendOutFloat( outError, FALSE );
}

/////////////////////////////////////////////////////////////
//  WheelEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(WheelEngine);

void
WheelEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(WheelEngine, CheckingEngine, "CheckingEngine" );
}

WheelEngine::WheelEngine()
{
    SO_ENGINE_CONSTRUCTOR(WheelEngine);
    SO_ENGINE_ADD_INPUT(origin,     (0,0,0));
    SO_ENGINE_ADD_INPUT(angle,     (0));
    SO_ENGINE_ADD_INPUT(offsetAngle,     (0));
    SO_ENGINE_ADD_INPUT(radius,     (1));

    SO_ENGINE_ADD_OUTPUT(totalRot,      SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(endPointOffset,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(oneDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(twoDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(threeDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(endPointLoc,      SoSFVec3f);
    isBuiltIn = TRUE;
}

WheelEngine::~WheelEngine()
{
}

void
WheelEngine::evaluate()
{
    float r = radius.getValue();

    sendOutVec3f(endPointOffset,SbVec3f(r,0,0));
    sendOutVec3f(oneDScale,SbVec3f(r,1,1));
    sendOutVec3f(twoDScale,SbVec3f(r,r,1));
    sendOutVec3f(threeDScale,SbVec3f(r,r,r));

    float zRot = angle.getValue() + offsetAngle.getValue();
    SbVec3f endPointDir( fcos( zRot), fsin( zRot), 0);
    SbVec3f newLoc = origin.getValue() + endPointDir * r;

    sendOutFloat(totalRot, zRot );
    sendOutVec3f(endPointLoc, newLoc );
}

/////////////////////////////////////////////////////////////
//  ArrowEngine
//
/////////////////////////////////////////////////////////////
    
SO_ENGINE_SOURCE(ArrowEngine);

void
ArrowEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(ArrowEngine, CheckingEngine, "CheckingEngine" );
}

ArrowEngine::ArrowEngine()
{
    SO_ENGINE_CONSTRUCTOR(ArrowEngine);
    SO_ENGINE_ADD_INPUT(origin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(dirPoint,   (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(length,     (1.5));

    SO_ENGINE_ADD_OUTPUT(angle,      SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(endPointOffset,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(oneDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(twoDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(threeDScale,      SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(endPointLoc,      SoSFVec3f);
    isBuiltIn = TRUE;
}

ArrowEngine::~ArrowEngine()
{
}

void
ArrowEngine::evaluate()
{
    SbVec3f endPointDir = dirPoint.getValue() - origin.getValue();

    // We are dealing only in the xy plane here. So zero out the z component
    // of the direction.
    endPointDir.setValue(endPointDir[0], endPointDir[1], 0.0);
    endPointDir.normalize();

    // angle is the angle between (dirPoint - origin) and (1,0,0)
    // It must be in the sense of rotation about (0,0,1), not (0,0,-1)
    // Find the rotation...
    SbRotation theRot( SbVec3f(1,0,0), endPointDir );

    // Decompose the rotation into angle and axis.
    SbVec3f theAxis;
    float   theAngle;
    theRot.getValue( theAxis, theAngle );

    // We want an angle about the positive Z axis.
    // If the axis is negative, we need to flip the angle.
    if ( theAxis[2] < 0 )
    theAngle *= -1.0;

    // Set angle based on our findings...
    sendOutFloat( angle, theAngle );

    float d = length.getValue();

    sendOutVec3f(endPointOffset,SbVec3f(d,0,0));
    sendOutVec3f(oneDScale,SbVec3f(d,1,1));
    sendOutVec3f(twoDScale,SbVec3f(d,d,1));
    sendOutVec3f(threeDScale,SbVec3f(d,d,d));

    SbVec3f newLoc = origin.getValue() + endPointDir * d;
    sendOutVec3f(endPointLoc, newLoc );
}


/////////////////////////////////////////////////////////////
//  DoubleArrowEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(DoubleArrowEngine);

void
DoubleArrowEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(DoubleArrowEngine, CheckingEngine, "CheckingEngine" );
}

DoubleArrowEngine::DoubleArrowEngine()
{
    SO_ENGINE_CONSTRUCTOR(DoubleArrowEngine);
    SO_ENGINE_ADD_INPUT(origin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(endPoint,     (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(size1,	      (1.0));
    SO_ENGINE_ADD_INPUT(size2,	      (1.0));

    SO_ENGINE_ADD_OUTPUT(outError,         SoSFBool);
    isBuiltIn = TRUE;
}

DoubleArrowEngine::~DoubleArrowEngine()
{
}

void
DoubleArrowEngine::evaluate()
{
    // If the hinge point is  further away than the length of the
    // link, change to errorColor...
    SbBool gotError = FALSE;
    float dist = (origin.getValue() - endPoint.getValue()).length();
    float s1 = size1.getValue();
    float s2 = size2.getValue();
    if ( dist > ( s1 + s2 ) )
	gotError = TRUE;
    if ( (dist < s1) || (dist < s2) )
	gotError = TRUE;

    sendOutBool(outError, gotError );
}

/////////////////////////////////////////////////////////////
//  BellEngine
//
/////////////////////////////////////////////////////////////
SO_ENGINE_SOURCE(BellEngine);

void
BellEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(BellEngine, CheckingEngine, "CheckingEngine" );
}

BellEngine::BellEngine()
{
    SO_ENGINE_CONSTRUCTOR(BellEngine);
    SO_ENGINE_ADD_INPUT(origin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(endPoint,     (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(size1,	      (1.0));
    SO_ENGINE_ADD_INPUT(size2,	      (1.0));

    SO_ENGINE_ADD_OUTPUT(overlapping,         SoSFBool);

    isBuiltIn = TRUE;
}

BellEngine::~BellEngine()
{
}

void
BellEngine::evaluate()
{
    // only evaluate if all four spatial parameters are connected.
    if ( !origin.isConnected() ||
         !endPoint.isConnected() ||
         !size1.isConnected() ||
         !size2.isConnected() ) {
	return;
    }

    SbBool isOverlapping = FALSE;
    SbVec3f endDiff = endPoint.getValue() - origin.getValue();
    float endLength = endDiff.length();
    float sizeSum = size1.getValue() + size2.getValue();
    isOverlapping = ( sizeSum > endLength ) ? TRUE : FALSE;

    sendOutBool(overlapping, isOverlapping );
}

/////////////////////////////////////////////////////////////
//  ZAngleFromRotationEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(ZAngleFromRotationEngine);

void
ZAngleFromRotationEngine::initClass()
{
    SO_ENGINE_INIT_CLASS(ZAngleFromRotationEngine, CheckingEngine, "CheckingEngine" );
}

ZAngleFromRotationEngine::ZAngleFromRotationEngine()
{
    SO_ENGINE_CONSTRUCTOR(ZAngleFromRotationEngine);
    SO_ENGINE_ADD_INPUT(inRotation,     (SbRotation::identity()));

    SO_ENGINE_ADD_OUTPUT(outAngle,      SoSFFloat);
    isBuiltIn = TRUE;
}

ZAngleFromRotationEngine::~ZAngleFromRotationEngine()
{
}

void
ZAngleFromRotationEngine::evaluate()
{
    // Decompose the rotation into angle and axis.
    SbVec3f theAxis;
    float   theAngle;
    SbRotation theRot = inRotation.getValue();
    theRot.getValue( theAxis, theAngle );

    // We want an angle about the positive Z axis.
    // If the axis is negative, we need to flip the angle. 
    if ( theAxis[2] < 0 )
	theAngle *= -1.0;

    sendOutFloat(outAngle, theAngle );
}
