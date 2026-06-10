
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a quad mesh.
//
// Use: protected

void
SoQuadMesh::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // When generating primitives for picking, there is no need to
    // create details now, since they will be created in
    // createTriangleDetail() when an intersection is found (but we
    // need to use the face detail to figure out the rest of it).
    // Otherwise, we create a face detail containing the 3 points of
    // the generated triangle, using the stuff in SoShape.
    // We also delay computing default texture coordinates.
    SbBool forPicking = action->isOfType(SoRayPickAction::getClassTypeId());

    //put the vertexProperty into the state:
    SoState *state = action->getState();
    state->push();
    SoVertexProperty* vp = (SoVertexProperty*)vertexProperty.getValue();
    if(vp){
	vp->doAction(action);
    }


    SoPrimitiveVertex           pv;
    SoFaceDetail                fd;
    SoPointDetail               pd;
    const SoCoordinateElement   *ce;
    int                         topVert, botVert, curVert;
    int                         quadIndex, matlIndex, normIndex, tcIndex;
    int                         row, col, numRows, numCols;
    Binding                     materialBinding, normalBinding;
    SoNormalBundle              nb(action, FALSE);
    SoTextureCoordinateBundle   tcb(action, FALSE, ! forPicking);

    ce = SoCoordinateElement::getInstance(action->getState());

    materialBinding = getMaterialBinding(action);
    normalBinding   = getNormalBinding(action);

    // Get number of rows and columns (of quads, not vertices)
    numRows = (int) verticesPerColumn.getValue() - 1;
    numCols = (int) verticesPerRow.getValue()    - 1;

    topVert = (int) startIndex.getValue();
    botVert = topVert + numCols + 1;

    if (forPicking) {
	pv.setTextureCoords(SbVec4f(0.0, 0.0, 0.0, 0.0));
	pv.setDetail(&fd);
    }
    else
	pv.setDetail(&pd);

    // Generate default normals, if necessary:
    if (SoNormalElement::getInstance(state)->getNum() == 0) {
	figureNormals(action->getState(), &nb);
	normalBinding = PER_VERTEX;
    }

    for (row = 0; row < numRows; row++) {

	fd.setPartIndex(row);

	// If either material or normal binding is per quad, we have
	// to generate each quad separately, to get the materials and
	// normals in the details to be correct
	if (materialBinding == PER_QUAD || normalBinding == PER_QUAD) {

	    for (col = -1; col < numCols; col++) {
		if (col >= 0) {
		    // Generate a triangle strip from the 4 vertices of the
		    // independent quad, in this order:
		    //          topVert-1, botVert-1, botVert, topVert

		    quadIndex = row * numCols + col;

		    fd.setFaceIndex(quadIndex);

		    beginShape(action, TRIANGLE_STRIP,
			       forPicking ? NULL : &fd);

		    curVert = topVert - 1;
		    matlIndex = getBindIndex(materialBinding, row, quadIndex, curVert);
		    normIndex = getBindIndex(normalBinding,   row, quadIndex, curVert);
		    tcIndex   = tcb.isFunction() ? 0 : curVert;

		    pv.setPoint(ce->get3(curVert));
		    pv.setNormal(nb.get(normIndex));
		    pv.setMaterialIndex(matlIndex);

		    if (! tcb.isFunction())
			pv.setTextureCoords(tcb.get(tcIndex));

		    if (! forPicking) {
			if (tcb.isFunction())
			    pv.setTextureCoords(tcb.get(pv.getPoint(),
							pv.getNormal()));
			pd.setCoordinateIndex(curVert);
			pd.setMaterialIndex(matlIndex);
			pd.setNormalIndex(normIndex);
			pd.setTextureCoordIndex(tcIndex);
		    }

		    shapeVertex(&pv);

		    curVert = botVert - 1;
		    matlIndex = getBindIndex(materialBinding, row, quadIndex, curVert);
		    normIndex = getBindIndex(normalBinding,   row, quadIndex, curVert);
		    tcIndex   = tcb.isFunction() ? 0 : curVert;

		    pv.setPoint(ce->get3(curVert));
		    pv.setNormal(nb.get(normIndex));
		    pv.setMaterialIndex(matlIndex);

		    if (! tcb.isFunction())
			pv.setTextureCoords(tcb.get(tcIndex));

		    if (! forPicking) {
			if (tcb.isFunction())
			    pv.setTextureCoords(tcb.get(pv.getPoint(),
							pv.getNormal()));
			pd.setCoordinateIndex(curVert);
			pd.setMaterialIndex(matlIndex);
			pd.setNormalIndex(normIndex);
			pd.setTextureCoordIndex(tcIndex);
		    }

		    shapeVertex(&pv);

		    curVert = topVert;
		    matlIndex = getBindIndex(materialBinding, row, quadIndex, curVert);
		    normIndex = getBindIndex(normalBinding,   row, quadIndex, curVert);
		    tcIndex   = tcb.isFunction() ? 0 : curVert;

		    pv.setPoint(ce->get3(curVert));
		    pv.setNormal(nb.get(normIndex));
		    pv.setMaterialIndex(matlIndex);

		    if (! tcb.isFunction())
			pv.setTextureCoords(tcb.get(tcIndex));

		    if (! forPicking) {
			if (tcb.isFunction())
			    pv.setTextureCoords(tcb.get(pv.getPoint(),
							pv.getNormal()));
			pd.setCoordinateIndex(curVert);
			pd.setMaterialIndex(matlIndex);
			pd.setNormalIndex(normIndex);
			pd.setTextureCoordIndex(tcIndex);
		    }

		    shapeVertex(&pv);

		    curVert = botVert;
		    matlIndex = getBindIndex(materialBinding, row, quadIndex, curVert);
		    normIndex = getBindIndex(normalBinding,   row, quadIndex, curVert);
		    tcIndex   = tcb.isFunction() ? 0 : curVert;

		    pv.setPoint(ce->get3(curVert));
		    pv.setNormal(nb.get(normIndex));
		    pv.setMaterialIndex(matlIndex);

		    if (! tcb.isFunction())
			pv.setTextureCoords(tcb.get(tcIndex));

		    if (! forPicking) {
			if (tcb.isFunction())
			    pv.setTextureCoords(tcb.get(pv.getPoint(),
							pv.getNormal()));
			pd.setCoordinateIndex(curVert);
			pd.setMaterialIndex(matlIndex);
			pd.setNormalIndex(normIndex);
			pd.setTextureCoordIndex(tcIndex);
		    }

		    shapeVertex(&pv);

		    endShape();
		}

		topVert++;
		botVert++;
	    }
	}

	// We can generate triangle strips, which is a little easier
	else {
	    beginShape(action, TRIANGLE_STRIP, forPicking ? NULL : &fd);

	    for (col = -1; col < numCols; col++) {

		quadIndex = row * numCols + col;

		fd.setFaceIndex(quadIndex);

		// Generate two vertices for each column:
		//      topVert, then botVert

		curVert = topVert;
		matlIndex = getBindIndex(materialBinding, row, quadIndex, curVert);
		normIndex = getBindIndex(normalBinding,   row, quadIndex, curVert);
		tcIndex   = tcb.isFunction() ? 0 : curVert;

		pv.setPoint(ce->get3(curVert));
		pv.setNormal(nb.get(normIndex));
		pv.setMaterialIndex(matlIndex);

		if (! tcb.isFunction())
		    pv.setTextureCoords(tcb.get(tcIndex));

		if (! forPicking) {
		    if (tcb.isFunction())
			pv.setTextureCoords(tcb.get(pv.getPoint(),
						    pv.getNormal()));
		    pd.setCoordinateIndex(curVert);
		    pd.setMaterialIndex(matlIndex);
		    pd.setNormalIndex(normIndex);
		    pd.setTextureCoordIndex(tcIndex);
		}

		shapeVertex(&pv);

		curVert = botVert;
		matlIndex = getBindIndex(materialBinding, row, quadIndex, curVert);
		normIndex = getBindIndex(normalBinding,   row, quadIndex, curVert);
		tcIndex   = tcb.isFunction() ? 0 : curVert;

		pv.setPoint(ce->get3(curVert));
		pv.setNormal(nb.get(normIndex));
		pv.setMaterialIndex(matlIndex);

		if (! tcb.isFunction())
		    pv.setTextureCoords(tcb.get(tcIndex));

		if (! forPicking) {
		    if (tcb.isFunction())
			pv.setTextureCoords(tcb.get(pv.getPoint(),
						    pv.getNormal()));
		    pd.setCoordinateIndex(curVert);
		    pd.setMaterialIndex(matlIndex);
		    pd.setNormalIndex(normIndex);
		    pd.setTextureCoordIndex(tcIndex);
		}

		shapeVertex(&pv);

		topVert++;
		botVert++;
	    }

	    endShape();
	}
    }

    state->pop();
}

//////////////////////////////////////////////////////////////////////////
// Following preprocessor-generated routines handle all combinations of
// Normal binding (per vertex, per face, per part, overall/none)
// Color Binding (per vertex, per face, per part, overall)
// Textures (on or off)
//////////////////////////////////////////////////////////////////////////

// 32 different rendering loops; the 5 bits used to determine the
// rendering case are:
// 43210  BITS            Routine suffix
// -----  ----            --------------
// 00...  Overall mtl     (Om)
// 01...  Part mtl        (Pm)
// 10...  Face mtl        (Fm)
// 11...  Vtx mtl         (Vm)
// ..00.  Overall/No norm (On)
// ..01.  Part norm       (Pn)
// ..10.  Face norm       (Fn)
// ..11.  Vtx norm        (Vn)
// ....0  No texcoord     -none-
// ....1  Vtx texcoord    (T)
//
SoQuadMesh::PMQM SoQuadMesh::renderFunc[32] = {
    &SoQuadMesh::OmOn, &SoQuadMesh::OmOnT,
    &SoQuadMesh::OmPn, &SoQuadMesh::OmPnT,
    &SoQuadMesh::OmFn, &SoQuadMesh::OmFnT,
    &SoQuadMesh::OmVn, &SoQuadMesh::OmVnT,
    &SoQuadMesh::PmOn, &SoQuadMesh::PmOnT,
    &SoQuadMesh::PmPn, &SoQuadMesh::PmPnT,
    &SoQuadMesh::PmFn, &SoQuadMesh::PmFnT,
    &SoQuadMesh::PmVn, &SoQuadMesh::PmVnT,
    &SoQuadMesh::FmOn, &SoQuadMesh::FmOnT,
    &SoQuadMesh::FmPn, &SoQuadMesh::FmPnT,
    &SoQuadMesh::FmFn, &SoQuadMesh::FmFnT,
    &SoQuadMesh::FmVn, &SoQuadMesh::FmVnT,
    &SoQuadMesh::VmOn, &SoQuadMesh::VmOnT,
    &SoQuadMesh::VmPn, &SoQuadMesh::VmPnT,
    &SoQuadMesh::VmFn, &SoQuadMesh::VmFnT,
    &SoQuadMesh::VmVn, &SoQuadMesh::VmVnT,
    };



void
SoQuadMesh::
OmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*vertexFunc)(vertexPtr);

	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
OmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
OmPn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*vertexFunc)(vertexPtr);

	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
OmPnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
OmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
OmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
OmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride = normalStride*verticesPerRow.getValue();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*normalFunc)(normalPtr);
	    (*vertexFunc)(vertexPtr);

	    (*normalFunc)(normalPtr+normalRowStride);
	    normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
OmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride = normalStride*verticesPerRow.getValue();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*normalFunc)(normalPtr);
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*normalFunc)(normalPtr+normalRowStride);
	    normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
PmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*vertexFunc)(vertexPtr);

	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
PmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
PmPn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*vertexFunc)(vertexPtr);

	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
PmPnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
PmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
PmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
PmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride = normalStride*verticesPerRow.getValue();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*normalFunc)(normalPtr);
	    (*vertexFunc)(vertexPtr);

	    (*normalFunc)(normalPtr+normalRowStride);
	    normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
PmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride = normalStride*verticesPerRow.getValue();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*normalFunc)(normalPtr);
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*normalFunc)(normalPtr+normalRowStride);
	    normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
FmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
FmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
FmPn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
FmPnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
FmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
FmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*texCoordFunc)(texCoordPtr+texCoordStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
FmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride = normalStride*verticesPerRow.getValue();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); 
	    (*vertexFunc)(vertexPtr);
	    (*normalFunc)(normalPtr+normalRowStride); 
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*normalFunc)(normalPtr+normalStride+normalRowStride); 
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*normalFunc)(normalPtr+normalStride);
	    normalPtr += normalStride; 
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	normalPtr += normalStride; 


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
FmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride = normalStride*verticesPerRow.getValue();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); 
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);
	    (*normalFunc)(normalPtr+normalRowStride); 
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*normalFunc)(normalPtr+normalStride+normalRowStride); 
	    (*texCoordFunc)(texCoordPtr+texCoordStride+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*normalFunc)(normalPtr+normalStride);
	    normalPtr += normalStride; 
	    (*texCoordFunc)(texCoordPtr+texCoordStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	normalPtr += normalStride; 


//Do Strip rendering if both ~Fn and ~Fm

    }
}



void
SoQuadMesh::
VmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride*
	verticesPerRow.getValue();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*colorFunc)(colorPtr);
	    (*vertexFunc)(vertexPtr);

	    (*colorFunc)(colorPtr+colorRowStride);
	    colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
VmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride*
	verticesPerRow.getValue();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*colorFunc)(colorPtr);
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*colorFunc)(colorPtr+colorRowStride);
	    colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
VmPn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride*
	verticesPerRow.getValue();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*colorFunc)(colorPtr);
	    (*vertexFunc)(vertexPtr);

	    (*colorFunc)(colorPtr+colorRowStride);
	    colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
VmPnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride*
	verticesPerRow.getValue();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*colorFunc)(colorPtr);
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*colorFunc)(colorPtr+colorRowStride);
	    colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
VmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride*
	verticesPerRow.getValue();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*colorFunc)(colorPtr); 
	    (*vertexFunc)(vertexPtr);
	    (*colorFunc)(colorPtr+colorRowStride); 
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*colorFunc)(colorPtr+colorStride+colorRowStride); 
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*colorFunc)(colorPtr+colorStride); 
	    colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	colorPtr += colorStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
VmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride*
	verticesPerRow.getValue();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
    const int nv = verticesPerRow.getValue() - 1;

    int v;
    for (int row = 0; row < numRows; row++) {
	glBegin(GL_QUADS);
	for(v = 0; v < nv; v++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*colorFunc)(colorPtr); 
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);
	    (*colorFunc)(colorPtr+colorRowStride); 
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    (*colorFunc)(colorPtr+colorStride+colorRowStride); 
	    (*texCoordFunc)(texCoordPtr+texCoordStride+texCoordRowStride);
	    (*vertexFunc)(vertexPtr+vertexStride+vertexRowStride);
	    (*colorFunc)(colorPtr+colorStride); 
	    colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr+texCoordStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexStride); 
	    vertexPtr += vertexStride;
	}
	glEnd(); //GL_QUADS
	//At the end of a row skip over last vertex:
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	colorPtr += colorStride;


//Do Strip rendering if both ~Fn and ~Fm

    }
}


void
SoQuadMesh::
VmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride*
	verticesPerRow.getValue();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride = normalStride*verticesPerRow.getValue();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*colorFunc)(colorPtr);
	    (*normalFunc)(normalPtr);
	    (*vertexFunc)(vertexPtr);

	    (*colorFunc)(colorPtr+colorRowStride);
	    colorPtr += colorStride;
	    (*normalFunc)(normalPtr+normalRowStride);
	    normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


void
SoQuadMesh::
VmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const unsigned int vertexRowStride = vertexStride*
	verticesPerRow.getValue();
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    const unsigned int colorRowStride = colorStride*
	verticesPerRow.getValue();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    const unsigned int normalRowStride = normalStride*verticesPerRow.getValue();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const unsigned int texCoordRowStride = texCoordStride*
	verticesPerRow.getValue();

    const int numRows = verticesPerColumn.getValue() - 1;
     const int nv = verticesPerRow.getValue();

    int v;
    for (int row = 0; row < numRows; row++) {

//Do Strip rendering if both ~Fn and ~Fm
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv; v++) {

	    (*colorFunc)(colorPtr);
	    (*normalFunc)(normalPtr);
	    (*texCoordFunc)(texCoordPtr);
	    (*vertexFunc)(vertexPtr);

	    (*colorFunc)(colorPtr+colorRowStride);
	    colorPtr += colorStride;
	    (*normalFunc)(normalPtr+normalRowStride);
	    normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr+texCoordRowStride);
	    texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr+vertexRowStride);
	    vertexPtr += vertexStride;
	}

	glEnd();

    }
}


