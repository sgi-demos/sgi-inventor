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
    flakeset.c++ - Snowflake particle system for a Christmas card

    Tim Heidmann
    December 15, 1993
*/

#include <Inventor/SbLinear.h>
#include <Inventor/SbTime.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <string.h>
#include <math.h>
#include <swirl.h>
#include <sincos.h>
#include <flakeset.h>


FlakeSet::FlakeSet(int iFlakes, float fallMin, float fallRange) {
    int i;

    // Create a bunch of flakes in -1 < x,y,z < 1
    nFlakes = iFlakes;
    pos = (SbVec3f *) malloc(sizeof(SbVec3f) * nFlakes);
    randpos = (SbVec3f *) malloc(sizeof(SbVec3f) * nFlakes);
    drawpos = (SbVec3f *) malloc(sizeof(SbVec3f) * nFlakes);
    fallRate = (float *) malloc(sizeof(float) * nFlakes);
    for (i = 0; i < nFlakes; i++) {
	// Create flakes in hemi-sphere y > 0, r < 1
	do
	    randpos[i].setValue(
		(random() & 65535) / 32768.0 - 1.0,
		(random() & 65535) / 65536.0,
		(random() & 65535) / 32768.0 - 1.0);
	while (randpos[i].length() >= 1.2);

	// Initially position the flakes randomly on the ground
	pos[i] = randpos[i];
	pos[i][1] *= 0.01;

	// Generate a non-linear distribution of speeds
	float r = (random() & 65535) / 65536.0;
	fallRate[i] = fallMin + fallRange * r * r;
    }
}

FlakeSet::~FlakeSet() {
    free(pos);
    free(drawpos);
    free(fallRate);
}

SbVec3f v0(-0.01, -0.01, 0.0), v1(0.00,  0.01, 0.0), v2( 0.01, -0.01, 0.0);

void
FlakeSet::draw() {
    int i;
    SbVec3f vx0, vx1, vx2;

    for (i=0; i<nFlakes; i++) {
	vx0 = v0 + drawpos[i];
	vx1 = v1 + drawpos[i];
	vx2 = v2 + drawpos[i];
	glBegin(GL_POLYGON);
	glVertex3fv(vx0.getValue());
	glVertex3fv(vx1.getValue());
	glVertex3fv(vx2.getValue());
    	glEnd();
    }
}


void
FlakeSet::swirl(Swirl &s, float interval) {

    SbVec3f origin;
    float soi, dist, spdfactor;
    int i0, i1, i2;

    int i;
    SbVec3f d1, d2(0.0, 0.0, 0.0);
    float spd;
    float ss, cc;

    s.getValues(origin, i0, soi, spdfactor);
    i1 = (i0+1)%3; i2 = (i0+2)%3;
    spdfactor *= interval;

    for (i = 0; i < nFlakes; i++) {
	d1 = pos[i] - origin;
	FASTDIST(dist, d1[0], d1[1], d1[2]);
	spd = soi - dist;
	if (spd >= 0) {
	    spd *= spdfactor;
	    sincos.eval(spd, ss, cc);
	    d2[i1] =  cc * d1[i1] + ss * d1[i2];
	    d2[i0] = d1[i0];
	    d2[i2] = -ss * d1[i1] + cc * d1[i2];
	    pos[i] = d2 + origin;
	}
    }
}

void
FlakeSet::fall(float interval) {
    int i;
    for (i=0; i < nFlakes; i++) {
	if (pos[i][1] > 0.01) {
	    pos[i][1] -= fallRate[i] * interval;
	}
     }
}



float lim_d0 = 0.9, lim_dl = 1.0, lim_px = 1.2;
float lim_k = 0.25 / (lim_dl - lim_d0);
float lim_dx = lim_dl + lim_dl - lim_d0;

void
FlakeSet::limit() {
    int i;
    float m, f, delta;

    for (i=0; i < nFlakes; i++) {
	drawpos[i] = pos[i];
	if (pos[i][1] < 0.0) {
	    drawpos[i][1] = 0.0;
	    if (pos[i][1] < -0.5) pos[i][1] -= 0.2;
	}

	m = pos[i].length();
	if (m > lim_d0) {
	    if (m > lim_dx)
		f = lim_dl / m;
	    else {
		delta = lim_dx - m;
		f = (1.0 - lim_k * delta * delta) / m;
	    }

	    drawpos[i] *= f;
	    if (m > lim_px) pos[i] *= 0.9;
	}
    }
}


void
FlakeSet::randomize(float f) {
    int i;
    for (i=0; i < nFlakes; i++) {
	// pos[i] += f * (randpos[i] - pos[i]);
	pos[i][0] += f * ((random() & 65535) / 32768.0 - 1.0);
	pos[i][1] += f * ((random() & 65535) / 32768.0 - 1.0);
	pos[i][2] += f * ((random() & 65535) / 32768.0 - 1.0);
	if (pos[i][1] < 0.0) pos[i][1] = 0.0;
    }
}
