
//////////////////////////////////////////////////////////////////////////
// Following preprocessor-generated routines handle all combinations of
// Normal binding (per vertex, per face, per part, overall/none)
// Color Binding (per vertex, per face, per part, overall)
// Textures (on or off)
//////////////////////////////////////////////////////////////////////////

// Material overall:

void
SoLineSet::
OmOn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
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
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
OmOnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
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
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
OmPn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
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
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;              
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	++numverts;
    }
}


void
SoLineSet::
OmPnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
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
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;              
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;

		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*texCoordFunc)(texCoordPtr);
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	++numverts;
    }
}


void
SoLineSet::
OmFn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr);
	normalPtr += normalStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
OmFnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr);
	normalPtr += normalStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
OmVn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
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
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
OmVnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
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
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
PmOn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
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
		(*colorFunc)(colorPtr);
		colorPtr += colorStride; 
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	++numverts;
    }
}


void
SoLineSet::
PmOnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
		(*colorFunc)(colorPtr);
		colorPtr += colorStride; 
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;

		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*texCoordFunc)(texCoordPtr);
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	++numverts;
    }
}


void
SoLineSet::
PmPn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
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
		(*colorFunc)(colorPtr);
		colorPtr += colorStride; 
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;              
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	++numverts;
    }
}


void
SoLineSet::
PmPnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
		(*colorFunc)(colorPtr);
		colorPtr += colorStride; 
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;              
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;

		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*texCoordFunc)(texCoordPtr);
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	++numverts;
    }
}


void
SoLineSet::
PmFn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr);
	normalPtr += normalStride;
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr);
		colorPtr += colorStride; 
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	++numverts;
    }
}


void
SoLineSet::
PmFnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr);
	normalPtr += normalStride;
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr);
		colorPtr += colorStride; 
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;

		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*texCoordFunc)(texCoordPtr);
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	++numverts;
    }
}


void
SoLineSet::
PmVn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);
	    (*normalFunc)(normalPtr);
	    normalPtr += normalStride;  

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr);
		colorPtr += colorStride; 
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	++numverts;
    }
}


void
SoLineSet::
PmVnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);
	    (*normalFunc)(normalPtr);
	    normalPtr += normalStride;  

	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr);
		colorPtr += colorStride; 
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;

		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;
		(*texCoordFunc)(texCoordPtr);
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	++numverts;
    }
}


void
SoLineSet::
FmOn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr);
	colorPtr+=colorStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
FmOnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr);
	colorPtr+=colorStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
FmPn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr);
	colorPtr+=colorStride;
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;              
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	++numverts;
    }
}


void
SoLineSet::
FmPnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr);
	colorPtr+=colorStride;
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;              
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;

		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*texCoordFunc)(texCoordPtr);
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	++numverts;
    }
}


void
SoLineSet::
FmFn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr);
	colorPtr+=colorStride;
	(*normalFunc)(normalPtr);
	normalPtr += normalStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
FmFnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr);
	colorPtr+=colorStride;
	(*normalFunc)(normalPtr);
	normalPtr += normalStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
FmVn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr);
	colorPtr+=colorStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
FmVnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*colorFunc)(colorPtr);
	colorPtr+=colorStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
VmOn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
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
		(*colorFunc)(colorPtr);
		colorPtr += colorStride;                   
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
VmOnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
		(*colorFunc)(colorPtr);
		colorPtr += colorStride;                   
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
VmPn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);
	    (*colorFunc)(colorPtr);
	    colorPtr += colorStride;

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;              
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*colorFunc)(colorPtr);
		colorPtr += colorStride;                   
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	++numverts;
    }
}


void
SoLineSet::
VmPnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	const int nv = (*numverts) -1;
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
	    glBegin(sendAdj?GL_LINES_ADJACENCY:GL_LINES);
	    (*colorFunc)(colorPtr);
	    colorPtr += colorStride;

	}
	for (v = 0; v < nv; v++) {                  
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;              
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;

		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;
		(*colorFunc)(colorPtr);
		colorPtr += colorStride;                   
		(*texCoordFunc)(texCoordPtr);
		(*vertexFunc)(vertexPtr);

	}
	glEnd();
	vertexPtr += vertexStride;
	texCoordPtr += texCoordStride;
	++numverts;
    }
}


void
SoLineSet::
VmFn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr);
	normalPtr += normalStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr);
		colorPtr += colorStride;                   
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
VmFnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

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
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
	(*normalFunc)(normalPtr);
	normalPtr += normalStride;
	const int nv = (*numverts);      
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {

	    glBegin(sendAdj?GL_LINE_STRIP_ADJACENCY:GL_LINE_STRIP);      
	}
	for (v = 0; v < nv; v++) {                  
		(*colorFunc)(colorPtr);
		colorPtr += colorStride;                   
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
VmVn
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
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
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;
		(*colorFunc)(colorPtr);
		colorPtr += colorStride;                   
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}


void
SoLineSet::
VmVnT
    (SoGLRenderAction* action ) {
    const int np = numVertices.getNum();
    const int32_t* numverts = numVertices.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);
    SbBool sendAdj = sendAdjacency.getValue();

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue());
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
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
		(*normalFunc)(normalPtr);
		normalPtr += normalStride;
		(*colorFunc)(colorPtr);
		colorPtr += colorStride;                   
		(*texCoordFunc)(texCoordPtr);
		texCoordPtr += texCoordStride;         
		(*vertexFunc)(vertexPtr);
		vertexPtr += vertexStride;                       
	}
	glEnd();
	++numverts;
    }
}

