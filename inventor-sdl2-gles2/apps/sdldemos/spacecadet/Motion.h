////////////////////////////////////////////////////////////////////////////
//
// Struct: Motion
//
// Interval of motion for a ship or a blast. Contains the start and
// end positions of the object, as well as the direction and time
// interval. Other information indicates any changes occurring during
// motion, as when the object hits something or something explodes
// near it, etc..
//
////////////////////////////////////////////////////////////////////////////

// This file can get included more than once
#ifndef _MOTION_INCLUDED_
#define _MOTION_INCLUDED_

#include <Inventor/SbLinear.h>

struct Motion {
    SbVec3f	position0;	// Position at start of motion interval
    SbVec3f	position1;	// Position at end of motion interval
    SbVec3f	forward;	// Direction of forward motion
    SbVec3f	up;		// "Up" with respect to motion
    float	numSeconds;	// Time length of interval
    SbBool	exited;		// Whether object exited tunnel
    SbBool	hit;		// Whether object hit something

    // These are all accumulated effects of things in a tunnel:
    float	explosionForce;	// Force of explosions near object
    float	addedEnergy;	// Energy added to object
    float	warpDistance;	// Distance remaining in warp mode
    float	stealthTime;	// Seconds remaining in stealth mode

    // These are set if the object hits something:
    float	t;		 // Parametric distance where hit
				 // occurred (0 - 1 along interval)
    SbVec3f	positionAfterHit; // New position object should have
    SbVec3f	forwardAfterHit;  // New direction object should take
};

#endif /* _MOTION_INCLUDED_ */
