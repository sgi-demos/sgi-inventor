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
SoIndexedTriangleSet::PMFS SoIndexedTriangleSet::TriRenderFunc[32] = {
    &SoIndexedTriangleSet::TriOmOn, &SoIndexedTriangleSet::TriOmOnT,
    &SoIndexedTriangleSet::TriOmFn, &SoIndexedTriangleSet::TriOmFnT,
    &SoIndexedTriangleSet::TriOmFn, &SoIndexedTriangleSet::TriOmFnT,
    &SoIndexedTriangleSet::TriOmVn, &SoIndexedTriangleSet::TriOmVnT,
    &SoIndexedTriangleSet::TriFmOn, &SoIndexedTriangleSet::TriFmOnT,
    &SoIndexedTriangleSet::TriFmFn, &SoIndexedTriangleSet::TriFmFnT,
    &SoIndexedTriangleSet::TriFmFn, &SoIndexedTriangleSet::TriFmFnT,
    &SoIndexedTriangleSet::TriFmVn, &SoIndexedTriangleSet::TriFmVnT,
    &SoIndexedTriangleSet::TriFmOn, &SoIndexedTriangleSet::TriFmOnT,
    &SoIndexedTriangleSet::TriFmFn, &SoIndexedTriangleSet::TriFmFnT,
    &SoIndexedTriangleSet::TriFmFn, &SoIndexedTriangleSet::TriFmFnT,
    &SoIndexedTriangleSet::TriFmVn, &SoIndexedTriangleSet::TriFmVnT,
    &SoIndexedTriangleSet::TriVmOn, &SoIndexedTriangleSet::TriVmOnT,
    &SoIndexedTriangleSet::TriVmFn, &SoIndexedTriangleSet::TriVmFnT,
    &SoIndexedTriangleSet::TriVmFn, &SoIndexedTriangleSet::TriVmFnT,
    &SoIndexedTriangleSet::TriVmVn, &SoIndexedTriangleSet::TriVmVnT,
    };

void
SoIndexedTriangleSet::
TriOmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriOmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriOmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriOmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriOmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriOmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
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
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriFmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriFmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriFmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriFmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
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
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriFmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriFmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
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
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriVmOn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriVmOnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
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
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriVmFn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriVmFnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
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
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriVmVn
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
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
	vtxCtr += 3;
    }
    glEnd();
}


void
SoIndexedTriangleSet::
TriVmVnT
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool sendAdj = sendAdjacency.getValue();
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

    glBegin(sendAdj?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);
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
	vtxCtr += 3;
    }
    glEnd();
}

