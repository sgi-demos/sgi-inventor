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
 * Copyright (C) 1990,91,92,93,94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      main()
 |
 |   Author(s)  : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoTriangleStripSet.h>

#define BASE_HEIGHT 0.0

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Print out a usage message.
//

void
usage()

//
////////////////////////////////////////////////////////////////////////
{
    fprintf(stderr, "\nUsage:  genForest [-r radius] [-e ellipse] [-h] "
		    " [-o outFile] [-n numTrees] [-s scale]\n");
    fprintf(stderr, "    [-n numTrees]:  Number of trees in forest.\n");
    fprintf(stderr, "    [-r radius]  :  Radius of forest.\n");
    fprintf(stderr, "    [-e ellipse] :  Stretch of the oval.\n");
    fprintf(stderr, "    [-s scale]   :  Scale the trees.\n");
    fprintf(stderr, "    [-o outFile] :  The output Inventor file\n");
    fprintf(stderr, "    [-h]         :  Print out this message.\n");
    exit(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is the mainline program for the genForest game.
//

main( int argc, char *argv[] )

//
////////////////////////////////////////////////////////////////////////
{
    SoDB::init();

    // Parse the command line
    int numTrees = 10;
    float radius = 100.0;
    float ovalStretch = 1.0;
    float scale = 1.0;
    char outFile[128];

    int err = 0;	// Flag: error in options?
    int c;

    // Note: optind and optarg are declared in getopt.h
    while ((c = getopt(argc, argv, "e:n:r:s:o:r:h")) != -1)
    {
	switch(c)
	{
	  case 'n':
	    numTrees = atoi(optarg);
            if (numTrees < 1) err = 1;
	    break;
	  case 'r':
	    radius = atof(optarg);
            if (radius <= 0.0) err = 1;
	    break;
	  case 'e':
	    ovalStretch = atof(optarg);
            if (ovalStretch <= 0.0) err = 1;
	    break;
	  case 's':
	    scale = atof(optarg);
            if (scale <= 0.0) err = 1;
	    break;
	  case 'o':
	    strcpy (outFile, optarg);
	    break;
	  case 'h':	// Help
	  default:
	    err = 1;
	    break;
	}
    }
    if (err)   {
	usage();
	exit(1);
    }

    // Init random-number generator:
    srand48((long)getpid());

    // Create a scene graph containing a few base colors,
    // some coordinates, and a triangle strip set.
    SoSeparator *root = new SoSeparator;
    SoCoordinate3 *coords = new SoCoordinate3;
    SoBaseColor *clrs = new SoBaseColor;
    SoMaterialBinding *mtlBind = new SoMaterialBinding;
    SoTriangleStripSet *strip = new SoTriangleStripSet;

    root->ref();
    root->addChild(coords);
    root->addChild(clrs);
    root->addChild(mtlBind);
    root->addChild(strip);

    // Make the two colors to switch between for the trees
    float clr[2][3];
    clr[0][0] = 0.0;
    clr[0][1] = 0.10;
    clr[0][2] = 0.04;
    clr[1][0] = 0.0;
    clr[1][1] = 0.15;
    clr[1][2] = 0.05;
    clrs->rgb.setValues(0, 2, clr);
    mtlBind->value = SoMaterialBinding::PER_PART;

    // Generate the coordinates for the trees by randomly placing them 
    // in an oval with random heights.
    float pt[3][3];
    float height, width;
    float r, a, x, z;
    for (int i = 0; i<numTrees; i++) {

        // Pick a random location within the oval.
        r = drand48() * radius;
        a = drand48() * M_PI_2;
        x = r * cosf(a) * ovalStretch;
        z = r * sinf(a);

        // The trees will range from 15.0 to 25.0 units tall
        height = scale * (60.0 + (drand48()-0.5) * 20.0);

        // The base of the tree will range from 4.0 to 12.0
        width  = scale * (20.0  + (drand48()-0.5) * 6.0);

        pt[0][0] = x - width;
        pt[0][1] = BASE_HEIGHT;
        pt[0][2] = z;

        pt[1][0] = x + width;
        pt[1][1] = BASE_HEIGHT;
        pt[1][2] = z;

        pt[2][0] = x;
        pt[2][1] = height;
        pt[2][2] = z;

        coords->point.setValues(3*i, 3, pt);
        strip->numVertices.set1Value(i, 3);
    }

    // Write out the scene graph
    SoWriteAction wa;

    if (!(wa.getOutput()->openFile(outFile))) {
        fprintf(stderr, "ERROR:  Could not open %s\n", outFile);
        exit(-1);
    }

    wa.apply(root);
    wa.getOutput()->closeFile();

    root->unref();
}
