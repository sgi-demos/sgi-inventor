Patches for Emscripten's LEGACY_GL_EMULATION JS library, teaching it the
GL1 idioms SGI Inventor actually uses. Two variants of the same patch:

  library_glemu-4.0.12-inventor.patch
      Current. Applies to <emscripten>/src/lib/libglemu.js in emscripten
      4.0.x (created against 4.0.12-git). Apply from the emscripten
      checkout root:
          patch -p1 < library_glemu-4.0.12-inventor.patch
      (or `git apply` it). This is the variant the top-level build
      instructions assume.

  library_glemu-3.1.6-inventor.patch
      Historical. Applies to <emscripten>/src/library_glemu.js in
      emscripten 3.1.6 (e.g. the Ubuntu 24.04 apt package), where the
      port was originally debugged:
          patch <emscripten>/src/library_glemu.js library_glemu-3.1.6-inventor.patch

What the patch fixes (each was an independent, compounding rendering
failure in the browser build — see ARCHITECTURE.md section 8 for the
full war stories):

1. GL1 state queries WebGL rejects "fail soft" (INVALID_ENUM + result
   left 0) and fixed-function code makes load-bearing decisions on the
   zeros:
   - glGetBooleanv(GL_RGBA_MODE) -> 0 makes SoGLLazyElement believe the
     framebuffer is COLOR INDEX mode, so RGBA materials are never sent:
     everything renders the emulation's default 0.8 gray.
     The hook now answers GL_RGBA_MODE/GL_DOUBLEBUFFER true,
     GL_STEREO/GL_INDEX_MODE false.
   - glGetIntegerv(GL_MAX_LIGHTS) -> 0 makes SoGLLightIdElement conclude
     no lights exist; headlight never enabled; black screen. The hook
     answers GL_MAX_LIGHTS (8) and GL_DRAW_BUFFER (GL_BACK).

2. glMaterialfv THROWS 'TODO' on GL_EMISSION and GL_AMBIENT_AND_DIFFUSE,
   both sent by SoGLLazyElement. With C++ unwinding disabled
   (-fignore-exceptions) the throw rips through the frame and desyncs
   Inventor's lazy GL state. Implemented both; added scalar glMaterialf
   (GL_SHININESS); aliased glLightModeli to glLightModelf (two-sided
   lighting); demoted every remaining fixed-function TODO throw to
   warnOnce. A single unsupported pname must never poison a frame.

3. Immediate mode wrote normal/color/texcoord records into the
   interleaved vertex stream AS THE CALLS ARRIVE. Inventor sets normals
   per FACE (one glNormal3fv, then several glVertex3fv), corrupting the
   layout and stride -> "triangle jumble" geometry. Restored classic GL
   semantics: glNormal*/glColor*/glTexCoord* only update current state
   (legal outside glBegin/glEnd, which Inventor relies on); every
   glVertex* emits a full record per the batchNeed* flags.

4. glBegin(GL_POLYGON) (unsupported) is rendered as GL_TRIANGLE_FAN —
   identical for convex polygons, which is what Inventor's convex
   SoFaceSet path emits per face (the slotcar track).

5. Bitmap text emulation ($GLBitmapEmu): SoText2 renders glyphs with
   glRasterPos3f + glBitmap wrapped in bitmap-only display lists
   replayed via glCallList(s) — none of which exists in WebGL. The
   raster position is transformed through the emulated MV/P matrices to
   window coordinates, each 1-bpp glyph becomes an RGBA texture (RGBA
   rather than ALPHA: alpha-only textures hit driver/ANGLE quirks and
   sample as solid rectangles), drawn as a screen-space quad with a
   dedicated minimal shader, GL state saved/restored around the draw.

Companion C-side change: libSoSDL/gl1stubs.c stubs GL1 entry points the
emulation does NOT implement. A C stub SHADOWS the JS implementation at
link time — never stub a function the (patched) emulation implements;
that is why glMaterialf/glLightModeli and the display-list/raster
functions are intentionally absent from gl1stubs.c.

Build-system gotcha: Emscripten's system JS libraries are not in the
build's dependency graph. After (re)applying the patch, delete the
output .html/.js/.wasm (or the whole build-em dir) to force a relink.
