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
    swirl.c++ - Support for swirling fields.

    Tim Heidmann
    December 22, 1993
*/

#include <Inventor/SbLinear.h>
#include <Inventor/SbTime.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef SDL_PORT
#include <device.h>
#endif
#include <swirl.h>
#include <sincos.h>


void
Swirl::addArm(int theAxis, float theRadius,
	float theHi_speed, float theNorm_speed,
	float theHi_decay, float theNorm_decay,
	float theHold_time) {

    if (nArms < MAXSWIRLARMS) {
	arm[nArms].phase = STILL;
	arm[nArms].theta = 0.0;
	arm[nArms].sign = (int) (random() & 0x1);
	arm[nArms].axis = theAxis;
	arm[nArms].radius = theRadius;
	arm[nArms].speed = 0.0;
	arm[nArms].hi_speed = theHi_speed;
	arm[nArms].norm_speed = theNorm_speed;
	arm[nArms].hi_decay = theHi_decay;
	arm[nArms].norm_decay = theNorm_decay;
	arm[nArms].hold_time = theHold_time;
	nArms++;
    }
}


void
Swirl::decay(float interval) {
    int i, a0, a1, a2;
    float r, ss, cc;

    play_time += interval;
    for (i=0; i<nArms; i++) {
	switch (arm[i].phase) {
	case STILL:
	    break;

	case ATTACK:
	    arm[i].speed -= interval * arm[i].hi_decay;
	    if (arm[i].speed < arm[i].norm_speed) {
		arm[i].phase = HOLD;
	    }
	    if (arm[i].sign) arm[i].theta += interval * arm[i].speed;
	    else             arm[i].theta -= interval * arm[i].speed;
	    break;

	case HOLD:
	    if (play_time > arm[i].hold_time) {
		arm[i].phase = DECAY;
	    }
	    if (arm[i].sign) arm[i].theta += interval * arm[i].speed;
	    else             arm[i].theta -= interval * arm[i].speed;
	    break;

	case DECAY:
	    arm[i].speed -= interval * arm[i].norm_decay;
	    if (arm[i].speed < 0.0) {
		arm[i].speed = 0.0;
		arm[i].sign = (int) (random() & 0x1);
		arm[i].phase = STILL;
	    } else
		if (arm[i].sign) arm[i].theta += interval * arm[i].speed;
		else             arm[i].theta -= interval * arm[i].speed;
	    break;

	}
    }
    
    center = pivot;
    for (i=0; i<(nArms-1); i++) {
	a0 = arm[i].axis;
	a1 = (a0+1) % 3;
	a2 = (a0+2) % 3;
	r = arm[i].radius;
	sincos.eval(arm[i].theta, ss, cc);
	center[a1] +=  r * cc + r * ss;
	center[a2] += -r * ss + r * cc;
    }
}


void
Swirl::attack(float interval, float value) {
    play_time = 0.0;
    for (int i = 0; i < nArms; i++) {
	arm[i].phase = ATTACK;
	arm[i].speed += value * arm[i].hi_decay;
	if (arm[i].speed > arm[i].hi_speed)
	    arm[i].speed = arm[i].hi_speed;
	if (arm[i].sign) arm[i].theta += interval * arm[i].speed;
	else             arm[i].theta -= interval * arm[i].speed;
    }
}
