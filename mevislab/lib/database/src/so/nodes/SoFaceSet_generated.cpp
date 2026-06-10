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
SoFaceSet::PMFS SoFaceSet::TriRenderFunc[32] = {
    &SoFaceSet::TriOmOn, &SoFaceSet::TriOmOnT,
    &SoFaceSet::TriOmFn, &SoFaceSet::TriOmFnT,
    &SoFaceSet::TriOmFn, &SoFaceSet::TriOmFnT,
    &SoFaceSet::TriOmVn, &SoFaceSet::TriOmVnT,
    &SoFaceSet::TriFmOn, &SoFaceSet::TriFmOnT,
    &SoFaceSet::TriFmFn, &SoFaceSet::TriFmFnT,
    &SoFaceSet::TriFmFn, &SoFaceSet::TriFmFnT,
    &SoFaceSet::TriFmVn, &SoFaceSet::TriFmVnT,
    &SoFaceSet::TriFmOn, &SoFaceSet::TriFmOnT,
    &SoFaceSet::TriFmFn, &SoFaceSet::TriFmFnT,
    &SoFaceSet::TriFmFn, &SoFaceSet::TriFmFnT,
    &SoFaceSet::TriFmVn, &SoFaceSet::TriFmVnT,
    &SoFaceSet::TriVmOn, &SoFaceSet::TriVmOnT,
    &SoFaceSet::TriVmFn, &SoFaceSet::TriVmFnT,
    &SoFaceSet::TriVmFn, &SoFaceSet::TriVmFnT,
    &SoFaceSet::TriVmVn, &SoFaceSet::TriVmVnT,
    };
SoFaceSet::PMFS SoFaceSet::QuadRenderFunc[32] = {
    &SoFaceSet::QuadOmOn, &SoFaceSet::QuadOmOnT,
    &SoFaceSet::QuadOmFn, &SoFaceSet::QuadOmFnT,
    &SoFaceSet::QuadOmFn, &SoFaceSet::QuadOmFnT,
    &SoFaceSet::QuadOmVn, &SoFaceSet::QuadOmVnT,
    &SoFaceSet::QuadFmOn, &SoFaceSet::QuadFmOnT,
    &SoFaceSet::QuadFmFn, &SoFaceSet::QuadFmFnT,
    &SoFaceSet::QuadFmFn, &SoFaceSet::QuadFmFnT,
    &SoFaceSet::QuadFmVn, &SoFaceSet::QuadFmVnT,
    &SoFaceSet::QuadFmOn, &SoFaceSet::QuadFmOnT,
    &SoFaceSet::QuadFmFn, &SoFaceSet::QuadFmFnT,
    &SoFaceSet::QuadFmFn, &SoFaceSet::QuadFmFnT,
    &SoFaceSet::QuadFmVn, &SoFaceSet::QuadFmVnT,
    &SoFaceSet::QuadVmOn, &SoFaceSet::QuadVmOnT,
    &SoFaceSet::QuadVmFn, &SoFaceSet::QuadVmFnT,
    &SoFaceSet::QuadVmFn, &SoFaceSet::QuadVmFnT,
    &SoFaceSet::QuadVmVn, &SoFaceSet::QuadVmVnT,
    };
SoFaceSet::PMFS SoFaceSet::GenRenderFunc[32] = {
    &SoFaceSet::GenOmOn, &SoFaceSet::GenOmOnT,
    &SoFaceSet::GenOmFn, &SoFaceSet::GenOmFnT,
    &SoFaceSet::GenOmFn, &SoFaceSet::GenOmFnT,
    &SoFaceSet::GenOmVn, &SoFaceSet::GenOmVnT,
    &SoFaceSet::GenFmOn, &SoFaceSet::GenFmOnT,
    &SoFaceSet::GenFmFn, &SoFaceSet::GenFmFnT,
    &SoFaceSet::GenFmFn, &SoFaceSet::GenFmFnT,
    &SoFaceSet::GenFmVn, &SoFaceSet::GenFmVnT,
    &SoFaceSet::GenFmOn, &SoFaceSet::GenFmOnT,
    &SoFaceSet::GenFmFn, &SoFaceSet::GenFmFnT,
    &SoFaceSet::GenFmFn, &SoFaceSet::GenFmFnT,
    &SoFaceSet::GenFmVn, &SoFaceSet::GenFmVnT,
    &SoFaceSet::GenVmOn, &SoFaceSet::GenVmOnT,
    &SoFaceSet::GenVmFn, &SoFaceSet::GenVmFnT,
    &SoFaceSet::GenVmFn, &SoFaceSet::GenVmFnT,
    &SoFaceSet::GenVmVn, &SoFaceSet::GenVmVnT,
    };

void
SoFaceSet::
TriOmOn
    (SoGLRenderAction* ) {
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadOmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenOmOn
    (SoGLRenderAction* )
{

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriOmOnT
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadOmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenOmOnT
    (SoGLRenderAction* )
{

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriOmFn
    (SoGLRenderAction* ) {
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadOmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenOmFn
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(numTris+numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriOmFnT
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadOmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenOmFnT
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(numTris+numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriOmVn
    (SoGLRenderAction* ) {
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue());
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue());
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadOmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue()+3*numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenOmVn
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = 
	vpCache.getNormals(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriOmVnT
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadOmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue()+3*numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenOmVnT
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *normalPtr = 
	vpCache.getNormals(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}



void
SoFaceSet::
TriFmOn
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadFmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenFmOn
    (SoGLRenderAction* )
{

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris+numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*colorFunc)(colorPtr); colorPtr+= colorStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriFmOnT
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadFmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenFmOnT
    (SoGLRenderAction* )
{

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris+numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*colorFunc)(colorPtr); colorPtr+= colorStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriFmFn
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadFmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenFmFn
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris+numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(numTris+numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*colorFunc)(colorPtr); colorPtr+= colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriFmFnT
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadFmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenFmFnT
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris+numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(numTris+numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*colorFunc)(colorPtr); colorPtr+= colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriFmVn
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadFmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue()+3*numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenFmVn
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris+numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = 
	vpCache.getNormals(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*colorFunc)(colorPtr); colorPtr+= colorStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriFmVnT
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadFmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue()+3*numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenFmVnT
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(numTris+numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = 
	vpCache.getNormals(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*colorFunc)(colorPtr); colorPtr+= colorStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}



void
SoFaceSet::
TriVmOn
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadVmOn
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue()+3*numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenVmOn
    (SoGLRenderAction* )
{

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = 
	vpCache.getColors(startIndex.getValue()+ 3*numTris + 4*numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriVmOnT
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadVmOnT
    (SoGLRenderAction* ) {

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue()+3*numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenVmOnT
    (SoGLRenderAction* )
{

    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = 
	vpCache.getColors(startIndex.getValue()+ 3*numTris + 4*numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriVmFn
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadVmFn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue()+3*numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenVmFn
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = 
	vpCache.getColors(startIndex.getValue()+ 3*numTris + 4*numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(numTris+numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriVmFnT
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadVmFnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue()+3*numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenVmFnT
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = 
	vpCache.getColors(startIndex.getValue()+ 3*numTris + 4*numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(numTris+numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriVmVn
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

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadVmVn
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue()+3*numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue()+3*numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenVmVn
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = 
	vpCache.getColors(startIndex.getValue()+ 3*numTris + 4*numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = 
	vpCache.getNormals(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


void
SoFaceSet::
TriVmVnT
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
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue());
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(sendAdjacency.getValue()?GL_TRIANGLES_ADJACENCY:GL_TRIANGLES);

    for (int tri = 0; tri < numTris; tri++) {

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
QuadVmVnT
    (SoGLRenderAction* ) {

    const char *vertexPtr = vpCache.getVertices(startIndex.getValue()+3*numTris);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(startIndex.getValue()+3*numTris);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(startIndex.getValue()+3*numTris);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(startIndex.getValue()+3*numTris);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;

    glBegin(GL_QUADS);

    for (int quad = 0; quad < numQuads; quad++) {
	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

	(*colorFunc)(colorPtr); colorPtr += colorStride;
	(*normalFunc)(normalPtr); normalPtr += normalStride;
	(*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	(*vertexFunc)(vertexPtr); vertexPtr += vertexStride;

    }
    glEnd();
}

void
SoFaceSet::
GenVmVnT
    (SoGLRenderAction* )
{

    const char *vertexPtr = 
	vpCache.getVertices(startIndex.getValue()+3*numTris+4*numQuads);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = 
	vpCache.getColors(startIndex.getValue()+ 3*numTris + 4*numQuads);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = 
	vpCache.getNormals(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = 
	vpCache.getTexCoords(startIndex.getValue() + 3*numTris + 4*numQuads);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int nf = numVertices.getNum();
    for(int faceCtr = numQuads + numTris; faceCtr < nf; faceCtr++) {
	glBegin(GL_POLYGON);

	const int nv = numVertices[faceCtr];
	for(int i=0; i<nv; i++) {
	    (*colorFunc)(colorPtr); colorPtr += colorStride;
	    (*normalFunc)(normalPtr); normalPtr += normalStride;
	    (*texCoordFunc)(texCoordPtr); texCoordPtr += texCoordStride;
	    (*vertexFunc)(vertexPtr); vertexPtr += vertexStride;
	}

	glEnd();
    }
}


