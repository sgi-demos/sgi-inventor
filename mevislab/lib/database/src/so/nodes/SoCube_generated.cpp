
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a cube.
//
// Use: protected

void
SoCube::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool              materialPerFace;
    int                 numDivisions, face, vert;
    float               s;
    SbVec3f             pt, norm;
    float               w, h, d;
    SbVec4f             tex;
    SbBool              genTexCoords;
    SoPrimitiveVertex   pv;
    SoCubeDetail        detail;
    const SoTextureCoordinateElement    *tce;

    materialPerFace = isMaterialPerFace(action);
    numDivisions    = computeNumDivisions(action);

    pv.setDetail(&detail);

    // Determine whether we should generate our own texture coordinates
    switch (SoTextureCoordinateElement::getType(action->getState())) {
      case SoTextureCoordinateElement::EXPLICIT:
	genTexCoords = TRUE;
	break;
      case SoTextureCoordinateElement::FUNCTION:
	genTexCoords = FALSE;
	break;
    }

    // If we're not generating our own coordinates, we'll need the
    // texture coordinate element to get coords based on points/normals.
    if (! genTexCoords)
	tce = SoTextureCoordinateElement::getInstance(action->getState());
    else {
	tex[2] = 0.0;
	tex[3] = 1.0;
    }

    getSize(w, h, d);


    for (face = 0; face < 6; face++) {

	if (face == 0 || materialPerFace)
	    pv.setMaterialIndex(face);
	pv.setNormal(normals[face]);
	detail.setPart(face);

	// Simple case of one polygon per face 
	if (numDivisions == 1) {
	    beginShape(action, TRIANGLE_STRIP);
	    vert = 3;
	    pt.setValue((*verts[face][vert])[0] * w,
			(*verts[face][vert])[1] * h,
			(*verts[face][vert])[2] * d);
	    if (genTexCoords) {
		tex[0] = texCoords[vert][0];
		tex[1] = texCoords[vert][1];
	    }
	    else
		tex = tce->get(pt, normals[face]);
	    pv.setPoint(pt);
	    pv.setTextureCoords(tex);
	    shapeVertex(&pv);
	    vert = 0;
	    pt.setValue((*verts[face][vert])[0] * w,
			(*verts[face][vert])[1] * h,
			(*verts[face][vert])[2] * d);
	    if (genTexCoords) {
		tex[0] = texCoords[vert][0];
		tex[1] = texCoords[vert][1];
	    }
	    else
		tex = tce->get(pt, normals[face]);
	    pv.setPoint(pt);
	    pv.setTextureCoords(tex);
	    shapeVertex(&pv);
	    vert = 2;
	    pt.setValue((*verts[face][vert])[0] * w,
			(*verts[face][vert])[1] * h,
			(*verts[face][vert])[2] * d);
	    if (genTexCoords) {
		tex[0] = texCoords[vert][0];
		tex[1] = texCoords[vert][1];
	    }
	    else
		tex = tce->get(pt, normals[face]);
	    pv.setPoint(pt);
	    pv.setTextureCoords(tex);
	    shapeVertex(&pv);
	    vert = 1;
	    pt.setValue((*verts[face][vert])[0] * w,
			(*verts[face][vert])[1] * h,
			(*verts[face][vert])[2] * d);
	    if (genTexCoords) {
		tex[0] = texCoords[vert][0];
		tex[1] = texCoords[vert][1];
	    }
	    else
		tex = tce->get(pt, normals[face]);
	    pv.setPoint(pt);
	    pv.setTextureCoords(tex);
	    shapeVertex(&pv);
	    endShape();
	}

	// More than one polygon per face
	else {
	    float       di = 1.0 / numDivisions;
	    SbVec3f     topPoint,    botPoint,    nextBotPoint;
	    SbVec3f     horizSpace, vertSpace;
	    int         strip, rect;

	    botPoint = *verts[face][0];

	    // Compute spacing between adjacent points in both directions
	    horizSpace = di * (*verts[face][1] - botPoint);
	    vertSpace  = di * (*verts[face][3] - botPoint);

	    // For each horizontal strip
	    for (strip = 0; strip < numDivisions; strip++) {

		// Compute current top point. Save it to use as bottom
		// of next strip
		nextBotPoint = topPoint = botPoint + vertSpace;

		beginShape(action, TRIANGLE_STRIP);

		// Send points at left end of strip
		s = 0.0;
		pt = topPoint;
		pt[0] *= w;
		pt[1] *= h;
		pt[2] *= d;
		if (genTexCoords) {
		    tex[0] = s;
		    tex[1] = (strip + 1) * di;
		}
		else
		    tex = tce->get(pt, normals[face]);
		pv.setPoint(pt);
		pv.setTextureCoords(tex);
		shapeVertex(&pv);
		pt = botPoint;
		pt[0] *= w;
		pt[1] *= h;
		pt[2] *= d;
		if (genTexCoords) {
		    tex[0] = s;
		    tex[1] = strip * di;
		}
		else
		    tex = tce->get(pt, normals[face]);
		pv.setPoint(pt);
		pv.setTextureCoords(tex);
		shapeVertex(&pv);

		// For each rectangular piece of strip
		for (rect = 0; rect < numDivisions; rect++) {

		    // Go to next rect
		    topPoint += horizSpace;
		    botPoint += horizSpace;
		    s += di;

		    // Send points at right side of rect
		    pt = topPoint;
		    pt[0] *= w;
		    pt[1] *= h;
		    pt[2] *= d;
		    if (genTexCoords) {
			tex[0] = s;
			tex[1] = (strip + 1) * di;
		    }
		    else
			tex = tce->get(pt, normals[face]);
		    pv.setPoint(pt);
		    pv.setTextureCoords(tex);
		    shapeVertex(&pv);
		    pt = botPoint;
		    pt[0] *= w;
		    pt[1] *= h;
		    pt[2] *= d;
		    if (genTexCoords) {
			tex[0] = s;
			tex[1] = strip * di;
		    }
		    else
			tex = tce->get(pt, normals[face]);
		    pv.setPoint(pt);
		    pv.setTextureCoords(tex);
		    shapeVertex(&pv);
		}

		endShape();

		// Get ready for next strip
		botPoint = nextBotPoint;
	    }
	}
    }
}

//
// Macro to multiply out coordinates to avoid extra GL calls:
//
#define SCALE(pt) (tmp[0] = (pt)[0]*scale[0], tmp[1] = (pt)[1]*scale[1], \
		   tmp[2] = (pt)[2]*scale[2], tmp)

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generic rendering of cube with or without normals, with or
//    without texture coordinates.
//
// Use: private

void
SoCube::GLRenderGeneric(SoGLRenderAction *action,
			SbBool sendNormals, SbBool doTextures)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f scale, tmp;
    getSize(scale[0], scale[1], scale[2]);

    SbBool              materialPerFace;
    int                 numDivisions, face, vert;
    float               s;
    SbVec3f             pt, norm;
    SoMaterialBundle    mb(action);

    materialPerFace = isMaterialPerFace(action);
    numDivisions    = computeNumDivisions(action);

    // Make sure first material is sent if necessary
    if (materialPerFace)
	mb.setUpMultiple();
    mb.sendFirst();

    if (numDivisions == 1)
	glBegin(GL_QUADS);

    for (face = 0; face < 6; face++) {

	if (materialPerFace && face > 0)
	    mb.send(face, numDivisions == 1);
	if (sendNormals)
	    glNormal3fv(normals[face].getValue());

	// Simple case of one polygon per face 
	if (numDivisions == 1) {
	    for (vert = 0; vert < 4; vert++) {
		if (doTextures)
		    glTexCoord2fv(texCoords[vert].getValue());
		glVertex3fv(SCALE(*verts[face][vert]).getValue());
	    }
	}

	// More than one polygon per face
	else {
	    float       di = 1.0 / numDivisions;
	    SbVec3f     topPoint,    botPoint,    nextBotPoint;
	    SbVec3f     horizSpace, vertSpace;
	    int         strip, rect;

	    botPoint = *verts[face][0];

	    // Compute spacing between adjacent points in both directions
	    horizSpace = di * (*verts[face][1] - botPoint);
	    vertSpace  = di * (*verts[face][3] - botPoint);

	    // For each horizontal strip
	    for (strip = 0; strip < numDivisions; strip++) {

		// Compute current top point. Save it to use as bottom
		// of next strip
		nextBotPoint = topPoint = botPoint + vertSpace;

		glBegin(GL_TRIANGLE_STRIP);

		// Send points at left end of strip
		s = 0.0;
		if (doTextures) {
		    glTexCoord2f(s, (strip + 1) * di);
		    glVertex3fv(SCALE(topPoint).getValue());
		    glTexCoord2f(s, strip * di);
		    glVertex3fv(SCALE(botPoint).getValue());
		}
		else {
		    glVertex3fv(SCALE(topPoint).getValue());
		    glVertex3fv(SCALE(botPoint).getValue());
		}

		// For each rectangular piece of strip
		for (rect = 0; rect < numDivisions; rect++) {

		    // Go to next rect
		    topPoint += horizSpace;
		    botPoint += horizSpace;
		    s += di;

		    // Send points at right side of rect
		    if (doTextures) {
			glTexCoord2f(s, (strip + 1) * di);
			glVertex3fv(SCALE(topPoint).getValue());
			glTexCoord2f(s, strip * di);
			glVertex3fv(SCALE(botPoint).getValue());
		    }
		    else {
			glVertex3fv(SCALE(topPoint).getValue());
			glVertex3fv(SCALE(botPoint).getValue());
		    }
		}

		glEnd();

		// Get ready for next strip
		botPoint = nextBotPoint;
	    }
	}
    }

    if (numDivisions == 1)
	glEnd();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Renders cube with normals and without texture coordinates.
//
// Use: private

void
SoCube::GLRenderNvertTnone(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f scale, tmp;
    getSize(scale[0], scale[1], scale[2]);

    SbBool              materialPerFace;
    int                 numDivisions, face, vert;
    SbVec3f             pt, norm;
    SoMaterialBundle    mb(action);

    materialPerFace = isMaterialPerFace(action);
    numDivisions    = computeNumDivisions(action);

    // Make sure first material is sent if necessary
    if (materialPerFace)
	mb.setUpMultiple();
    mb.sendFirst();

    if (numDivisions == 1)
	glBegin(GL_QUADS);

    for (face = 0; face < 6; face++) {

	if (materialPerFace && face > 0)
	    mb.send(face, numDivisions == 1);
	glNormal3fv(normals[face].getValue());

	// Simple case of one polygon per face 
	if (numDivisions == 1) {
	    for (vert = 0; vert < 4; vert++) {
		glVertex3fv(SCALE(*verts[face][vert]).getValue());
	    }
	}

	// More than one polygon per face
	else {
	    float       di = 1.0 / numDivisions;
	    SbVec3f     topPoint,    botPoint,    nextBotPoint;
	    SbVec3f     horizSpace, vertSpace;
	    int         strip, rect;

	    botPoint = *verts[face][0];

	    // Compute spacing between adjacent points in both directions
	    horizSpace = di * (*verts[face][1] - botPoint);
	    vertSpace  = di * (*verts[face][3] - botPoint);

	    // For each horizontal strip
	    for (strip = 0; strip < numDivisions; strip++) {

		// Compute current top point. Save it to use as bottom
		// of next strip
		nextBotPoint = topPoint = botPoint + vertSpace;

		glBegin(GL_TRIANGLE_STRIP);

		// Send points at left end of strip
		glVertex3fv(SCALE(topPoint).getValue());
		glVertex3fv(SCALE(botPoint).getValue());

		// For each rectangular piece of strip
		for (rect = 0; rect < numDivisions; rect++) {

		    // Go to next rect
		    topPoint += horizSpace;
		    botPoint += horizSpace;

		    // Send points at right side of rect
		    glVertex3fv(SCALE(topPoint).getValue());
		    glVertex3fv(SCALE(botPoint).getValue());
		}

		glEnd();

		// Get ready for next strip
		botPoint = nextBotPoint;
	    }
	}
    }

    if (numDivisions == 1)
	glEnd();
}

