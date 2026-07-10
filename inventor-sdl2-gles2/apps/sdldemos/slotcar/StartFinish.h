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
 |      This file contains the definition of the StartFinish class.
 |
 |   Classes:
 |      StartFinish
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _StartFinish_
#define  _StartFinish_

#include "Straight.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: StartFinish
//
//  A StartFinish section of track
//
//////////////////////////////////////////////////////////////////////////////

class StartFinish : public Straight
{
    SO_NODE_HEADER(StartFinish);

  public:
    static void		initClass();

    // Must pass length to constructor.
    StartFinish(float length);

  protected:
    StartFinish();
    virtual ~StartFinish();

    // This is called by readInstance or the public constructor to
    // create an appropriate scene graph.
    virtual void	constructSceneGraph();

  private:
    static SoSeparator *sharedStuff;
};

#endif /* _StartFinish_ */
