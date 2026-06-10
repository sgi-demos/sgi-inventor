//  -*- C++ -*-

/*
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file describes the pieces used in the drop game.
 |      All pieces are derived from the base class TsPiece.
 |
 |   Classes:
 |      TsEll, TsTee, TsZee, TsLTri, TsMTri, TsRtri
 |
 |   Author: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _TS_PIECES_
#define _TS_PIECES_

#include "TsPiece.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsEll
//
//  This piece is an 'L' shape composed of four blocks, three in a row, and
//  the fourth to one side at the end.
//
//                                     ***
//                                       *
//
//////////////////////////////////////////////////////////////////////////////

class TsEll : public TsPiece {
public:
    TsEll();
   ~TsEll();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsTee
//
//  This piece is an 'T' shape composed of four blocks, three in a row, and
//  the fourth to one side in the middle of the other three.
//
//                                     ***
//                                      *
//
//////////////////////////////////////////////////////////////////////////////

class TsTee : public TsPiece {
public:
    TsTee();
   ~TsTee();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsZee
//
//  This piece is an 'Z' shape composed of four blocks, two in a row, and
//  two more to one side and shifted over one space.
//
//                                     **
//                                      **
//
//////////////////////////////////////////////////////////////////////////////

class TsZee : public TsPiece {
public:
    TsZee();
   ~TsZee();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsLTri
//
//  This piece is a triangle shape with an extra block over the left
//  block of the triangle.
//
//////////////////////////////////////////////////////////////////////////////

class TsLTri : public TsPiece {
public:
    TsLTri();
   ~TsLTri();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsMTri
//
//  This piece is a triangle shape with an extra block over the middle
//  block of the triangle.
//
//////////////////////////////////////////////////////////////////////////////

class TsMTri : public TsPiece {
public:
    TsMTri();
   ~TsMTri();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsRTri
//
//  This piece is a triangle shape with an extra block over the right
//  block of the triangle.
//
//////////////////////////////////////////////////////////////////////////////

class TsRTri : public TsPiece {
public:
    TsRTri();
   ~TsRTri();
};

#endif /* _TS_PIECES_ */


