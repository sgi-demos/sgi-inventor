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
// 01...  Part mtl        (Pm)  NOT GENERATED, Fm is same!
// 10...  Face mtl        (Fm)
// 11...  Vtx mtl         (Vm)
// ..00.  Overall/No norm (On)
// ..01.  Part norm       (Pn)  NOT GENERATED, Fn is same!
// ..10.  Face norm       (Fn)
// ..11.  Vtx norm        (Vn)
// ....0  No texcoord     -none-
// ....1  Vtx texcoord    (T)
//
SoIndexedFaceSet::PMFS SoIndexedFaceSet::TriRenderFunc[32] = {
    &SoIndexedFaceSet::TriOmOn, &SoIndexedFaceSet::TriOmOnT,
    &SoIndexedFaceSet::TriOmFn, &SoIndexedFaceSet::TriOmFnT,
    &SoIndexedFaceSet::TriOmFn, &SoIndexedFaceSet::TriOmFnT,
    &SoIndexedFaceSet::TriOmVn, &SoIndexedFaceSet::TriOmVnT,
    &SoIndexedFaceSet::TriFmOn, &SoIndexedFaceSet::TriFmOnT,
    &SoIndexedFaceSet::TriFmFn, &SoIndexedFaceSet::TriFmFnT,
    &SoIndexedFaceSet::TriFmFn, &SoIndexedFaceSet::TriFmFnT,
    &SoIndexedFaceSet::TriFmVn, &SoIndexedFaceSet::TriFmVnT,
    &SoIndexedFaceSet::TriFmOn, &SoIndexedFaceSet::TriFmOnT,
    &SoIndexedFaceSet::TriFmFn, &SoIndexedFaceSet::TriFmFnT,
    &SoIndexedFaceSet::TriFmFn, &SoIndexedFaceSet::TriFmFnT,
    &SoIndexedFaceSet::TriFmVn, &SoIndexedFaceSet::TriFmVnT,
    &SoIndexedFaceSet::TriVmOn, &SoIndexedFaceSet::TriVmOnT,
    &SoIndexedFaceSet::TriVmFn, &SoIndexedFaceSet::TriVmFnT,
    &SoIndexedFaceSet::TriVmFn, &SoIndexedFaceSet::TriVmFnT,
    &SoIndexedFaceSet::TriVmVn, &SoIndexedFaceSet::TriVmVnT,
    };
SoIndexedFaceSet::PMFS SoIndexedFaceSet::QuadRenderFunc[32] = {
    &SoIndexedFaceSet::QuadOmOn, &SoIndexedFaceSet::QuadOmOnT,
    &SoIndexedFaceSet::QuadOmFn, &SoIndexedFaceSet::QuadOmFnT,
    &SoIndexedFaceSet::QuadOmFn, &SoIndexedFaceSet::QuadOmFnT,
    &SoIndexedFaceSet::QuadOmVn, &SoIndexedFaceSet::QuadOmVnT,
    &SoIndexedFaceSet::QuadFmOn, &SoIndexedFaceSet::QuadFmOnT,
    &SoIndexedFaceSet::QuadFmFn, &SoIndexedFaceSet::QuadFmFnT,
    &SoIndexedFaceSet::QuadFmFn, &SoIndexedFaceSet::QuadFmFnT,
    &SoIndexedFaceSet::QuadFmVn, &SoIndexedFaceSet::QuadFmVnT,
    &SoIndexedFaceSet::QuadFmOn, &SoIndexedFaceSet::QuadFmOnT,
    &SoIndexedFaceSet::QuadFmFn, &SoIndexedFaceSet::QuadFmFnT,
    &SoIndexedFaceSet::QuadFmFn, &SoIndexedFaceSet::QuadFmFnT,
    &SoIndexedFaceSet::QuadFmVn, &SoIndexedFaceSet::QuadFmVnT,
    &SoIndexedFaceSet::QuadVmOn, &SoIndexedFaceSet::QuadVmOnT,
    &SoIndexedFaceSet::QuadVmFn, &SoIndexedFaceSet::QuadVmFnT,
    &SoIndexedFaceSet::QuadVmFn, &SoIndexedFaceSet::QuadVmFnT,
    &SoIndexedFaceSet::QuadVmVn, &SoIndexedFaceSet::QuadVmVnT,
    };
SoIndexedFaceSet::PMFS SoIndexedFaceSet::GenRenderFunc[32] = {
    &SoIndexedFaceSet::GenOmOn, &SoIndexedFaceSet::GenOmOnT,
    &SoIndexedFaceSet::GenOmFn, &SoIndexedFaceSet::GenOmFnT,
    &SoIndexedFaceSet::GenOmFn, &SoIndexedFaceSet::GenOmFnT,
    &SoIndexedFaceSet::GenOmVn, &SoIndexedFaceSet::GenOmVnT,
    &SoIndexedFaceSet::GenFmOn, &SoIndexedFaceSet::GenFmOnT,
    &SoIndexedFaceSet::GenFmFn, &SoIndexedFaceSet::GenFmFnT,
    &SoIndexedFaceSet::GenFmFn, &SoIndexedFaceSet::GenFmFnT,
    &SoIndexedFaceSet::GenFmVn, &SoIndexedFaceSet::GenFmVnT,
    &SoIndexedFaceSet::GenFmOn, &SoIndexedFaceSet::GenFmOnT,
    &SoIndexedFaceSet::GenFmFn, &SoIndexedFaceSet::GenFmFnT,
    &SoIndexedFaceSet::GenFmFn, &SoIndexedFaceSet::GenFmFnT,
    &SoIndexedFaceSet::GenFmVn, &SoIndexedFaceSet::GenFmVnT,
    &SoIndexedFaceSet::GenVmOn, &SoIndexedFaceSet::GenVmOnT,
    &SoIndexedFaceSet::GenVmFn, &SoIndexedFaceSet::GenVmFnT,
    &SoIndexedFaceSet::GenVmFn, &SoIndexedFaceSet::GenVmFnT,
    &SoIndexedFaceSet::GenVmVn, &SoIndexedFaceSet::GenVmVnT,
    };

void
SoIndexedFaceSet::
TriOmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadOmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    for (int quad = 0; quad < numQuads; quad++) {

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenOmOn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    int vtxCtr = numQuads*5 + numTris*4;
    while (vtxCtr < numVI) {
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriOmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadOmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    for (int quad = 0; quad < numQuads; quad++) {

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenOmOnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    while (vtxCtr < numVI) {
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriOmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadOmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenOmFn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriOmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadOmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenOmFnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriOmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadOmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    for (int quad = 0; quad < numQuads; quad++) {

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenOmVn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    while (vtxCtr < numVI) {
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriOmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadOmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    for (int quad = 0; quad < numQuads; quad++) {

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+3]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenOmVnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    while (vtxCtr < numVI) {
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}



void
SoIndexedFaceSet::
TriFmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadFmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	++faceCtr;

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenFmOn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriFmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadFmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	++faceCtr;

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenFmOnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriFmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadFmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenFmFn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriFmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadFmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenFmFnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriFmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadFmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	++faceCtr;

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenFmVn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriFmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadFmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	++faceCtr;

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+3]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenFmVnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}



void
SoIndexedFaceSet::
TriVmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadVmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    for (int quad = 0; quad < numQuads; quad++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenVmOn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    while (vtxCtr < numVI) {
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriVmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadVmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    for (int quad = 0; quad < numQuads; quad++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+3]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenVmOnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    while (vtxCtr < numVI) {
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriVmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadVmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenVmFn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriVmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadVmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    int faceCtr = numTris;
    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+3]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenVmFnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    int faceCtr = numQuads + numTris;
    while (vtxCtr < numVI) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
	++faceCtr;
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriVmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadVmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    for (int quad = 0; quad < numQuads; quad++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+3]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenVmVn
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    while (vtxCtr < numVI) {
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


void
SoIndexedFaceSet::
TriVmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
QuadVmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
    for (int quad = 0; quad < numQuads; quad++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+3]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+3]);
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

void
SoIndexedFaceSet::
GenVmVnT
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = numQuads*5 + numTris*4;
    while (vtxCtr < numVI) {
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}


