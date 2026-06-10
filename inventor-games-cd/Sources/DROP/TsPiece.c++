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
 |   Classes	: TsPiece
 |
 |   Author	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

//--------------------------- Include ----------------------------------
#include <stdio.h>
#include <Inventor/nodes/SoTransform.h>
#include "TsPiece.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Constructor for the TsPiece.
//
// Use: public

TsPiece::TsPiece()

//
////////////////////////////////////////////////////////////////////////
{
    orientation        = 0;
    numBlocks          = 0;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	Destructor for TsPiece.
//
// Use: public

TsPiece::~TsPiece()

//
////////////////////////////////////////////////////////////////////////
{
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine returns the list of positions of the blocks for the
//      piece in its current orientation.  The positions are specified as
//      indices into the standard 27-cube.  This method assumes that space
//      to store the returned indices is passed in.
//
// Use: public

int
TsPiece::getPosition(
        int pos[] )               // (out) the list of positions

// Values Returned:
//      number of blocks in the piece
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    for (i=0; i<numBlocks; i++)
        pos[i] = piecePositionTable[orientation][i];

    return numBlocks;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine returns the list of positions of the blocks for the
//      piece in the given orientation.  The positions are specified as
//      indices into the standard 27-cube.  This method assumes that space
//      to store the returned indices is passed in.
//
// Use: public

int
TsPiece::getPosition(
        int newOrientation,
        int pos[])               // (out) the list of positions

// Values Returned:
//      number of blocks in the piece
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    for (i=0; i<numBlocks; i++)
        pos[i] = piecePositionTable[newOrientation][i];

    return numBlocks;
}




