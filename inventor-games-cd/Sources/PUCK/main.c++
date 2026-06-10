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
 |   $Revision: 1.14 $
 |
 |   Description:
 |      Main program for crossfire game.
 | 
 |   Author(s): David Mott, Helga Thorvaldsdottir
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Inventor/Xt/SoXt.h>
#include "SoSocket.h"
#include "Server.h"
#include "Player.h"
#include "msg.h"
#include "InventorLogo.h"
#include <GL/gl.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCallback.h>


////////////////////////////////////////////////////////////////////////
//
//  Set the Inventor logo on the player's screen.
//
static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
	glViewport(0, 0, 80, 80);
    }
}

static void
setOverlayLogo(SoXtRenderArea *ra)
{
    static SoSeparator *logo = NULL;
    
    if (logo == NULL) {
	SoInput in;
	in.setBuffer((void *)ivLogo, ivLogoSize);
	logo = SoDB::readAll(&in);
	logo->ref();
	in.closeFile();
	
	// Add a callback node which will set the viewport
	SoCallback *cb = new SoCallback;
	cb->setCallback(logoCB);
	logo->insertChild(cb, 0);
    }
    
    SbColor colors[2];
    colors[0].setValue(0, 0, 0);
    colors[1].setValue(0.1, 0.1, 0.1);
    ra->setOverlayColorMap(1, 2, colors);
    ra->setOverlaySceneGraph(logo);	
}
//
////////////////////////////////////////////////////////////////////////


static void
print_usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [ -hq ] [ machine_name ] \n", progname);
    fprintf(stderr,
            "\t-h            Help  (Print this message)\n"
            "\t-q            Quiet mode  (Disable audio effects)\n"
            "\tmachine_name  Connect to game on another machine\n"
	    );
    exit(0);
}

void
main(int argc, char *argv[])
{
    // Command-line options
    SbBool  serverLaunchNeeded = TRUE;
    char    serverName[40];		// Name of server machine
    SbBool  useSound = TRUE;		// TRUE if sound should be on
    gethostname(serverName, 40);

    int c;
    while ((c = getopt(argc, argv, "hq")) != EOF) {
	switch (c) {
	    case 'q':
		useSound = FALSE;
		break;
	    case 'h':
		print_usage(argv[0]);
		break;
	    default :
		print_usage(argv[0]);
		break;
	}
    }
    
    if (optind == (argc - 1)) {
	strcpy(serverName, argv[optind]);
	serverLaunchNeeded = FALSE;
	printf("connecting to game on machine %s...\n", serverName);
    }
    
    // Start a server if needed
    if (serverLaunchNeeded) {
	// Fork off a server process
	pid_t pid = fork();
	if (pid == 0) {
	    // This is the child (server) process.
	    SoXt::init(argv[0]);
	    Server *server = new Server();
	    SoXt::mainLoop();
	    return; // all done when the server exits
	}
	else if (pid < 0) {
	    // Error.
	    fprintf(stderr, "Error - could not start server process\n");
	    return;
	}
    }
        
    // Start the player
    if (serverName != "") {
	Widget mainWindow = SoXt::init(argv[0]);
	Player *player = new Player(serverName, mainWindow, useSound);
	setOverlayLogo(player->getRenderArea());
	SoXt::mainLoop();
    }
    else {
	fprintf(stderr, "must specify server name with -s.\n");
	return;
    }

}

