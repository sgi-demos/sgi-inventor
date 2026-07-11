# glues patches — upstreamed

The patches that used to live here are now folded into the canonical
glues fork (https://github.com/sgi-demos/glues), which all the sgi-demos
ports share; the build script
(`tools/build-glues-gl4es.sh`) compiles straight from that checkout.

What was upstreamed (see this directory's git history for the patches):

- **libtess GLdouble internals** (`libtess-gldouble.patch`): reverts
  `source/libtess/` to SGI's original GLdouble types, matching the
  GLdouble public tess API the fork already declared. The float-converted
  internals both broke the standard GLU ABI (callers pass GLdouble*) and
  lost the precision libtess' sweep algorithm needs (SoText3 glyph
  outlines hit the CheckForIntersect assertion). Tess math is CPU-side;
  GLES never sees these values.

- **gl4es hardware mipmaps** (`hw-mipmap-gl4es.patch`, from the
  rss-sdl2-gles2 port): gated by `-DGLUES_USE_HW_MIPMAP`, defers
  power-of-two `gluBuild2DMipmaps` to gl4es' `GL_GENERATE_MIPMAP` —
  CPU-built chains sample black under gl4es `*_MIPMAP_*` min filters.

Also reconciled upstream: the `GLUES_GL4ES` branch of `glues.h`
self-includes gl4es's `GL/glu_mangle.h` on Apple/Emscripten, so glues'
glu* definitions get the same mangled names (mglu*) that consumers
compiled against gl4es's `<GL/glu.h>` reference — no build-system
`-include` needed.
