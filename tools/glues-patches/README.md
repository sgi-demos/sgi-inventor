# glues double-precision libtess

The Emscripten build links GLU functionality from the `glues` project
(GLU ES port). Its libtess had been mechanically converted to GLfloat
for OpenGL ES, which created two failures for this port:

1. **ABI mismatch.** Inventor compiles against the standard `GL/glu.h`
   (sysroot), where `gluTessVertex` takes `GLdouble*` (8-byte doubles).
   glues' float-converted implementation read the same bytes as
   `GLfloat[3]`, producing garbage coordinates.
2. **Precision.** libtess' sweep algorithm needs double precision;
   with floats, font-outline contours (SoText3) hit the
   `CheckForIntersect` assertion / memory corruption.

`libtess-gldouble.patch` reverts `source/libtess/` to SGI's original
GLdouble types and updates the tess prototypes in `source/glues.h`
(adding a `GLdouble` typedef the GLES headers lack). Tess math is
entirely CPU-side; OpenGL ES never sees these values.

Rebuild used (objects archived into `build-em/libglues.a`):

    emcc -O2 -DNDEBUG -DLIBRARYBUILD -D__USE_SDL_GLES__=1 -sUSE_SDL=2 \
         -Iinclude -Isource -c source/libtess/<each>.c

(`-DNDEBUG` matches release GLU; the assertions are advisory.)
