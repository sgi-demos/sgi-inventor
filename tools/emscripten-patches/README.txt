Patch for Emscripten's src/library_glemu.js (LEGACY_GL_EMULATION),
created against emscripten 3.1.6 (Ubuntu 24.04 apt package).

Why: Inventor's SoGLLazyElement sends glMaterialfv(GL_EMISSION) and
GL_AMBIENT_AND_DIFFUSE; stock glemu THROWS 'TODO' on these. With C++
unwinding disabled (-fignore-exceptions), the throw aborts the frame
mid-material-send and permanently desyncs Inventor's lazy GL state:
everything renders with the default 0.8 gray diffuse ("grayscale maze").

The patch:
  - glMaterialfv: handle GL_EMISSION and GL_AMBIENT_AND_DIFFUSE
  - add scalar glMaterialf (GL_SHININESS)
  - glLightModeli aliased to glLightModelf (two-sided lighting)
  - demote all remaining FFP TODO throws to warnOnce

Apply (path depends on your emscripten install):
  patch <emscripten>/src/library_glemu.js library_glemu-3.1.6-inventor.patch

Companion change: gl1stubs.c no longer stubs glMaterialf/glLightModeli,
so the JS implementations above are actually linked.
