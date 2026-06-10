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
#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include "Common.h"
#include "Mine.h"
#include "Motion.h"
#include "Panel.h"
#include "Ring.h"
#include "Scene.h"
#include "Setup.h"
#include "Tunnel.h"

struct TunnelSection {
    float		length;		// Length of section
    int			numSubSections;	// Number of subsections
    SbRotation		startAngle;	// Angle at start of section
    SbRotation		endAngle;	// Angle at end of section
    SbPlane		endPlane;	// Plane dividing section from next
    SbVec3f		startCenter;	// Center of start of section
    SbVec3f		endCenter;	// Center of end of section
    SoSwitch		*dispSwitch;	// Switch node of section display graph
    int			numMines;	// Number of mines in section
    Mine		**mines;	// Pointers to mines in section
    Ring		*ring;		// Ring in section (or NULL)
    SbBool		isVisible;	// Whether section is visible
};

// Tunnel texture coordinates are set up so that s varies from 0 to
// SMAX around the perimeter of each cross-section. t varies from 0 at
// the first cross-section and increases as a function of tunnel
// length, so the texture is scaled evenly. The multiplier is defined
// here.
#define TUNNEL_SMAX	4.0
#define TUNNEL_TMULT	0.4

////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
////////////////////////////////////////////////////////////////////////////

Tunnel::Tunnel(int level)
{
    // Set constant values
    radius = TUNNEL_RADIUS;
    numSides = TUNNEL_NUM_SIDES;
    totalLength = TUNNEL_TOTAL_LENGTH;
    maxSectionLength = MAX_SECTION_LENGTH;
    maxSubSectionLength = MAX_SUB_SECTION_LENGTH;

    // Set parameters based on given level
    minSectionLength = MAX_SECTION_LENGTH / level;
    if (minSectionLength < MIN_SECTION_LENGTH)
	minSectionLength = MIN_SECTION_LENGTH;
    maxJointAngle = RADIANS(level * LEVEL_ANGLE_INCREASE);
    if (maxJointAngle > RADIANS(MAX_JOINT_ANGLE))
	maxJointAngle = RADIANS(MAX_JOINT_ANGLE);

    if (maxSectionLength < minSectionLength)
	maxSectionLength = minSectionLength;

    // Compute section lengths and angles
    computeSections();

    // Compute vertex coordinates and normals
    computeVertices();

    // Set up mines and rings
    addMines(level);
    addRings(level);

    // Build scene graph to represent tunnel
    buildGraph();

    curSection   = 0;
    prevDistance = 0.0;

    setSectionSwitches();
}

////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
////////////////////////////////////////////////////////////////////////////

Tunnel::~Tunnel()
{
    for (int i = 0; i < numSections; i++) {
	for (int j = 0; j < sections[i].numMines; j++)
	    delete sections[i].mines[j];
	if (sections[i].numMines > 0)
	    delete sections[i].mines;
	if (sections[i].ring != NULL)
	    delete sections[i].ring;
    }

    delete [] sections;
    delete [] vertexCoords;
    delete [] vertexNormals;
    delete [] vertexTexCoords;

    root->unref();
}

////////////////////////////////////////////////////////////////////////////
//
// Returns scene graph representing tunnel.
//
////////////////////////////////////////////////////////////////////////////

SoNode *
Tunnel::getGraph()
{
    return root;
}

////////////////////////////////////////////////////////////////////////////
//
// Computes per-section info.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::computeSections()
{
    // Figure out how many sections there can be and allocate room
    int maxNumSections = (int) (totalLength / minSectionLength) + 1;
    sections = new TunnelSection [maxNumSections];

    float		len = 0.0;
    int			secNum = 0;
    SbVec3f		dir(0.0, 0.0, -1.0);
    TunnelSection	*sec;

    sec = &sections[0];
    sec->startCenter.setValue(0.0, 0.0, 0.0);
    sec->startAngle = SbRotation::identity();

    // Keep adding sections until the total length is reached
    while (len < totalLength) {

	// Sanity check
	if (secNum >= maxNumSections) {
	    fprintf(stderr, "Error in Tunnel::computeSections()\n"); // ???
	    return;
	}

	// Copy info from previous section
	if (secNum > 0) {
	    sec->startAngle  = sections[secNum - 1].endAngle;
	    sec->startCenter = sections[secNum - 1].endCenter;
	}

	// Choose random length and make sure it doesn't exceed the total
	sec->length = RANDOM(minSectionLength, maxSectionLength);
	len += sec->length;
	if (len > totalLength)
	    sec->length -= len - totalLength;

	// Set end of section
	sec->endCenter = sec->startCenter + sec->length * dir;

	// Choose random direction and angle
	float	jointAngle    = RANDOM(0.0, maxJointAngle);
	SbVec3f	jointAxis(RANDOM(-1.0, 1.0),
			  RANDOM(-1.0, 1.0),
			  RANDOM(-1.0, 1.0));
	jointAxis.normalize();
	sec->endAngle.setValue(jointAxis, jointAngle);

	// Compute and store number of subsections in this section
	sec->numSubSections = (int) ceilf(sec->length / maxSubSectionLength);

	// Compute plane at end of section
	SbVec3f sectionAxis = sec->endCenter - sec->startCenter;
	sectionAxis.normalize();
	// Rotate plane by half of the joint angle
	SbRotation planeRot = sec->endAngle;
	planeRot.scaleAngle(.5);
	planeRot.multVec(sectionAxis, sectionAxis);
	sec->endPlane = SbPlane(sectionAxis, sec->endCenter);

	// Change direction for next section
	sec->endAngle.multVec(dir, dir);

	// All sections are initially invisible
	sec->isVisible = FALSE;

	secNum++;
	sec++;
    }

    numSections = secNum;
}

////////////////////////////////////////////////////////////////////////////
//
// Computes vertex coordinates, normals, and texture coordinates.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::computeVertices()
{
    // Figure out how many subsections there can be and allocate room
    // for vertex coordinates and normals
    int maxNumSubSections = (int) ((totalLength / minSectionLength) +
				   (totalLength / maxSubSectionLength) + 1);
    int maxNumVertices = maxNumSubSections * (numSides + 1);
    vertexCoords    = new SbVec3f [maxNumVertices];
    vertexNormals   = new SbVec3f [maxNumVertices];
    vertexTexCoords = new SbVec2f [maxNumVertices];

    // Compute coords and normals for initial cross-section (in
    // xy-plane). Store at beginning of vertexCoords, vertexNormals,
    // and vertexTexCoords arrays.
    for (int side = 0; side < numSides; side++) {
	float theta = side * ((2 * M_PI) / numSides);
	float c = cosf(theta), s = sinf(theta);

	vertexCoords[side].setValue(c, s, 0.0);

	// Make normals point inward
	vertexNormals[side].setValue(-c, -s, 0.0);

	// Texture coordinates are defined so that s changes (from 0
	// to 4) around the perimeter of the cross-section, and t
	// changes from one cross-section to the next.
	vertexTexCoords[side].setValue(TUNNEL_SMAX * side * (1.0 / numSides),
				       0.0);
    }
    vertexCoords[numSides]  = vertexCoords[0];
    vertexNormals[numSides] = vertexNormals[0];
    vertexTexCoords[numSides].setValue(TUNNEL_SMAX, 0.0);

    //
    // We need to compute the vertices at each cross-section
    // separating sections and subsections. Here's the plan: We
    // already have the vertices for the initial cross section. Each
    // subsequent cross-section is the same as the first, but rotated
    // (at the joints) and translated (along the lengths). We'll keep
    // track of the cumulative rotation from the initial section to
    // the current section, and use the stored endPoints to determine
    // how to translate, assuming the initial cross-section is at the
    // origin.
    //

    SbRotation	curRotation = SbRotation::identity();
    float	curLength = 0.0;
    int		numCrossSections = 1;

    // For each section (including the first one)
    for (int secNum = 0; secNum < numSections; secNum++) {

	TunnelSection *sec = &sections[secNum];

	// If this is not the first section, complete the rotation
	// from the previous joint
	if (secNum > 0) {
	    SbRotation rot = sec->startAngle;
	    rot.scaleAngle(0.5);
	    curRotation *= rot;
	}

	// For each joint between subsections of the current section,
	// store a cross-section
	if (sec->numSubSections > 1) {
	    SbVec3f trans =
		(sec->endCenter - sec->startCenter) / sec->numSubSections;
	    float subLen = sec->length / sec->numSubSections;
	    for (int sub = 1; sub < sec->numSubSections; sub++)
		storeCrossSection(numCrossSections++, curRotation,
				  sec->startCenter + sub * trans,
				  curLength + sub * subLen);
	}

	// Rotate to joint before next section, if any
	if (secNum < numSections - 1) {
	    SbRotation rot = sec->endAngle;
	    rot.scaleAngle(0.5);
	    curRotation *= rot;
	}

	// Add section length to current length
	curLength += sec->length;

	// Store next section
	storeCrossSection(numCrossSections++,
			  curRotation, sec->endCenter, curLength);
    }

    // Save total number of vertices
    numVertices = numCrossSections * (numSides + 1);
}

////////////////////////////////////////////////////////////////////////////
//
// Stores vertices of nth cross-section in vertexCoords,
// vertexNormals, and vertexTexCoords arrays by rotating and
// translating 1st cross-section.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::storeCrossSection(int sectionNum, const SbRotation &rotation,
			  const SbVec3f &translation, float distFromStart)
{
    int firstSectionIndex = 0;
    int curSectionStartIndex = sectionNum * (numSides + 1);
    int curSectionIndex      = curSectionStartIndex;

    float t = TUNNEL_TMULT * distFromStart;

    for (int side = 0; side < numSides; side++) {

	// Rotate coords and normals
	rotation.multVec(vertexCoords[firstSectionIndex],
			 vertexCoords[curSectionIndex]);
	rotation.multVec(vertexNormals[firstSectionIndex],
			 vertexNormals[curSectionIndex]);

	// Translate coords
	vertexCoords[curSectionIndex] += translation;

	// Set up texture coordinates
	float s = vertexTexCoords[firstSectionIndex][0];
	vertexTexCoords[curSectionIndex].setValue(s, t);

	firstSectionIndex++;
	curSectionIndex++;
    }

    // Make sure first and last coords match up
    vertexCoords[curSectionIndex]  = vertexCoords[curSectionStartIndex];
    vertexNormals[curSectionIndex] = vertexNormals[curSectionStartIndex];
    vertexTexCoords[curSectionIndex].setValue(TUNNEL_SMAX, t);
}

////////////////////////////////////////////////////////////////////////////
//
// Adds mines to tunnel, based on current level.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::addMines(int level)
{
    // The maximum number of mines per section depends on the current
    // level. So does the minimum mine size and delay time. 
    float maxMinesPerSection	= MAX_MINES_PER_SECTION(level);
    float minMineSize		= MIN_MINE_SIZE(level);
    float maxMineSize		= MAX_MINE_SIZE;
    float minDelayTime		= MIN_DELAY_TIME(level);
    float maxDelayTime		= MAX_DELAY_TIME;

    // First section is always free of mines
    sections[0].numMines = 0;

    // Add mines to each section after the first one
    for (int secNum = 1; secNum < numSections; secNum++) {
	TunnelSection *sec = &sections[secNum];

	sec->numMines = (int) (RANDOM(0.0, maxMinesPerSection) + 0.5);
	if (sec->numMines > 0) {
	    sec->mines = new Mine * [sec->numMines];

	    // Mines can't be placed too close to a joint
	    float minDist = TUNNEL_SAFE_LENGTH;
	    float maxDist = sec->length - TUNNEL_SAFE_LENGTH;

	    // Create and place the mines
	    for (int i = 0; i < sec->numMines; i++) {
		SbVec3f pos = computePosition(sec,
					      RANDOM(minDist, maxDist),
					      RANDOM(0.0, 0.9));
		sec->mines[i] = new Mine(pos,
					 RANDOM(minMineSize, maxMineSize),
					 RANDOM(minDelayTime, maxDelayTime));
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Adds rings to tunnel, based on current level.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::addRings(int level)
{
    // The odds of having an ring in a section depends on the
    // current level. So does the minimum ring size.
    float ringOdds    = RING_ODDS(level);
    float minRingSize = MIN_RING_SIZE(level);
    float maxRingSize = MAX_RING_SIZE(level);

    // First and last sections always have no ring
    sections[0].ring = sections[numSections - 1].ring = NULL;

    // Add rings to sections after the first one and before the
    // last one
    for (int secNum = 1; secNum < numSections - 1; secNum++) {
	TunnelSection *sec = &sections[secNum];

	// Section has to be long enough for a ring
	if (sec->length > 2 * TUNNEL_SAFE_LENGTH &&
	    RANDOM(0.0, 1.0) <= ringOdds) {

	    // Ring can't be placed too close to a joint
	    float minDist = TUNNEL_SAFE_LENGTH;
	    float maxDist = sec->length - TUNNEL_SAFE_LENGTH;

	    // Orientation of ring is along section axis
	    SbVec3f sectionAxis = sec->endCenter - sec->startCenter;
	    sectionAxis.normalize();
	    SbRotation rot(SbVec3f(0.0, 0.0, -1.0), sectionAxis);

	    // Pick a random distance and radius
	    float distance = RANDOM(minDist, maxDist);
	    float radius   = RANDOM(minRingSize, maxRingSize);

	    // Compute a position for the ring
	    SbVec3f pos    = computePosition(sec, distance, 1.0 - radius);

	    // Determine the type of the ring, randomly. The ratio of
	    // energizer to stealth to warp rings is 4/3/1.
	    float rando = RANDOM(0.0, 8.0);

	    if (rando <= 4.0)		// Energizer
		sec->ring = new EnergizerRing(pos, rot, radius,
					      RANDOM(MIN_RING_ENERGY,
						     MAX_RING_ENERGY));
	    else if (rando <= 7.0)	// Stealth
		sec->ring = new StealthRing(pos, rot, radius,
					    RANDOM(MIN_RING_STEALTH_TIME,
						   MAX_RING_STEALTH_TIME));

	    else			// Warp
		sec->ring = new WarpRing(pos, rot, radius,
					 RANDOM(MIN_RING_WARP_DISTANCE,
						MAX_RING_WARP_DISTANCE));
	}
	else
	    sec->ring = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Computes a position that is the given distance from the start of
// the section and the given radius from the center axis.
//
////////////////////////////////////////////////////////////////////////////

SbVec3f
Tunnel::computePosition(TunnelSection *sec, float distance, float radius)
{
    // This is the axis through the middle of the section
    SbVec3f axis = sec->endCenter - sec->startCenter;
    axis.normalize();

    // We need any vector perpendicular to the axis. We'll take
    // the cross-product of the axis and the y-axis; if that
    // doesn't work, we'll do the same with the x-axis. If that
    // doesn't work, the world is broken and we can all quit.
    SbVec3f perp = axis.cross(SbVec3f(0.0, 1.0, 0.0));
    if (perp.normalize() == 0.0) {
	perp = axis.cross(SbVec3f(1.0, 0.0, 0.0));
	perp.normalize();
    }

    // Choose a random rotation around the axis
    SbRotation rot(axis, RANDOM(0.0, 2 * M_PI));
    SbVec3f spoke;
    rot.multVec(perp, spoke);

    // Return spot between center and wall
    return sec->startCenter + (distance * axis) + (radius * spoke);
}

////////////////////////////////////////////////////////////////////////////
//
// Builds scene graph representing tunnel.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::buildGraph()
{
    root = Scene::readFile("Tunnel.iv");
    root->ref();

    SoCoordinate3 *coords  = (SoCoordinate3 *) Scene::find("TunnelCoords");
    SoNormal      *normal  = (SoNormal      *) Scene::find("TunnelNormals");
    SoSeparator   *tunSep  = (SoSeparator   *) Scene::find("TunnelSep");
    SoSeparator   *tunAxis = (SoSeparator   *) Scene::find("TunnelAxis");
    SoGroup	  *tunPts  = (SoGroup       *)Scene::find("TunnelPointsGroup");
    SoTextureCoordinate2 *tc =
	(SoTextureCoordinate2 *) Scene::find("TunnelTexCoords");

    coords->point.setValues(0,  numVertices, vertexCoords);
    normal->vector.setValues(0, numVertices, vertexNormals);
    tc->point.setValues(0, numVertices, vertexTexCoords);

    // This texture node is created once and instanced for each
    // section, if the machine we are running on is fast enough to
    // support real-time textures
    SoTexture2 *tex;
    if (Setup::isTextureEnabled()) {
	tex = new SoTexture2;
	tex->filename = "textures/tunnel.rgb";
    }
    else
	tex = NULL;

    // Build a subgraph for each section, rooted by a switch. Add
    // subgraphs in reverse order so that they are drawn back to
    // front.
    int startIndex = 0;
    int	childIndex = tunSep->getNumChildren();
    for (int secNum = 0; secNum < numSections; secNum++) {
	TunnelSection *sec = &sections[secNum];

	SoSwitch    *sw    = new SoSwitch;
	SoSeparator *sep   = new SoSeparator;
	SoSeparator *qsep  = new SoSeparator;
	SoQuadMesh  *qmesh = new SoQuadMesh;

	// Can't cache the callbacks
	sep->renderCaching = SoSeparator::OFF;
	// Leave auto-caching on the separator over the quad mesh. If
	// texturing is on, we have to give the texture a chance to
	// build its own display list. Turning caching ON here
	// wouldn't allow that.
	qsep->renderCaching = SoSeparator::AUTO;

	tunSep->insertChild(sw, childIndex);
	sw->addChild(sep);

	// Add axis line down center of tunnel
	SoSeparator	*axisSep   = new SoSeparator;
	SoCoordinate3	*axisCoord = new SoCoordinate3;
	axisSep->addChild(axisCoord);
	axisCoord->point.set1Value(0, sec->startCenter);
	axisCoord->point.set1Value(1, sec->endCenter);
	axisSep->addChild(tunAxis);
	axisSep->renderCaching = SoSeparator::ON;
	sw->addChild(axisSep);

	// Add mines and ring
	for (int i = 0; i < sec->numMines; i++)
	    sw->addChild(sec->mines[i]->getGraph());
	if (sec->ring != NULL)
	    sw->addChild(sec->ring->getGraph());

	// Turn off depth-buffering when drawing the tunnel walls
	SoCallback *cb1 = new SoCallback;
	cb1->setCallback(&Tunnel::depthCB, (void *) FALSE);
	sep->addChild(cb1);

	// Add texture to separator with the quad mesh
	if (tex != NULL)
	    qsep->addChild(tex);
	qmesh->startIndex        = startIndex;
	qmesh->verticesPerColumn = sec->numSubSections + 1;
	qmesh->verticesPerRow    = numSides + 1;
	qsep->addChild(qmesh);
	sep->addChild(qsep);

	// Add bright points on walls
	SoSeparator *ptsSep = new SoSeparator;
	SoPointSet  *pts    = new SoPointSet;
	pts->startIndex = startIndex;
	pts->numPoints  = (sec->numSubSections + 1) * (numSides + 1);
	ptsSep->addChild(tunPts);
	ptsSep->addChild(pts);
	ptsSep->renderCaching = SoSeparator::ON;
	sep->addChild(ptsSep);

	// Turn on depth-buffering to draw the rest of the stuff
	SoCallback *cb2 = new SoCallback;
	cb2->setCallback(&Tunnel::depthCB, (void *) TRUE);
	sep->addChild(cb2);

	// Save pointer to switch node so we can turn it on or off later
	sec->dispSwitch = sw;

	// Increment to next section
	startIndex += sec->numSubSections * (numSides + 1);
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Updates location and status of ship within tunnel.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::updateShip(Motion *shipMotion)
{
    TunnelSection *sec = &sections[curSection];

    // See if ship's new position is on other side of plane
    while (sec->endPlane.isInHalfSpace(shipMotion->position1)) {
	prevDistance += sec->length;
	curSection++;
	sec++;
	setSectionSwitches();
	if (curSection == numSections) {
	    shipMotion->exited = TRUE;
	    return;
	}
    }

    // See if ship's position is outside tunnel
    SbLine	axisLine(sec->startCenter, sec->endCenter);
    SbVec3f	closestPoint1 =axisLine.getClosestPoint(shipMotion->position1);
    SbVec3f	diff1 = shipMotion->position1 - closestPoint1;
    float distFromCenter1 = diff1.normalize();

    // Leave a little leeway inside tunnel
    if (distFromCenter1 >= 0.95) {
	shipMotion->hit = TRUE;

	// Find parametric value at intersection of ship's motion with
	// cylindrical tunnel wall. This is the point at which the
	// distance is 0.95 (which should be between the distances of
	// the endpoints of the ship's motion from the center axis).
	SbVec3f	closestPoint0 =axisLine.getClosestPoint(shipMotion->position0);
	SbVec3f	diff0 = shipMotion->position0 - closestPoint0;
	float distFromCenter0 = diff1.length();
	shipMotion->t = ((0.95            - distFromCenter0) /
			 (distFromCenter1 - distFromCenter0));
	
	// Move inside a little bit and turn to move forward
	distFromCenter1 = .9;
	shipMotion->positionAfterHit= closestPoint1 + distFromCenter1 * diff1;
	shipMotion->forwardAfterHit = sec->endCenter - sec->startCenter;
	shipMotion->forwardAfterHit.normalize();
    }

    // Compute distance along tunnel
    float d = (closestPoint1 - sec->startCenter).length();
    float curDistance = (prevDistance + d) / totalLength;

    updateMines(shipMotion);
    updateRings(shipMotion);

    // Update status panel
    Panel::setCentering(distFromCenter1);
    Panel::setDistance(curDistance);
}

////////////////////////////////////////////////////////////////////////////
//
// Updates mine status.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::updateMines(Motion *shipMotion)
{
    // Look at all visible sections
    for (int secNum = 0; secNum < numSections; secNum++) {
	TunnelSection *sec = &sections[secNum];
	if (sec->isVisible)
	    for (int i = 0; i < sec->numMines; i++)
		sec->mines[i]->updateShip(shipMotion);
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Updates ring status.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::updateRings(Motion *shipMotion)
{
    // Look at all visible sections after and including the current one
    for (int secNum = curSection; secNum < numSections; secNum++) {
	TunnelSection *sec = &sections[secNum];
	if (sec->isVisible && sec->ring != NULL)
	    sec->ring->updateShip(shipMotion);
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Sets switch for each section on or off, based on current position.
// This is called when the current position changes sections.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::setSectionSwitches()
{
    // Turn off all switches before the section before the current one
    for (int secNum = 0; secNum < curSection - 1; secNum++) {
	TunnelSection *sec = &sections[secNum];
	if (sec->isVisible) {
	    sec->isVisible = FALSE;
	    sec->dispSwitch->whichChild = SO_SWITCH_NONE;
	}
    }

    // Turn on sections after and including the current one that are
    // within 200 units of the start of the current section
    float totalDist = 0.0;
    secNum = curSection;
    while (secNum < numSections && totalDist < 200.0) {
	TunnelSection *sec = &sections[secNum];
	if (! sec->isVisible) {
	    sec->isVisible = TRUE;
	    sec->dispSwitch->whichChild = SO_SWITCH_ALL;
	}
	totalDist += sec->length;
	secNum++;
    }
}

////////////////////////////////////////////////////////////////////////////
//
// Moves ship safely through tunnel for given distance.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::moveSafely(Motion *shipMotion, float distance)
{
    TunnelSection *sec = &sections[curSection];

    // Repeat this until we have exited tunnel or used up the given distance
    SbVec3f	startPos = shipMotion->position0;
    SbVec3f	endPos;
    float	distanceLeft = distance;
    do {
	// Move ship given distance along center of current section
	SbVec3f axisDir = sec->endCenter - sec->startCenter;
	axisDir.normalize();
	endPos = startPos + distanceLeft * axisDir;

	// If ship crossed into new section, see how much distance is
	// left and start in next section
	if (sec->endPlane.isInHalfSpace(endPos)) {

	    // Exited last section?
	    if (++curSection == numSections) {
		shipMotion->exited = TRUE;
		return;
	    }

	    // Entering new section - see how much distance we used up
	    float distanceUsed = (sec->endCenter - startPos).dot(axisDir);
	    distanceLeft -= distanceUsed;

	    // Move ship to start of next section
	    prevDistance += sec->length;
	    sec++;
	    startPos = sec->startCenter;

	    setSectionSwitches();
	}
	else
	    distanceLeft -= distance;
    } while (distanceLeft > 0.0);

    // Set new ship position and direction
    shipMotion->positionAfterHit = endPos;
    shipMotion->forwardAfterHit = sec->endCenter - sec->startCenter;
    shipMotion->forwardAfterHit.normalize();
}

////////////////////////////////////////////////////////////////////////////
//
// Updates location and status of blast within tunnel.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::updateBlast(Motion *blastMotion)
{
    // Check only sections that are visible. This might miss a few
    // hits on sections that have already passed, but big deal.

    // Check backwards from section before current section
    for (int secNum = curSection - 1; secNum >= 0; secNum--) {
	if (sections[secNum].isVisible)
	    updateBlastSection(secNum, blastMotion);
	else
	    break;
    }

    // Check forward from this section
    for (secNum = curSection; secNum < numSections; secNum++)
	if (sections[secNum].isVisible)
	    updateBlastSection(secNum, blastMotion);
}

////////////////////////////////////////////////////////////////////////////
//
// Implements updateBlast() for a section.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::updateBlastSection(int secNum, Motion *blastMotion)
{
    TunnelSection *sec = &sections[secNum];

    for (int i = 0; i < sec->numMines; i++)
	sec->mines[i]->updateBlast(blastMotion);

    if (sec->ring != NULL)
	sec->ring->updateBlast(blastMotion);
}

////////////////////////////////////////////////////////////////////////////
//
// Callback to enable or disable depth test. The userData pointer is
// actually the Boolean flag.
//
////////////////////////////////////////////////////////////////////////////

void
Tunnel::depthCB(void *userData, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
	if ((SbBool) userData)
	    glEnable(GL_DEPTH_TEST);
	else
	    glDisable(GL_DEPTH_TEST);
    }
}
