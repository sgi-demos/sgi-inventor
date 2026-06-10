/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */


/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |      This file defines the SoColorIndex node class.
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_COLOR_INDEX_
#define  _SO_COLOR_INDEX_

#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/nodes/SoSubNode.h>


////////////////////////////////////////////////////////////////////////////////
//! Surface color index node.
/*!
\class SoColorIndex
\ingroup Nodes
This node specifies the color to use for subsequent shapes as an index
into the current color table. This is used only for <tt>BASE_COLOR</tt>
lighting (see SoLightModel) in color index mode. Color index mode
may be enabled by the window in which rendering occurs.


Since color indices make sense only in the context of OpenGL rendering,
this node implements only a method for the SoGLRenderAction.

\par Action Behavior
\par
SoGLRenderAction
<BR> Sets the color index for subsequent shapes rendered with <tt>BASE_COLOR</tt> lighting. 

\par See Also
\par
SoBaseColor, SoLightModel
*/
////////////////////////////////////////////////////////////////////////////////

class INVENTOR_API SoColorIndex : public SoNode {

    SO_NODE_HEADER(SoColorIndex);

  public:
    //! \name Fields
    //@{

    //! Color index.
    SoMFInt32           index;          

    //@}

    //! Creates a color index node with default settings.
    SoColorIndex();

  SoEXTENDER public:
    virtual void        GLRender(SoGLRenderAction *action);

  SoINTERNAL public:
    static void         initClass();

  protected:
    virtual ~SoColorIndex();
};

#endif /* _SO_COLOR_INDEX_ */
