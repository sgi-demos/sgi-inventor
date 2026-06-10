//////////////////////////////////////////////////////////////////////////
// Following preprocessor-generated routines handle all combinations of
// Normal binding (per vertex, per face, per part, overall/none)
// Color Binding (per vertex, per face, per part, overall)
// Textures (on or off)
//////////////////////////////////////////////////////////////////////////

// Material overall:

void
SoIndexedLineSet::
OmOn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
OmOnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
OmPn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    int nrmCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
OmPnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    int nrmCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
OmFn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
OmFnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
OmVn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
OmVnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
PmOn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int clrCtr = 0;

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
PmOnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int clrCtr = 0;

    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
PmPn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int nrmCtr = 0;
    int clrCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
PmPnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int nrmCtr = 0;
    int clrCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
PmFn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int clrCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
PmFnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int clrCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
PmVn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int clrCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);   

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
PmVnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int clrCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);   

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
FmOn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
FmOnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
FmPn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int nrmCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
FmPnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int nrmCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
FmFn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
FmFnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
FmVn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
FmVnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
VmOn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);              
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
VmOnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);              
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
VmPn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int nrmCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);              
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
VmPnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
    int nrmCtr = 0;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();

    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);             
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);              
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	}
	glEnd();
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
	++numverts;
    }
}


void
SoIndexedLineSet::
VmFn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);              
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
VmFnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);              
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
VmVn
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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

    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);              
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}


void
SoIndexedLineSet::
VmVnT
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
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
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);              
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);        
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);                     
	}
	glEnd();
	vtxCtr++;  //skip over -1 at end of polyline
	++numverts;
    }
}

