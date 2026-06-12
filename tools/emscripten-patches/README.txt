Patch for Emscripten's src/library_glemu.js (LEGACY_GL_EMULATION),
created against emscripten 3.1.6 (Ubuntu 24.04 apt package).

Why (two independent causes of the "grayscale maze"):

1. PRIMARY: SoGLLazyElement::init calls glGetBooleanv(GL_RGBA_MODE) to
   detect color-index mode. WebGL rejects that enum (INVALID_ENUM) and
   the result stays 0, so Inventor believes the framebuffer is in COLOR
   INDEX mode and never sends RGBA materials at all - everything keeps
   the emulation's default 0.8 gray diffuse.

2. SoGLLazyElement sends glMaterialfv(GL_EMISSION) and
   GL_AMBIENT_AND_DIFFUSE; stock glemu THROWS 'TODO' on these. With C++
   unwinding disabled (-fignore-exceptions), the throw aborts the frame
   mid-material-send and desyncs Inventor's lazy GL state.

Related: SoGLLightIdElement queries glGetIntegerv(GL_MAX_LIGHTS), also
not a WebGL enum -> 0 lights -> headlight never enabled -> black screen.

The patch:
  - glGetBooleanv hook: answer GL_RGBA_MODE/GL_DOUBLEBUFFER (true),
    GL_STEREO/GL_INDEX_MODE (false)
  - glGetIntegerv hook: answer GL_MAX_LIGHTS (8), GL_DRAW_BUFFER (GL_BACK)
  - glMaterialfv: handle GL_EMISSION and GL_AMBIENT_AND_DIFFUSE
  - add scalar glMaterialf (GL_SHININESS)
  - glLightModeli aliased to glLightModelf (two-sided lighting)
  - demote all remaining FFP TODO throws to warnOnce

Apply (path depends on your emscripten install):
  patch <emscripten>/src/library_glemu.js library_glemu-3.1.6-inventor.patch

Companion change: gl1stubs.c no longer stubs glMaterialf/glLightModeli,
so the JS implementations above are actually linked.
