//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Source: /d/geek/bell/toolkits/Inventor/apps/games.skip/drop/RCS/TsPiece.h,v $
 |   $Revision: 1.1 $
 |   $Date: 94/03/08 10:10:53 $
 |
 |   Description:
 |	This class describes a base class for constructing pieces used in
 |      the drop demo program.  Pieces are composed of blocks arranged
 |      in some order which defines the particular piece.  The pieces are
 |      stored as indices into a three dimensional array (the playing field)
 |      for each block that is part of the piece.  Operations on the piece
 |      move it to different locations and orientations on the playing field
 |      by modifying the indices into the playing field.
 |
 |   Classes:
 |      TsPiece
 |
 |   Author: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _TS_PIECE_
#define _TS_PIECE_

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsPiece
//
//  The base class for the drop pieces.  Methods transform the piece and
//  store its position and orientation.
//
//////////////////////////////////////////////////////////////////////////////

class TsPiece {
public:
    TsPiece();
   ~TsPiece();

    // Sets the orientation of the piece
    void  	        setOrientation( int orient ) {orientation = orient;};

    // Get the positions of the blocks of the piece in its current
    // orientation.  Return the number of blocks in the piece.
    int 	        getPosition( int pos[] );

    // Get the positions of the blocks of the piece given an orientation.
    // Return the number of blocks in the piece.
    int                 getPosition( int orientation, int pos[] );

protected:
    int                 orientation;
    int                 numBlocks;
    int                 piecePositionTable[24][4];

};

#endif /* _TS_PIECE_ */




