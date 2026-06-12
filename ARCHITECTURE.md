# SGI Open Inventor — SDL2 / OpenGL-ES2 / WebAssembly Port

Part of the [sgi-demos](https://github.com/sgi-demos) historical preservation
project: getting genuine SGI-era Inventor demos running natively on modern
machines and in any web browser, with minimal changes to the original code.

This document describes the architecture of the port, the major pieces of
work, and the hard-won lessons encoded in the tree.

---

## 1. Goal and approach

SGI Inventor (1992-) is a retained-mode scene graph built on IRIS GL / OpenGL
1.x fixed-function, with an Xt/Motif GUI layer. The goal is to run real SGI
demos (maze, and eventually slotcars-class content) on:

- **Native desktop** (Linux/macOS/Windows) via SDL2
- **The browser** via Emscripten, compiled to WebAssembly

The base is **aumuell/open-inventor** — the actual SGI source lineage, not a
reimplementation (Coin3D was considered and rejected: for preservation, the
SGI code itself is the artifact). Strategy in one line: *keep the scene-graph
core byte-identical where possible, replace the windowing layer, and teach
the GL emulation layer the GL1 idioms Inventor actually uses.*

## 2. Repository layout (what's ours vs. upstream)

```
CMakeLists.txt              Slim sdl-port build (upstream preserved as
CMakeLists.upstream.txt     CMakeLists.upstream.txt)
lib/                        Core libInventor — upstream, minimal patches
libFL/                      Font library (FreeType-based; no X11)
libimage/                   SGI .rgb image library — upstream
tools/ppp/                  Inventor's code generator (host tool)
libSoSDL/                   NEW: the SDL windowing/viewer layer
apps/sdltests/              NEW: smoke tests (ivread, ivcone, ivview,
                            ivinteract)
apps/sdldemos/maze/         Ported SGI maze game (from apps/demos/maze)
apps/samples/common/        Upstream sample helpers (maze needs
                            InventorLogo.h from here — keep it!)
tools/emscripten-patches/   Patch for Emscripten's LEGACY_GL_EMULATION
tools/harness/              Headless Node mock-WebGL test harness
```

What we deliberately do **not** build: `libSoXt` / `libInventorXt` (the
Xt/Motif viewers and editors), GLw, man pages, the converters. The cut line
is favorable: core libInventor has no Xt dependency; only the component
library does.

## 3. The slim build (M1)

The top-level `CMakeLists.txt` builds only: `tools/ppp` (a host-side code
generator that must run at build time), `libimage`, `libFL`, core
`libInventor` as a static lib, `libSoSDL`, and the SDL tests/demos. Static
libs keep the eventual Emscripten link simple.

Core libInventor compiled with **zero source changes** at M1 — the SGI code
is healthy. All subsequent core patches are small and surgical (see §8).

## 4. Severing X11 (M2)

Inventor's only structural X11 dependency in the core is
`SoWindowElement`, which carries the window/context handles through the
traversal state. The port replaces `Window`/`GLXContext`/`Display*` with
forward-declared `SDL_Window*`/`SDL_GLContext` so the header does not force
SDL.h on all of Inventor. A GIF-reader XColor shim and an offscreen-GLX stub
cover the few remaining X11 touches.

## 5. libSoSDL — the windowing/viewer layer (M2–M4)

Three classes replace what SoXt provided, ~1k lines total:

- **`SoSDL`** — init + main loop. `SoSDL::init()` performs SDL_Init,
  SoDB/SoNodeKit/SoInteraction init. The loop waits on SDL events with a
  timeout derived from Inventor's pending timer sensors, then services the
  timer and delay (idle) sensor queues — the behavior of SoXt's `select()`
  loop. `doOneIteration()` exists specifically so Emscripten's
  `emscripten_set_main_loop` can drive it (browser paces the loop; the event
  wait never blocks there).
- **`SoSDLRenderArea`** — SDL window + GL context ownership, SDL event →
  SoEvent translation (mouse, keyboard), SoHandleEventAction dispatch,
  SoSceneManager-driven rendering. Also implements `IV_DUMP_DIR` frame
  dumping (PPM per frame) — our ground-truth tool for headless testing,
  added after discovering `xwd` silently captures stale black for GL windows
  under Xvfb.
- **`SoSDLExaminerViewer`** — camera manipulation math ported directly from
  `SoXtExamVwr.c++`: sphere-sheet trackball spin, focal-plane pan, focal
  dolly, seek, viewAll. Left drag spins, middle/shift-left pans, ctrl-left
  or wheel dollies, ESC toggles viewing/picking.

### The "viewer responsibilities" list

SoXt did several things behind the application's back. Any new viewer (and
any future demo port) must reclaim these, or scenes break in confusing ways:

1. `glEnable(GL_DEPTH_TEST)` after context creation (M2 — its absence
   masqueraded as a lighting bug for most of a milestone).
2. Graft the headlight **after** a scene-provided camera (M4).
3. `adjustCameraClippingPlanes` from the scene bounding box before each
   render (M4 — without it the maze was entirely far-plane clipped).

## 6. The maze demo port (M4)

`apps/sdldemos/maze` is the 1994 SGI maze game with ~10 changed lines:
SoSDL init, `SoSDLExaminerViewer` instead of `SoXtExaminerViewer`, the main
loop, `NO_AUDIO`, and the overlay-plane Inventor logo removed (no overlay
planes anywhere modern). Scene data (`box.iv`) is embedded as a C array, so
the browser build needs no preloaded filesystem. The zero-arg `main()` under
`__EMSCRIPTEN__` works around Ubuntu emcc 3.1.6 + clang 15 disagreeing on
the `__main_argc_argv` symbol name (two-arg main silently strips the whole
program).

## 7. Emscripten build (M5)

`emcmake cmake` cross-build with these arrangements (top-level
CMakeLists.txt, `if (EMSCRIPTEN)` blocks):

- **Ports at compile time**: `-sUSE_SDL=2 -sUSE_FREETYPE=1` must be in
  *compile* options, not just link options, or `SDL.h`/`ft2build.h` are not
  found. (On Debian/Ubuntu's emscripten, the system cache is frozen — copy
  it, point `EM_CONFIG` at a config with `FROZEN_CACHE = False`, and
  `embuilder build freetype sdl2`.)
- **Host codegen**: `PPP_COMMAND` points at a natively built `ppp` from a
  native build directory (codegen cannot run under wasm).
- **JPEG**: `IV_NO_JPEG` guards SoTexture2's JPEG reader; GIF and SGI .rgb
  texture reading still work.
- **GLU**: linked from a wasm build of **glues** (the GLU-ES port — the
  original SGI tessellator/NURBS/mipmap code, which matters for behavioral
  fidelity). The C sources need `-D__USE_SDL_GLES__`; libnurbs is C++
  (`.cc`) and needs `-DLIBRARYBUILD`. Archive everything into
  `libglues.a`.
- **Render caching disabled at runtime**: `SoSDL::init()` sets
  `IV_SEPARATOR_MAX_CACHES=0` before `SoDB::init()` — display lists do not
  exist under the GL emulation.
- **gl1stubs.c**: no-op C stubs for GL1 entry points absent from the
  emulation (display lists, accum, stipple, raster ops, attrib stack).
  IMPORTANT: a C stub *shadows* the JS emulation's implementation at link
  time — never stub a function the emulation implements (this bit us with
  `glMaterialf` and `glLightModeli`).
- Link: `-sLEGACY_GL_EMULATION=1 -sGL_UNSAFE_OPTS=0 -sALLOW_MEMORY_GROWTH=1
  -sEXIT_RUNTIME=0`.
- Build-system gotcha: Emscripten's system JS libraries are **not** in
  make's dependency graph. After patching `library_glemu.js`, delete the
  output html/js/wasm to force a relink.

## 8. The GL emulation patch (M5b–M5d) — the heart of the browser work

Emscripten's `LEGACY_GL_EMULATION` is explicitly "a collection of limited
workarounds." Inventor exercises GL1 idioms it does not handle, producing
three independent, compounding failures. The fix lives in
`tools/emscripten-patches/library_glemu-3.1.6-inventor.patch` (created
against emscripten 3.1.6; apply to `<emscripten>/src/library_glemu.js`).

**Failure 1 — color-index gray (the original "grayscale maze").**
`SoGLLazyElement::init()` calls `glGetBooleanv(GL_RGBA_MODE)` to detect
color-index framebuffers. WebGL rejects the enum (INVALID_ENUM), the result
reads back 0, and Inventor concludes the framebuffer is color-index — so it
never sends RGBA materials at all, leaving the emulation's default 0.8 gray
diffuse everywhere. *Patch: the `glGetBooleanv` hook answers GL_RGBA_MODE
and GL_DOUBLEBUFFER true, GL_STEREO and GL_INDEX_MODE false.*

**Failure 2 — material throws poisoning lazy state.**
Stock glemu's `glMaterialfv` throws `'TODO'` on GL_EMISSION and
GL_AMBIENT_AND_DIFFUSE — both sent by SoGLLazyElement. With C++ unwinding
disabled in wasm builds (`-fignore-exceptions`), the throw rips through the
frame mid-material-send while Inventor's lazy bookkeeping believes the
state was sent; subsequent frames skip the sends entirely. *Patch:
implement EMISSION and AMBIENT_AND_DIFFUSE, add scalar `glMaterialf`
(SHININESS), alias `glLightModeli` to `glLightModelf` (two-sided lighting),
and demote every remaining fixed-function TODO throw to `warnOnce` — a
single unsupported pname must never be able to poison a frame.*

**Failure 3 — black screen (zero lights).**
`SoGLLightIdElement` queries `glGetIntegerv(GL_MAX_LIGHTS)` — also not a
WebGL enum — reads 0, concludes no lights exist, and never enables the
headlight. GL_LIGHTING on with zero lights renders black. *Patch: the
`glGetIntegerv` hook answers GL_MAX_LIGHTS (8) and GL_DRAW_BUFFER
(GL_BACK).*

**Failure 4 — triangle jumble (corrupted vertex stream).**
Stock glemu immediate mode writes normal/color/texcoord records into the
interleaved vertex stream *as the calls arrive*. Inventor sets normals per
FACE — one `glNormal3fv`, then several `glVertex3fv` — so records land
between vertices, the per-vertex layout and stride become inconsistent, and
geometry shreds into a jumble. *Patch: restore classic GL semantics.
`glNormal*` / `glColor*` / `glTexCoord*` only update current state (and are
legal outside glBegin/glEnd, which Inventor relies on); every `glVertex*`
emits a full record — position, plus a normal slot reserved for the whole
batch whenever lighting is enabled (`batchNeedNormal`), plus color/texcoord
per batch flags.*

**Failure 5 — GL_POLYGON throws (slotcar, M6).**
Inventor's convex SoFaceSet path issues `glBegin(GL_POLYGON)`; stock glemu
throws "unsupported immediate mode 9". A convex polygon renders
identically as a triangle fan over the same vertices. *Patch: map mode 9
to GL_TRIANGLE_FAN in flush().* (Found in seconds by the Node harness —
the throw surfaced with a full stack instead of a black canvas.)

**Failure 6 — SoText2 invisible (no raster ops).**
SoText2 draws glyphs with `glRasterPos3f` + `glBitmap` wrapped in
bitmap-only display lists replayed via `glCallLists(GL_2_BYTES, ...)` —
none of which exists in WebGL (they were no-op C stubs, so text silently
vanished). *Patch: a bitmap-text emulation (`$GLBitmapEmu`): the raster
position is transformed through the emulated modelview/projection to
window coordinates; each 1-bpp glyph (4-byte-aligned rows, MSB-first,
bottom-up — libFL's format) becomes an ALPHA texture drawn as a
screen-space quad with a dedicated discard-shader; `glGenLists` /
`glNewList` / `glCallList(s)` / `glListBase` are implemented for
bitmap-recording lists.* Companion fix outside the emulation: the
Emscripten FreeType 2.6.0 port maps almost no glyphs through a Type1
unicode charmap (native FreeType 2.13 is fine), so the bundled fonts are
TrueType (DejaVu Serif standing in for Utopia; see
`apps/sdldemos/slotcar/data/fonts/README.txt`).

**Failure 7 — every glyph rendered as the .notdef tofu box (iconv
endianness, not GL at all).** After the bitmap-text emulation worked,
browser text showed identical boxes for every character. A long
instrumented hunt (GPU texture readback, full draw-state dumps,
per-quad logs — all clean) ended at libFL's TRACE output: characters
arrived byte-swapped (0x5300 for 'S'). Root cause: SGI's
SoText2/SoText3 swap iconv output to big-endian for libFL with
`DGL_HTON_SHORT` — necessary on glibc, whose `"UCS-2"` emits
little-endian — but **musl's `"UCS-2"` is big-endian**, so under
Emscripten the swap corrupted correct data. Apple's libiconv is also
BE, hence the original `#ifndef __APPLE__`. *Fix: request `"UCS-2BE"`
explicitly and delete the swap; every libc honors the explicit name.*
The diagnostic ladder that found it (r3–r6) is worth keeping in mind:
console version banners, GL-error probes, FBO texture readback,
framebuffer readback after one draw, and finally enabling the original
1990s TRACE logging — which is what actually named the bug.

**Failure 8 — SoText3 tessellation crashed (GLU ABI mismatch).** With
real characters finally flowing, SoText3 fed glyph outlines to glues'
libtess and crashed in `CheckForIntersect`. glues' GLES port had
converted libtess to GLfloat, but Inventor compiles against the
standard sysroot `GL/glu.h` where `gluTessVertex` takes `GLdouble*` —
double[3] written, float[3] read: garbage coordinates, and float
precision is below what the sweep algorithm needs anyway. *Fix:
revert libtess to SGI's original GLdouble types (vendored in
`tools/glues-patches/`); tess math is CPU-side and GLES never sees
it.*

The general lesson: **GL1 state queries that WebGL rejects fail soft** —
INVALID_ENUM plus an untouched (zero) result — and fixed-function code makes
load-bearing decisions on those zeros. Every glGet* the scene graph performs
must be audited against the emulation.

## 9. The headless harness (`tools/harness/`)

A Node.js harness that loads the Emscripten `maze.js` against a recording
mock WebGL context — no browser needed. It captures draw calls, uniform
uploads (e.g. `u_materialDiffuse` values), GL1 entry-point taps, and
unknown-enum `getParameter` queries. Used to diagnose every emulation bug
above with a seconds-long edit/run loop.

Mechanics worth knowing (all learned the hard way):

- Emscripten 3.1.6's hoisted `var Module` shadows globals — the harness
  patches the loader line to read `globalThis.HARNESS_MODULE`
  (`patch-scratch.py`).
- Delete `globalThis.fetch` so the loader takes the fs path on modern Node;
  no-op `setWindowTitle` (undefined in the Node branch).
- The mock must expose **real WebGL enum values** (`FLOAT` = 0x1406 etc.) —
  the emulation does arithmetic on them (`byteSizeByType[type - root]`).
- The mock's `getParameter` must **mimic real WebGL**: warn + return null
  for unknown enums. An early version returned a truthy default for
  everything, which masked the GL_RGBA_MODE and GL_MAX_LIGHTS bugs
  completely. A harness that is more permissive than the browser verifies
  nothing.
- The mock GL must pass `instanceof WebGLRenderingContext` (Emscripten's
  Safari workaround checks it).
- The mock must define the enum constants as context properties
  (`GLctx.VIEWPORT` etc.) and answer the state queries the emulation
  performs (VIEWPORT, ARRAY_BUFFER_BINDING, ACTIVE_TEXTURE,
  TEXTURE_BINDING_2D, UNPACK_ALIGNMENT, getVertexAttrib) — the
  bitmap-text emulation saves/restores real GL state around glyph quads.

The harness validates state and uniform flow, not pixels — final visual
verification is always a real browser.

## 10. Milestone history

| Milestone | Deliverable |
|---|---|
| M0 | Repo survey; keep/drop/stub map |
| M1 | Slim CMake build; core libInventor compiles untouched; `ivread` round-trips .iv files |
| M2 | X11 severed from core; SDL2 window + SoSceneManager; the classic red cone (lesson: depth test) |
| M3 | libSoSDL complete: sensor-serviced main loop, SoEvent translation, ExaminerViewer math from SoXt |
| M4 | SGI maze game running natively (~10 lines changed); clipping-plane + headlight lessons; IV_DUMP_DIR |
| M5 | Emscripten build links and runs in browser (gray) |
| M5b | EMISSION-throw fix; vendored glemu patch; Node harness |
| M5c | GL_RGBA_MODE / GL_MAX_LIGHTS — color and lighting correct |
| M5d | Immediate-mode batch semantics recovered from archived M5 build — geometry correct. **Maze fully working in browser.** |
| M6a | SGI slotcar (1994) ported and racing natively: overlay-plane emulation in SoSDLRenderArea, viewport-restore responsibility, Sky raw-GL lighting fix, bundled Utopia fonts, 1994-C++ modernization |
| M6b | Real LongOcean.iv recovered; GL_POLYGON→TRIANGLE_FAN glemu fix (failure 5); slotcar wasm build passing the harness |
| M6c | Bitmap-text emulation (failure 6): SoText2 glyphs render in the browser; TrueType fonts bundled (FreeType 2.6 Type1 gap) |
| M6d | The tofu-box saga: UCS-2 endianness fix in SoText2/SoText3 (glibc vs musl iconv, failure 7) and double-precision glues libtess (GLU ABI mismatch, failure 8). Full text pipeline working in wasm |
| M6e/f | SoText3 repeated-character fix (geometry display lists vs the bitmap-only emulation); 1280x1024; outline glyph scale corrected (KLUDGE_FACTOR retuned to the IRIX convention) - startup screen pixel-faithful |
| M7 | Sound: SDL2 software mixer (SoundSDL.c++) replaces the IRIX dmedia stub - per-car engine loops with live pitch (variable-rate playback, linear interp), screech, crash one-shots. AIFFs converted to WAV (originals kept); verified via SDL's disk audio driver (engine fires at the Start click) |

## 11. Process notes (how not to lose work)

This port survived a near-total loss of its own history; the practices below
are now policy:

- **Every check-in exports a deliverable**: a git bundle *and/or* a plain
  source zip into a location the human downloads immediately.
- **Bundles are verified before export** (`git bundle verify`) and must
  record *complete* history. The original M5 bundle silently carried a
  shallow-clone prerequisite (so it could not be cloned) and 174 MB of
  committed build directories (`.gitignore` now prevents the latter). If a
  bundle ever fails to clone: `git bundle list-heads` for the real tip,
  `git bundle unbundle` into a fresh repo, and `git archive <tip>` extracts
  the working tree without needing ancestry.
- **Keep the artifacts you ship.** The user's archived copy of the M5
  browser build contained the only surviving copy of the immediate-mode
  patch; it was recovered from the compiled JS verbatim.
- **Knowledge lives in the tree**, not in a chat: emulation patches are
  vendored with a README of root causes; this file records the rest.

## 12. Roadmap

- **M6 follow-ups**: slotcar browser verification on real hardware;
  network play over WebRTC/WebSocket relay (the Xt socket hook is the
  only missing piece); SDL_mixer audio for the AIFF engine sounds.
- Two-sided lighting quality check (glLightModeli is wired but the FFP
  shader's treatment is approximate).
- Performance: the emulation re-creates renderer state per flush in places;
  profile once content is heavier.
- Upstreaming: demo pages on sgi-demos.github.io; consider offering the
  glemu patch upstream to emscripten (the immediate-mode and glGet fixes
  are generally useful for GL1-era ports).
