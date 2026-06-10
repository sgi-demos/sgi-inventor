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
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes	: TsField::buildBlock
 |
 |   Author	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

//--------------------------- Include ----------------------------------
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoNormal.h>
#include "TsField.h"


////////////////////////////////////////////////////////////////////////
//
// Description:
//	Initialize the Inventor data representing the block.  The block
//      is the basic unit from which all of the pieces are built.  It
//      is instanced for all pieces and the field.  The block is centered at
//      (1, 1, -1) and as length of 2.
//
// Use: private

void
TsField::buildBlock()

//
////////////////////////////////////////////////////////////////////////
{
    SoCoordinate3    *coords;
    SoIndexedFaceSet *faces;
    SoNormal         *normals;
    SbVec3f          *verts, tcoord;
    long             *indices, *ind;
    int              i;

    block = new SoSeparator();
    block->renderCaching = SoSeparator::ON;
    block->ref();

    verts = new SbVec3f[24];
    // Front
    verts[0].setValue  ( 0.1,  0.1,  0.0);
    verts[1].setValue  ( 1.9,  0.1,  0.0);
    verts[2].setValue  ( 1.9,  1.9,  0.0);
    verts[3].setValue  ( 0.1,  1.9,  0.0);
    // Right
    verts[4].setValue  ( 2.0,  0.1, -0.1);
    verts[5].setValue  ( 2.0,  0.1, -1.9);
    verts[6].setValue  ( 2.0,  1.9, -1.9);
    verts[7].setValue  ( 2.0,  1.9, -0.1);
    // Back
    verts[8].setValue  ( 1.9,  0.1, -2.0);
    verts[9].setValue  ( 0.1,  0.1, -2.0);
    verts[10].setValue ( 0.1,  1.9, -2.0);
    verts[11].setValue ( 1.9,  1.9, -2.0);
    // Left
    verts[12].setValue ( 0.0,  0.1, -1.9);
    verts[13].setValue ( 0.0,  0.1, -0.1);
    verts[14].setValue ( 0.0,  1.9, -0.1);
    verts[15].setValue ( 0.0,  1.9, -1.9);
    // Bottom
    verts[16].setValue ( 0.1,  0.0, -1.9);
    verts[17].setValue ( 1.9,  0.0, -1.9);
    verts[18].setValue ( 1.9,  0.0, -0.1);
    verts[19].setValue ( 0.1,  0.0, -0.1);
    // Top
    verts[20].setValue ( 0.1,  2.0, -0.1);
    verts[21].setValue ( 1.9,  2.0, -0.1);
    verts[22].setValue ( 1.9,  2.0, -1.9);
    verts[23].setValue ( 0.1,  2.0, -1.9);
    coords = new SoCoordinate3();
    coords->point.setValues (0, 24, verts);
    tcoord.setValue (0.0, 0.0, 1.0);
    verts[0] = tcoord;
    verts[1] = tcoord;
    verts[2] = tcoord;
    verts[3] = tcoord;
    tcoord.setValue (1.0, 0.0, 0.0);
    verts[4] = tcoord;
    verts[5] = tcoord;
    verts[6] = tcoord;
    verts[7] = tcoord;
    tcoord.setValue (0.0, 0.0, -1.0);
    verts[8] = tcoord;
    verts[9] = tcoord;
    verts[10] = tcoord;
    verts[11] = tcoord;
    tcoord.setValue (-1.0, 0.0, 0.0);
    verts[12] = tcoord;
    verts[13] = tcoord;
    verts[14] = tcoord;
    verts[15] = tcoord;
    tcoord.setValue (0.0, -1.0, 0.0);
    verts[16] = tcoord;
    verts[17] = tcoord;
    verts[18] = tcoord;
    verts[19] = tcoord;
    tcoord.setValue (0.0, 1.0, 0.0);
    verts[20] = tcoord;
    verts[21] = tcoord;
    verts[22] = tcoord;
    verts[23] = tcoord;
    normals = new SoNormal();
    normals->vector.setValues (0, 24, verts);
    delete verts;

    indices = new long[30+60+32];
    ind = indices;
    for (i=0; i<6; i++)
    {
        *ind++ = i*4;
        *ind++ = i*4+1;
        *ind++ = i*4+2;
        *ind++ = i*4+3;
        *ind++ = -1;
    }
    *ind++ = 13;  *ind++ = 12;  *ind++ = 16;  *ind++ = 19;  *ind++ = -1;
    *ind++ = 18;  *ind++ = 17;  *ind++ =  5;  *ind++ =  4;  *ind++ = -1;
    *ind++ =  7;  *ind++ =  6;  *ind++ = 22;  *ind++ = 21;  *ind++ = -1;
    *ind++ = 15;  *ind++ = 14;  *ind++ = 20;  *ind++ = 23;  *ind++ = -1;
    *ind++ = 19;  *ind++ = 18;  *ind++ =  1;  *ind++ =  0;  *ind++ = -1;
    *ind++ =  4;  *ind++ =  7;  *ind++ =  2;  *ind++ =  1;  *ind++ = -1;
    *ind++ =  3;  *ind++ =  2;  *ind++ = 21;  *ind++ = 20;  *ind++ = -1;
    *ind++ =  0;  *ind++ =  3;  *ind++ = 14;  *ind++ = 13;  *ind++ = -1;
    *ind++ = 17;  *ind++ = 16;  *ind++ =  9;  *ind++ =  8;  *ind++ = -1;
    *ind++ =  6;  *ind++ =  5;  *ind++ =  8;  *ind++ = 11;  *ind++ = -1;
    *ind++ = 11;  *ind++ = 10;  *ind++ = 23;  *ind++ = 22;  *ind++ = -1;
    *ind++ = 10;  *ind++ =  9;  *ind++ = 12;  *ind++ = 15;  *ind++ = -1;

    *ind++ =  0;  *ind++ = 13;  *ind++ = 19;  *ind++ = -1;
    *ind++ =  1;  *ind++ = 18;  *ind++ =  4;  *ind++ = -1;
    *ind++ =  2;  *ind++ =  7;  *ind++ = 21;  *ind++ = -1;
    *ind++ =  3;  *ind++ = 20;  *ind++ = 14;  *ind++ = -1;
    *ind++ =  9;  *ind++ = 16;  *ind++ = 12;  *ind++ = -1;
    *ind++ =  8;  *ind++ =  5;  *ind++ = 17;  *ind++ = -1;
    *ind++ = 11;  *ind++ = 22;  *ind++ =  6;  *ind++ = -1;
    *ind++ = 10;  *ind++ = 15;  *ind++ = 23;  *ind++ = -1;

    faces = new SoIndexedFaceSet();
    faces->coordIndex.setValues (0, 30+60+32, indices);
    delete indices;

    block->addChild (coords);
    block->addChild (normals);
    block->addChild (faces);
}




