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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      Defines the Button and ToggleButton classes 
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

#include <Inventor/errors/SoDebugError.h>

#include "Buttons.h"
#include "MechPiece.h"

////////////////////////////////////////////////
// Class: Button
////////////////////////////////////////////////

SO_KIT_SOURCE(Button);

void
Button::initClass()
{
    SO_KIT_INIT_CLASS(Button, SoDragger, "Dragger" );
}

Button::Button()
{
    SO_KIT_CONSTRUCTOR(Button);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(buttonSwitch, SoSwitch, TRUE,
			    geomSeparator, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(buttonSep, SoSeparator, TRUE,
			    buttonSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(buttonGeom, SoSeparator, TRUE,
			    buttonSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(buttonText, SoSeparator, TRUE,
			    buttonSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(buttonActiveSep, SoSeparator, TRUE,
			    buttonSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(buttonActiveGeom, SoSeparator, TRUE,
			    buttonActiveSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(buttonActiveText, SoSeparator, TRUE,
			    buttonActiveSep, ,TRUE);

    SO_KIT_ADD_FIELD(isOn, (0));

    SO_KIT_INIT_INSTANCE();

    // Turn off notification from the SoSFNode fields used by the nodekit.
    // From SoInteractionKit class
    MechPiece::makeNodeFieldStopNotifying( topSeparator);
    MechPiece::makeNodeFieldStopNotifying( geomSeparator);
    // From SoDragger class
    MechPiece::makeNodeFieldStopNotifying( motionMatrix);
    // From this class
    MechPiece::makeNodeFieldStopNotifying( buttonSwitch);
    MechPiece::makeNodeFieldStopNotifying( buttonSep);
    MechPiece::makeNodeFieldStopNotifying( buttonActiveSep);

    setPartAsDefault("buttonGeom",       "buttonButtonGeom");
    setPartAsDefault("buttonText",       "buttonButtonText");
    setPartAsDefault("buttonActiveGeom", "buttonButtonActiveGeom");
    setPartAsDefault("buttonActiveText", "buttonButtonActiveText");

    setSwitchValue(buttonSwitch.getValue(), 0 );

    addStartCallback( &Button::startCB );
    addFinishCallback( &Button::finishCB );
}

Button::~Button()
{
}

void
Button::startCB(void *, SoDragger *inDragger )
{
    Button *b = (Button *) inDragger;
    b->setSwitchValue( b->buttonSwitch.getValue(), 1 );
    b->isOn = TRUE;
}
void
Button::finishCB(void *, SoDragger *inDragger )
{
    Button *b = (Button *) inDragger;
    b->setSwitchValue( b->buttonSwitch.getValue(), 0 );
    b->isOn = FALSE;
}

////////////////////////////////////////////////
// Class: ToggleButton
////////////////////////////////////////////////

SO_KIT_SOURCE(ToggleButton);

void
ToggleButton::initClass()
{
    SO_KIT_INIT_CLASS(ToggleButton, Button, "Button" );
}

ToggleButton::ToggleButton()
{
    SO_KIT_CONSTRUCTOR(ToggleButton);

    isBuiltIn = TRUE;

    SO_KIT_INIT_INSTANCE();

    setPartAsDefault("buttonGeom",       "toggleButtonButtonGeom");
    setPartAsDefault("buttonText",       "toggleButtonButtonText");
    setPartAsDefault("buttonActiveGeom", "toggleButtonButtonActiveGeom");
    setPartAsDefault("buttonActiveText", "toggleButtonButtonActiveText");

    setSwitchValue(buttonSwitch.getValue(), 0 );

    removeStartCallback( &Button::startCB );
    removeFinishCallback( &Button::finishCB );

    addStartCallback( &ToggleButton::startCB );
}

ToggleButton::~ToggleButton()
{
}

void
ToggleButton::startCB(void *, SoDragger *inDragger )
{
    ToggleButton *b = (ToggleButton *) inDragger;
    if ( b->isOn.getValue() == TRUE ) {
	b->setSwitchValue( b->buttonSwitch.getValue(), 0 );
	b->isOn = FALSE;
    }
    else {
	b->setSwitchValue( b->buttonSwitch.getValue(), 1 );
	b->isOn = TRUE;
    }
}
