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
    sincos.c++ - Quick sin/cosine

    Tim Heidmann
    December 12, 1993
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sincos.h>

SinCos sincos(500);

SinCos::SinCos(int ii) : n(ii) {
    float th, dth;
    int i;

    s = (float *) malloc((n+1) * sizeof(float));
    dth = M_PI / 2.0 / n;
    for (i=0, th = 0.0; i<=n; i++, th += dth)
	s[i] = fsin(th);
}

SinCos::~SinCos() {
    free(s);
}

void
SinCos::eval(float th, float &ss, float &cc) {
    int i, rem, quad;

    i = (int) (th * n);
    rem = i % n;
    quad = i / n;
    if (rem < 0) {
	rem += n;
	quad -= 1;
    }
    quad %= 4;
    if (quad < 0) quad += 4;

    switch (quad) {
    case 0: ss =  s[    rem]; cc =  s[n - rem]; break;
    case 1: ss =  s[n - rem]; cc = -s[    rem]; break;
    case 2: ss = -s[    rem]; cc = -s[n - rem]; break;
    case 3: ss = -s[n - rem]; cc =  s[    rem]; break;
    }
}
