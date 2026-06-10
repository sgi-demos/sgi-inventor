//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Description:
 |	This file defines the ButtonStrip class. 
 |      It creates an SoXtRenderArea that has buttons arranged in 
 |      a row. 
 |
 |      It uses an ortho camera and BASE_COLOR rendering.
 |
 |	Buttons are added with two overloaded methods of addButton.
 |	One takes a single arg of type Button.
 |	The second takes four arguments:
 |	icon, iconActive, callback, and userData.
 |
 |	Only the second version uses the resources:
 |      "buttonStripBacking"
 |      "buttonStripBackingActive"
 |      place a background behind each button. 
 |	The scene graph for these should fit within a standard 
 |      (2x2x2) cube or it may intersect the rest of the icon.
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_BUTTON_STRIP_
#define  _SO_BUTTON_STRIP_

#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/draggers/SoDragger.h>

class SoSeparator;
class SoOrthographicCamera;
class SoLightModel;
class SoNodeList;
class Button;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: ButtonStrip
//
//////////////////////////////////////////////////////////////////////////////

class ButtonStrip {
    
  public:

    ButtonStrip( Widget parent = NULL,
		 const char *name = NULL,
		 SbBool buildInsideParent = TRUE);
    ~ButtonStrip();

    SoXtRenderArea *getRenderArea() { return renderArea; }

    // icon and iconActive should be sized in a (2x2x2) cube.
    void addButton( SoNode *icon, SoNode *iconActive,
		    SoDraggerCB *pressedCallback, void *userData );

    // Button should be sized ina (2x2x2) cube.
    void addButton( Button *newButton );

  protected:

    SoSeparator          *allScene;
    SoOrthographicCamera *camera;
    SoLightModel         *lightModel;

    SoNodeList           *buttonList;

    SoGroup *backing;
    SoGroup *backingActive;

    SoXtRenderArea       *renderArea;
};

#endif /* _SO_BUTTON_STRIP_ */
