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
 * Copyright (C) 1990,91,92,93,94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.17 $
 |
 |   Classes:
 |	State
 |
 |   Author(s)	: Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/fields/SoSFLong.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/nodes/SoText2.h>

#include "Car.h"
#include "State.h"

#ifndef DEBUG
#define NDEBUG
#endif
#include <assert.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//

GameState::GameState()
//
////////////////////////////////////////////////////////////////////////
{
    myCarNumber = -1;
    myCarStartedRace = FALSE;
    network = NULL;
    scene = NULL;
    track = NULL;
    camera = NULL;
    numCarsFinished = 0;
    simulationSensor = NULL;
    controlledCar = NULL;
    playQuietly = FALSE;
    showAnnotation = TRUE;
    startDelay = 45.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add a car to the list of cars.
//

void
GameState::addCar(Car *car, SbBool mine)
//
////////////////////////////////////////////////////////////////////////
{
    if (mine) controlledCar = car;
    
    cars.append((void *)car);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove a car from the list of cars.
//

void
GameState::removeCar(Car *car)
//
////////////////////////////////////////////////////////////////////////
{
    int i = cars.find((const void *)car);
    if (i != -1) cars.remove(i);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the car belonging to the given player.
//

Car *
GameState::findCar(const CarPlayer &player) const
//
////////////////////////////////////////////////////////////////////////
{
    for (int i=0; i<getNumCars(); i++)
        if (getCar(i)->getId() == player)
            return getCar(i);

    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figures out positions of cars.  A bubble sort is used, which is
//    simple and quick since the list is usually already sorted.
//

void
GameState::sortCars()
//
////////////////////////////////////////////////////////////////////////
{
    SbBool exchanged = TRUE;
    
    while (exchanged) {
	exchanged = 0;
	for (int i = 0; i < cars.getLength()-1; i++) {

	    if (getCar(i+1)->isAheadOf(getCar(i))) {
		// Swap
		exchanged = 1;
		void *temp = cars[i];
		cars[i] = cars[i+1];
		cars[i+1] = temp;
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets global fields that control the heads-up display of who is
//    in front/in back of whom.
//

void
GameState::displayPlaces()
//
////////////////////////////////////////////////////////////////////////
{
    static int oldPlace = -1;
    sortCars();

    // Set the racing place digit
    SoText2 *placeNode = (SoText2 *)SoNode::getByName("RACE_PLACE");
    int i;	// used after the loop (pre-ANSI scoping)
    for (i = 0; i < getNumCars(); i++)
        if ((Car *)cars[i] == controlledCar)
            if (i != oldPlace) {
                oldPlace = i;
                placeNode->string.setValue(oldPlace+1);
            }

    for (i = 0; i < 8; i++) {
	// Enough room for "Position1" through "Position8":
	char positionGFName[16];
	sprintf(positionGFName, "Position%d", i+1);
	SoSFString *positionGF = (SoSFString *)
	    SoDB::getGlobalField(positionGFName);

	// Enough room for "Pos1Color" through "Pos8Color":
	char posColorGFName[10];
	sprintf(posColorGFName, "Pos%dColor", i+1);
	SoSFLong *posColorGF = (SoSFLong *)
	    SoDB::getGlobalField(posColorGFName);

	// If this assertion fails, it means the heads-up display
	// scene graph doesn't contain the right global fields, and
	// must be fixed.
	assert(positionGF != NULL && posColorGF != NULL);
	
	if (i < getNumCars()) {
	    Car *car = getCar(i);
            char cname[32];
	    if (car->getCarNum() < ROBOT_CAR_NUM) {
		sprintf(cname, "#%d %s", car->getCarNum(),
			car->getCarName().getString());
	    } else {
		sprintf(cname, "   %s", car->getCarName().getString());
	    }

	    if (positionGF->getValue() != SbString(cname))
		positionGF->setValue(cname);

	    int whichColor;
	    if (car->getIsFinished()) whichColor = 0;
	    else whichColor = 1;
	    if (posColorGF->getValue() != whichColor)
		posColorGF->setValue(whichColor);

	} else {
	    if (positionGF->getValue() != "")
		positionGF->setValue("");
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sort players according to their point totals.
//

void
GameState::displayPointTotals()
//
////////////////////////////////////////////////////////////////////////
{
    // Count the number of non-robot cars
    int *carSort = new int[getNumCars()];
    int carCt = 0;
    int i;	// used after the loop (pre-ANSI scoping)
    for (i=0; i<getNumCars(); i++)
        if (getCar(i)->getCarNum() < ROBOT_CAR_NUM)
            carSort[carCt++] = i;
            
    // Perform a bubble sort
    SbBool exchanged = TRUE;
    
    while (exchanged) {
	exchanged = FALSE;
	for (i = 0; i < carCt-1; i++) {

            int pTotal1 = getCar(carSort[i  ])->getPointTotal();
            int pTotal2 = getCar(carSort[i+1])->getPointTotal();
	    if (pTotal2 > pTotal1) {
		// Swap
		exchanged = TRUE;
		int temp = carSort[i];
		carSort[i] = carSort[i+1];
		carSort[i+1] = temp;
	    }
	}
    }

    // Assign the first 6 (if there are that many) to the point total
    // displayed on the screen.
    SoText2 *pointTotalsNode = (SoText2 *)SoNode::getByName(
                    "POINT_TOTAL_POINTS");
    SoText2 *pointNamesNode = (SoText2 *)SoNode::getByName(
                    "POINT_TOTAL_NAMES");
    int numToDisplay = (carCt < 6) ? carCt : 6;
    pointTotalsNode->string.deleteValues(0);
    pointNamesNode->string.deleteValues(0);
    for (i=0; i<numToDisplay; i++) {
        pointTotalsNode->string.set1Value(i,
                getCar(carSort[i])->getPointTotal());
        pointNamesNode->string.set1Value(i,
                getCar(carSort[i])->getCarName());
    }

    delete [] carSort;
}
