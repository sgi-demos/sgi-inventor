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
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.10 $
 |
 |   Description:
 |      Definition of message classes defined in msg.h
 | 
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "msg.h"

//////////////////////////////////////////////////////////////////////////////
//
// Constructor for the Shot class.
//
Shot::Shot(int id, float x, float y, float dx, float dy, const SbTime &t)
  : Command(MSG_SHOTFIRED)
//
//////////////////////////////////////////////////////////////////////////////
{
    from.setValue(x, y);
    dir.setValue(dx, dy);
    playerId = id;
    timestamp = t;
}


//////////////////////////////////////////////////////////////////////////////
//
// Constructor for the StartGame class.
//
StartGame::StartGame(int i, char **n) : Command(MSG_STARTGAME)
//
//////////////////////////////////////////////////////////////////////////////
{
    timestamp.setToTimeOfDay();
    numPlayers = i;
    strcpy(names[0], n[0]);
    strcpy(names[1], n[1]);
}
