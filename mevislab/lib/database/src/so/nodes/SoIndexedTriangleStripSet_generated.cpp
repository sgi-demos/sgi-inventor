//////////////////////////////////////////////////////////////////////////
// Following preprocessor-generated routines handle all combinations of
// Normal binding (per vertex, per face, per part, overall/none)
// Color Binding (per vertex, per face, per part, overall)
// Textures (on or off)
//////////////////////////////////////////////////////////////////////////

// Material overall:

void
SoIndexedTriangleStripSet::
OmOn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
OmOnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
OmPn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
OmPnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
OmFn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int nrmCtr=0;
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);    
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
OmFnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int nrmCtr=0;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);    
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
OmVn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);       
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
OmVnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);       
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
PmOn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
PmOnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
PmPn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
PmPnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
PmFn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int nrmCtr=0;
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);    
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
PmFnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int nrmCtr=0;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);    
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
PmVn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);       
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
PmVnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);       
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
FmOn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int clrCtr=0;
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                 
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                  
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);     
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
FmOnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int clrCtr=0;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                 
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                  
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);     
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
FmPn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int clrCtr=0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                 
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                  
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);     
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
FmPnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int clrCtr=0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                 
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                  
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);     
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
FmFn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int clrCtr=0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int nrmCtr=0;
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                 
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);     
	    (*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);    
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::
FmFnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int clrCtr=0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
    int nrmCtr=0;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    glShadeModel(GL_FLAT);
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                 
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    // Per-face cases:
	    if (v) {
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
	    }
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);     
	    (*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);    
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
    glShadeModel(GL_SMOOTH);
}


void
SoIndexedTriangleStripSet::FmVn(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int ns = numStrips;
    const int *numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int32_t *const normalIndx = getNormalIndices();
    const int32_t *const colorIndx = getColorIndices();

    unsigned int f;
    unsigned int v = 0;
    int clrCtr = 0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < ns; strip++) {
	const int nf = (*numverts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*normalFunc)(normalPtr+normalIndx[(v-0)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-1)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-2)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
	    } else {
		(*normalFunc)(normalPtr+normalIndx[(v-2)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-1)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-0)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
	    }
	    ++v;
	}
	++numverts;
	++v;
    }
    glEnd();

}

void
SoIndexedTriangleStripSet::FmVnT(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int ns = numStrips;
    const int *numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int32_t *const normalIndx = getNormalIndices();
    const int32_t *const colorIndx = getColorIndices();
    const int32_t *const tCoordIndx = getTexCoordIndices();

    unsigned int f;
    unsigned int v = 0;
    int clrCtr =0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < ns; strip++) {
	const int nf = (*numverts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*normalFunc)(normalPtr+normalIndx[(v-0)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-0)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-1)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-1)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-2)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-2)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
	    } else {
		(*normalFunc)(normalPtr+normalIndx[(v-2)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-2)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-1)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-1)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-0)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-0)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
	    }
	    ++v;
	}
	++numverts;
	++v;
    }
    glEnd();

}

void
SoIndexedTriangleStripSet::
VmOn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);                  
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);           
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
VmOnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);                  
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);           
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
VmPn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);                  
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);           
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
VmPnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);                  
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);           
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::VmFn(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int ns = numStrips;
    const int * numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int32_t *const normalIndx = getNormalIndices();
    const int32_t *const colorIndx = getColorIndices();

    unsigned int f;
    unsigned int v = 0;
    int nrmCtr=0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < ns; strip++) {
	const int nf = (*numverts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*normalFunc)(normalPtr+normalIndx[nrmCtr++]*normalStride);

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*colorFunc)(colorPtr+colorIndx[(v-0)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-1)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-2)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
	    } else {
		(*colorFunc)(colorPtr+colorIndx[(v-2)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-1)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-0)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
	    }
	    ++v;
	}
	++numverts;
	++v;
    }
    glEnd();

}
void
SoIndexedTriangleStripSet::VmFnT(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int ns = numStrips;
    const int *numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int32_t *const normalIndx = getNormalIndices();
    const int32_t *const colorIndx = getColorIndices();
    const int32_t *const tCoordIndx = getTexCoordIndices();

    unsigned int f;
    unsigned int v = 0;
    int nrmCtr = 0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < ns; strip++) {
	const int nf = (*numverts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:

	    (*normalFunc)(normalPtr+normalIndx[nrmCtr++]*normalStride);

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*colorFunc)(colorPtr+colorIndx[(v-0)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-0)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-1)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-1)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-2)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-2)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
	    } else {
		(*colorFunc)(colorPtr+colorIndx[(v-2)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-2)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-1)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-1)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-0)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-0)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
	    }
	    ++v;
	}
	++numverts;
	++v;
    }
    glEnd();

}

void
SoIndexedTriangleStripSet::
VmVn
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);                  
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);       
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);           
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}


void
SoIndexedTriangleStripSet::
VmVnT
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
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
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);

	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);                  
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);           
	}
	if (v < nv) { // Leftovers
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);           
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);         
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);         
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
}

