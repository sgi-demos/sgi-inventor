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
 |   main ()
 |
 |   Author	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <math.h>
#include <Inventor/Xt/SoXt.h>
#include "TsField.h"

#define  FIELD_RESOLUTION   4

/*
 _______________________________________________________________________
 |
 |   Synopsis:
 |	main()
 |
 |   Description:
 |	Initialize the game and enter the Xt main loop.
 |
 _______________________________________________________________________
 */
void
main( int, char **argv )
{
    // Initialize Inventor and Xt
    Widget appWindow = SoXt::init(argv[0]);
    if ( appWindow == NULL ) exit( 1 );

    //
    // Initialize a random number generator to be used in selecting the
    // next piece and orientation.
    //
    long tvec;
    time (&tvec);
    srand48 (tvec);

    // Build the layout of the game.
    TsField  *field  = new TsField( FIELD_RESOLUTION, appWindow );
    field->setTitle( "Drop" );
    field->initGame();
    field->show();

    // Enter the Xt event loop
    SoXt::show( appWindow );
    SoXt::mainLoop();
}


