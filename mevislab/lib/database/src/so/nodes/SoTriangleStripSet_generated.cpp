
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
SoTriangleStripSet::PMTSS SoTriangleStripSet::renderFunc[32] = {
    &SoTriangleStripSet::OmOn, &SoTriangleStripSet::OmOnT,
    &SoTriangleStripSet::OmPn, &SoTriangleStripSet::OmPnT,
    &SoTriangleStripSet::OmFn, &SoTriangleStripSet::OmFnT,
    &SoTriangleStripSet::OmVn, &SoTriangleStripSet::OmVnT,
    &SoTriangleStripSet::PmOn, &SoTriangleStripSet::PmOnT,
    &SoTriangleStripSet::PmPn, &SoTriangleStripSet::PmPnT,
    &SoTriangleStripSet::PmFn, &SoTriangleStripSet::PmFnT,
    &SoTriangleStripSet::PmVn, &SoTriangleStripSet::PmVnT,
    &SoTriangleStripSet::FmOn, &SoTriangleStripSet::FmOnT,
    &SoTriangleStripSet::FmPn, &SoTriangleStripSet::FmPnT,
    &SoTriangleStripSet::FmFn, &SoTriangleStripSet::FmFnT,
    &SoTriangleStripSet::FmVn, &SoTriangleStripSet::FmVnT,
    &SoTriangleStripSet::VmOn, &SoTriangleStripSet::VmOnT,
    &SoTriangleStripSet::VmPn, &SoTriangleStripSet::VmPnT,
    &SoTriangleStripSet::VmFn, &SoTriangleStripSet::VmFnT,
    &SoTriangleStripSet::VmVn, &SoTriangleStripSet::VmVnT,
    };

// Material overall:

void
SoTriangleStripSet::
OmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
OmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
OmPn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
OmPnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
OmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
OmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
OmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*normalFunc)(normalPtr+0*normalStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*normalFunc)(normalPtr+1*normalStride);
	    normalPtr += 2*normalStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
OmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*normalFunc)(normalPtr+0*normalStride);
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*normalFunc)(normalPtr+1*normalStride);
	    normalPtr += 2*normalStride;
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
PmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
PmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
PmPn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
PmPnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
PmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
PmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
PmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*normalFunc)(normalPtr+0*normalStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*normalFunc)(normalPtr+1*normalStride);
	    normalPtr += 2*normalStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
PmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*normalFunc)(normalPtr+0*normalStride);
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*normalFunc)(normalPtr+1*normalStride);
	    normalPtr += 2*normalStride;
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
FmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
	    }
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
	    }
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
FmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
	    }
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
	    }
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
FmPn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
	    }
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
	    }
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
FmPnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
	    }
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
	    }
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
FmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::
FmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    glShadeModel(GL_FLAT);
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr); colorPtr += colorStride;
		(*normalFunc)(normalPtr); normalPtr += normalStride;
	    }
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoTriangleStripSet::FmVn(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    unsigned int f;
    unsigned int v = 0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < numStrips; strip++) {
	const int nf = (*numVerts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*colorFunc)(colorPtr); colorPtr += colorStride;

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*normalFunc)(normalPtr+(v-0)*normalStride);
		(*vertexFunc)(vertexPtr+(v-0)*vertexStride);
		(*normalFunc)(normalPtr+(v-1)*normalStride);
		(*vertexFunc)(vertexPtr+(v-1)*vertexStride);
		(*normalFunc)(normalPtr+(v-2)*normalStride);
		(*vertexFunc)(vertexPtr+(v-2)*vertexStride);
	    } else {
		(*normalFunc)(normalPtr+(v-2)*normalStride);
		(*vertexFunc)(vertexPtr+(v-2)*vertexStride);
		(*normalFunc)(normalPtr+(v-1)*normalStride);
		(*vertexFunc)(vertexPtr+(v-1)*vertexStride);
		(*normalFunc)(normalPtr+(v-0)*normalStride);
		(*vertexFunc)(vertexPtr+(v-0)*vertexStride);
	    }
	    ++v;
	}
	++numVerts;
    }
    glEnd();
}

void
SoTriangleStripSet::FmVnT(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    unsigned int f;
    unsigned int v = 0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < numStrips; strip++) {
	const int nf = (*numVerts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*colorFunc)(colorPtr); colorPtr += colorStride;

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*normalFunc)(normalPtr+(v-0)*normalStride);
		(*texCoordFunc)(texCoordPtr+(v-0)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-0)*vertexStride);
		(*normalFunc)(normalPtr+(v-1)*normalStride);
		(*texCoordFunc)(texCoordPtr+(v-1)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-1)*vertexStride);
		(*normalFunc)(normalPtr+(v-2)*normalStride);
		(*texCoordFunc)(texCoordPtr+(v-2)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-2)*vertexStride);
	    } else {
		(*normalFunc)(normalPtr+(v-2)*normalStride);
		(*texCoordFunc)(texCoordPtr+(v-2)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-2)*vertexStride);
		(*normalFunc)(normalPtr+(v-1)*normalStride);
		(*texCoordFunc)(texCoordPtr+(v-1)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-1)*vertexStride);
		(*normalFunc)(normalPtr+(v-0)*normalStride);
		(*texCoordFunc)(texCoordPtr+(v-0)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-0)*vertexStride);
	    }
	    ++v;
	}
	++numVerts;
    }
    glEnd();
}

void
SoTriangleStripSet::
VmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+0*colorStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*colorFunc)(colorPtr+1*colorStride);
	    colorPtr += 2*colorStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
VmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+0*colorStride);
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*colorFunc)(colorPtr+1*colorStride);
	    colorPtr += 2*colorStride;
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
VmPn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+0*colorStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*colorFunc)(colorPtr+1*colorStride);
	    colorPtr += 2*colorStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
VmPnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+0*colorStride);
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*colorFunc)(colorPtr+1*colorStride);
	    colorPtr += 2*colorStride;
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::VmFn(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    unsigned int f;
    unsigned int v = 0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < numStrips; strip++) {
	const int nf = (*numVerts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*normalFunc)(normalPtr); normalPtr += normalStride;

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*colorFunc)(colorPtr+(v-0)*colorStride);
		(*vertexFunc)(vertexPtr+(v-0)*vertexStride);
		(*colorFunc)(colorPtr+(v-1)*colorStride);
		(*vertexFunc)(vertexPtr+(v-1)*vertexStride);
		(*colorFunc)(colorPtr+(v-2)*colorStride);
		(*vertexFunc)(vertexPtr+(v-2)*vertexStride);
	    } else {
		(*colorFunc)(colorPtr+(v-2)*colorStride);
		(*vertexFunc)(vertexPtr+(v-2)*vertexStride);
		(*colorFunc)(colorPtr+(v-1)*colorStride);
		(*vertexFunc)(vertexPtr+(v-1)*vertexStride);
		(*colorFunc)(colorPtr+(v-0)*colorStride);
		(*vertexFunc)(vertexPtr+(v-0)*vertexStride);
	    }
	    ++v;
	}
	++numVerts;
    }
    glEnd();
}
void
SoTriangleStripSet::VmFnT(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    unsigned int f;
    unsigned int v = 0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < numStrips; strip++) {
	const int nf = (*numVerts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*normalFunc)(normalPtr); normalPtr += normalStride;

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*colorFunc)(colorPtr+(v-0)*colorStride);
		(*texCoordFunc)(texCoordPtr+(v-0)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-0)*vertexStride);
		(*colorFunc)(colorPtr+(v-1)*colorStride);
		(*texCoordFunc)(texCoordPtr+(v-1)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-1)*vertexStride);
		(*colorFunc)(colorPtr+(v-2)*colorStride);
		(*texCoordFunc)(texCoordPtr+(v-2)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-2)*vertexStride);
	    } else {
		(*colorFunc)(colorPtr+(v-2)*colorStride);
		(*texCoordFunc)(texCoordPtr+(v-2)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-2)*vertexStride);
		(*colorFunc)(colorPtr+(v-1)*colorStride);
		(*texCoordFunc)(texCoordPtr+(v-1)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-1)*vertexStride);
		(*colorFunc)(colorPtr+(v-0)*colorStride);
		(*texCoordFunc)(texCoordPtr+(v-0)*texCoordStride);
		(*vertexFunc)(vertexPtr+(v-0)*vertexStride);
	    }
	    ++v;
	}
	++numVerts;
    }
    glEnd();
}

void
SoTriangleStripSet::
VmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+0*colorStride);
	    (*normalFunc)(normalPtr+0*normalStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*colorFunc)(colorPtr+1*colorStride);
	    colorPtr += 2*colorStride;
	    (*normalFunc)(normalPtr+1*normalStride);
	    normalPtr += 2*normalStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}


void
SoTriangleStripSet::
VmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int numStrips = numVertices.getNum();
    const int32_t *numVerts = numVertices.getValues(0);

    int v;
    for (int strip = 0; strip < numStrips; strip++) {
	const int nv = (*numVerts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+0*colorStride);
	    (*texCoordFunc)(texCoordPtr+0*texCoordStride);
	    (*vertexFunc)(vertexPtr+0*vertexStride);
	    (*colorFunc)(colorPtr+1*colorStride);
	    colorPtr += 2*colorStride;
	    (*texCoordFunc)(texCoordPtr+1*texCoordStride);
	    texCoordPtr += 2*texCoordStride;
	    (*vertexFunc)(vertexPtr+1*vertexStride);
	    vertexPtr += 2*vertexStride;
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}
	glEnd();
	++numVerts;
    }
}

