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
 |   $Revision: 1.5 $
 |
 |   Description:
 |      Shot and puck classes (moving objects).
 | 
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>
#include <limits.h>
#include "MovingObject.h"

//////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
MovingObjectData::MovingObjectData(
    const SbVec2f &l, const SbVec2f &d, const SbTime &t)
//
//////////////////////////////////////////////////////////////////////////////
{
    collision.when = SbTime::max();
    setTimeZeroData(l, d, t);
}

//////////////////////////////////////////////////////////////////////////////
//
// Precomputes a bunch of constants for point-circle intersection routines
//
void
MovingObjectData::setTimeZeroData(
    const SbVec2f &l, const SbVec2f &d, const SbTime &t)
//
//////////////////////////////////////////////////////////////////////////////
{
    _loc0 = l;
    _dir0 = d;
    _time0 = t;
    
    _dirDotDir = _dir0.dot(_dir0);
    _locDotLoc = _loc0.dot(_loc0);
    _locDotDirX2 = 2.0 * _loc0.dot(_dir0);
}

//////////////////////////////////////////////////////////////////////////////
//
// Update the location of the moving object to reflect the passed time.
//
void
MovingObjectData::update(const SbTime &time)
//
//////////////////////////////////////////////////////////////////////////////
{
    // x = loc0.x + dir0.x * t
    // y = loc0.y + dir0.y * t
    //
    // where t is the amount of time since t0
    
    float t = time.getValue() - _time0.getValue();
    _loc = _loc0 + _dir0 * t;
}

//////////////////////////////////////////////////////////////////////////////
//
// Store info describing a collision this object will have with the rink.
//
void
MovingObjectData::setRinkCollision(
    const SbVec2f &where,
    const SbTime &when, 
    const SbVec2f &normal)
//
//////////////////////////////////////////////////////////////////////////////
{
    collision.where = where;
    collision.when = when;
    collision.normal = normal;
}

//////////////////////////////////////////////////////////////////////////////
//
// Compute the time at which the shot will leave the rink.
//
ShotData::ShotData(
    const SbVec2f &l,
    const SbVec2f &d,
    const SbTime &t) : MovingObjectData(l, d, t)
//
//////////////////////////////////////////////////////////////////////////////
{
    _force = 1;
}

//////////////////////////////////////////////////////////////////////////////
//
// A puck has mass and radius.
//
PuckData::PuckData(
    const SbVec2f &l,
    const SbVec2f &d,
    const SbTime &t) : MovingObjectData(l, d, t)
//
//////////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    _mass = 50; //??? this makes it move slowly for testing...
#else
    _mass = 10;
#endif
    _radius = 1;
    _radiusXradius = 1;
}

//////////////////////////////////////////////////////////////////////////////
//
// Set the puck radius and precompute (radius * radius).
//
void
PuckData::setRadius(int r)
//
//////////////////////////////////////////////////////////////////////////////
{ 
    _radius = r;
    _radiusXradius = r * r;
}


