/*
 * GL 1.x entry points absent from Emscripten's LEGACY_GL_EMULATION.
 * All are no-ops: the Inventor SDL port disables render caching
 * (display lists) at runtime and does not exercise color-index mode,
 * the accumulation buffer, stipples, raster text, or the attrib stack
 * in the browser build. Revisit per-function if a demo needs one. 
 * (glMaterialf and glLightModeli are intentionally NOT stubbed here:
 * they are implemented in the patched library_glemu.js - see
 * tools/emscripten-patches/.)
 */

void glAccum(unsigned op, float value) {}
void glClearIndex(float c) {}
void glColorMaterial(unsigned face, unsigned mode) {}
void glIndexi(int c) {}
void glIndexiv(const int *c) {}
void glLightf(unsigned light, unsigned pname, float param) {}
void glLineStipple(int factor, unsigned short pattern) {}
void glPolygonStipple(const unsigned char *mask) {}
void glPopAttrib(void) {}
void glPushAttrib(unsigned mask) {}
void glRects(short x1, short y1, short x2, short y2) {}
void glTexCoord4fv(const float *v) {}
void glTexGenf(unsigned coord, unsigned pname, float param) {}
