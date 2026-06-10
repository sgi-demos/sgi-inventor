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
 |   Methods	: TsField::handleKeyboard
 |
 |   Author	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

//--------------------------- Include ----------------------------------
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoSpaceballButtonEvent.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoSeparator.h>
#include "TsField.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine handles all keyboard events that move the pieces as
//      they fall.
//
// Use: private, static

void
TsField::handleKeyboard( void *data, SoEventCallback *node )

//
////////////////////////////////////////////////////////////////////////
{
    TsField       *field = (TsField *)data;

    if (SO_KEY_PRESS_EVENT(node->getEvent(), RETURN)) {
        field->handleReturnKey();
        node->setHandled();
        return;
    }

    // If the game has been paused, don't accept any input.  Just return.
    if (field->gamePaused) {
        node->setHandled();
        return;
    }
    
    if (SO_KEY_PRESS_EVENT(node->getEvent(), LEFT_ARROW))
        field->translation (TS_X_AXIS, -1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), RIGHT_ARROW))
        field->translation (TS_X_AXIS,  1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), UP_ARROW))
        field->translation (TS_Z_AXIS,  1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), DOWN_ARROW))
        field->translation (TS_Z_AXIS, -1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), F))
        field->rotation (TS_Z_AXIS,   1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), S))
        field->rotation (TS_Z_AXIS,  -1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), D))
        field->rotation (TS_Y_AXIS,  1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), C))
        field->rotation (TS_X_AXIS,  -1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), E))
        field->rotation (TS_X_AXIS,   1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), SPACE))
        field->dropPiece();
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), H)) {
        field->handleHelpKey();
    }

    node->setHandled();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine handles the return key press.
//
// Use: public

void
TsField::handleReturnKey()

//
////////////////////////////////////////////////////////////////////////
{
    // If help had been displayed, either start the first game
    // or resume a game in progress.
    if (helpDisplayed) {
        helpDisplayed = FALSE;
        helpRoot->removeChild(helpGroup); //old:overRoot->removeChild(helpRoot)
        if (isGameOver) 
            startNewGame();
        else
            resumeGame();
    }
    else {
        // Start a new game
        startNewGame();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine handles the H key press.
//
// Use: public

void
TsField::handleHelpKey()

//
////////////////////////////////////////////////////////////////////////
{
    // If the help message is not displayed, add the help message geometry
    // to the scene graph and pause the game.
    if (!helpDisplayed) {
        helpRoot->addChild(helpGroup); //old: overRoot->addChild(helpRoot);
        helpDisplayed = TRUE;
        pauseGame();
    }
}
