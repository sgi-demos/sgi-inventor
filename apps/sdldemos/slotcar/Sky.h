//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |      This file contains the definition of the Sky class.
 |
 |   Classes:
 |      Sky
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SKY_
#define  _SKY_

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Sky
//
//  A Sky section of track
//
//////////////////////////////////////////////////////////////////////////////

class Sky : public SoNode
{
    SO_NODE_HEADER(Sky);

  public:
    static void		initClass();

    Sky();

    SoSFColor	horizonColor;
    SoSFFloat	horizonHeight;
    SoSFColor	skyBottomColor;
    SoSFColor	skyTopColor;

  protected:
    virtual ~Sky();

    virtual void GLRender(SoGLRenderAction *action);
    virtual SbBool      affectsState() const;
};

#endif /* _SKY_ */
