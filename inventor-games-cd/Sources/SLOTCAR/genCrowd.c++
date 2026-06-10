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
 |   $Revision: 1.3 $
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
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoInfo.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoPointSet.h>

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
    fprintf(stderr, "\nUsage:  genCrowd [-n numPoints] [-h] [-o outFile]\n");
    fprintf(stderr, "    [-n numPoints] :  Number of people.\n");
    fprintf(stderr, "    [-o outFile]   :  Output Inventor file.\n");
    fprintf(stderr, "    [-h]           :  Print out this message.\n");
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
    int numPoints = 500;
    char outFile[128];

    int err = 0;	// Flag: error in options?
    int c;

    // Note: optind and optarg are declared in getopt.h
    while ((c = getopt(argc, argv, "n:o:h")) != -1)
    {
	switch(c)
	{
	  case 'n':
	    numPoints = atoi(optarg);
            if (numPoints < 1) err = 1;
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
    // some coordinates, and a point set.
    SoSeparator *root        = new SoSeparator;
    SoCoordinate3 *coords    = new SoCoordinate3;
    SoBaseColor *clrs        = new SoBaseColor;
    SoDrawStyle *dstyle      = new SoDrawStyle;
    SoInfo *info             = new SoInfo;
    SoPointSet *points       = new SoPointSet;

    root->ref();
    root->addChild(info);
    root->addChild(coords);
    root->addChild(clrs);
    root->addChild(dstyle);
    root->addChild(points);

    // Generate an informational string containing the genCrowd command
    // used to generate the crowd.
    char str[32];
    sprintf(str, "genCrowd -n %d", numPoints);
    info->string.setValue(str);

    // Make the two colors to switch between for the trees
    float clr[1][3];
    clr[0][0] = 0.8;
    clr[0][1] = 0.8;
    clr[0][2] = 0.8;
    clrs->rgb.setValues(0, 1, clr);
    dstyle->pointSize = 2.0;

    // Generate the coordinates for the trees by randomly placing them 
    // in an oval with random heights.
    float pt[3];
    float x, y, z;
    for (int i = 0; i<numPoints; i++) {

        // Pick a random location within the oval.
        x = (drand48()-0.5) * 295.0;
        z = drand48() * 65.0 + 8.0;
        y = 11.0 + (-50.0/75.0) * z + 50.0;

        pt[0] = x;
        pt[1] = y;
        pt[2] = z;

        coords->point.setValues(i, 1, &pt);
    }
    points->numPoints = numPoints;

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
