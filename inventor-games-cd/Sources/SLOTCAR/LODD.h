/*
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the LODD node class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_LEVEL_OF_DETAIL_
#define  _SO_LEVEL_OF_DETAIL_

#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/nodes/SoGroup.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: LODD
//
//  LevelOfDetailDistance.  This is a faster, simpler version of the
//  SoLevelOfDetail node that transforms (0,0,0) in object space into
//  world space and figures out how far away that point is away from
//  the eye.
//
//  If there are N children, this node's distance field should contain
//  N-1 distances, with the closest distance first.
//
//////////////////////////////////////////////////////////////////////////////

class LODD : public SoGroup {

    SO_NODE_HEADER(LODD);

  public:
    // Fields
    SoMFFloat		distance;	// Distances to use for comparison

    // Default constructor
    LODD();

  SoEXTENDER public:
    // Implement actions:
    virtual void	doAction(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	getMatrix(SoGetMatrixAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	rayPick(SoRayPickAction *action);
    virtual void	search(SoSearchAction *action);

  SoINTERNAL public:
    static void		initClass();

  private:
    // Destructor
    virtual ~LODD();
};

#endif /* _SO_LEVEL_OF_DETAIL_ */
