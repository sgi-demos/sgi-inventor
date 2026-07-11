# SGI Open Inventor — SDL2 / OpenGL ES 2 / WebAssembly port

Genuine 1990s SGI Inventor demos and games, running natively on modern
machines and in any web browser — built from the actual SGI source
lineage, not a reimplementation. Part of the
[sgi-demos](https://github.com/sgi-demos) historical preservation project.

Ported so far (all run native and in-browser):

| Demo | Origin |
|---|---|
| **maze** | SGI demo, 1994 — tilt the maze, roll the marble |
| **slotcar** ("SlotCars") | Bell/Immel, SGI 1994 — SoText3 title, textures, robot cars, engine sound |
| **drop** | SGI demo — falling-blocks game in 3D |
| **hohoho** | Inventor Games CD — holiday card with swirling snow |
| **puck** | Inventor Games CD — 3D air hockey |
| **spacecadet** | Inventor Games CD — space flight sim |
| **linkatron** | Inventor Games CD — linkage building toy |
| **pbn** (pbnsolve + pbncreate) | Inventor Games CD — Paint By Numbers puzzles + editor |
| **revo** | SGI demo — surface-of-revolution editor (draw a profile, spin it) |
| **qmorf** | Gavin Bell, SGI — 3D morphing between quad-mesh CyberHeads |
| **noodle** | Paul Isaacs, SGI 1994 — generalized-cylinder editor (5 windows) |
| **textomatic** | SGI demo — 3D text editor with extrusion-bevel editing |

The scene-graph core is kept byte-identical where possible; the Xt/Motif
windowing layer is replaced by a small SDL2 layer (`libSoSDL`), and all of
Inventor's OpenGL 1.x rendering goes through
[gl4es](https://github.com/sgi-demos/gl4es) to OpenGL ES 2 — WebGL in the
browser on **stock, unpatched emscripten**. See
[ARCHITECTURE.md](ARCHITECTURE.md) for the full design and the war
stories; the upstream open-inventor README is preserved as
[README.upstream.md](README.upstream.md).

## Prerequisites (macOS)

```sh
brew install cmake sdl2 jpeg-turbo freetype pkg-config
```

Xcode Command Line Tools provide the compiler, OpenGL framework, bison,
and iconv.

## Native macOS build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
```

Run the demos (each opens an SDL window):

```sh
build/apps/sdldemos/maze/maze
build/apps/sdldemos/slotcar/slotcar
build/apps/sdldemos/drop/drop            # ... and so on for each game
```

Also built: `build/apps/sdltests/{ivread,ivcone,ivview,ivinteract}` —
smoke tests (`ivview <file.iv>` is a handy generic viewer, e.g. against
`data/models/*.iv`).

Debug aid: `IV_DUMP_DIR=/some/dir build/apps/sdldemos/maze/maze` writes
each rendered frame as a PPM — ground truth for headless testing.

## Browser (WebAssembly) build

The web build uses stock emscripten with the gl4es backend (no emscripten
patches). Two one-time setup steps:

**1. Native build first.** The wasm build runs Inventor's `ppp` code
generator on the host, and expects it at `build/tools/ppp/ppp` (already
there if you did the native build above; override with `-DPPP_COMMAND=`).

**2. gl4es + glues.** Sibling checkouts of
[gl4es](https://github.com/sgi-demos/gl4es) (run its `build-all.sh` to
produce `lib/libGL-{native,web}.a`) and
[glues](https://github.com/sgi-demos/glues) (the script below looks for
it at `../glues` or `~/Github/glues`, cloning one otherwise):

```sh
tools/build-glues-gl4es.sh            # full glues for gl4es, native + web
```

Then configure and build:

```sh
emcmake cmake -S . -B build-em-gl4es -DCMAKE_BUILD_TYPE=Release -DIV_GL_BACKEND=gl4es
cmake --build build-em-gl4es -j8
```

Serve and play (wasm requires HTTP, not file://):

```sh
python3 -m http.server 8000
# http://localhost:8000/build-em-gl4es/apps/sdldemos/maze/maze.html
# http://localhost:8000/build-em-gl4es/apps/sdldemos/slotcar/slotcar.html
# ... one .html per game
```

## The gl4es backend, natively

`-DIV_GL_BACKEND=gl4es` also works for the **native** build, routing GL1
through gl4es to ANGLE (GLES-on-Metal) — the same GL path the browser
uses, so web rendering bugs reproduce natively where they are debuggable.
Verified pixel-identical against the desktop-GL backend across all demos.
Requires [opengl-for-mac](https://github.com/erik-larsen/opengl-for-mac)
(ANGLE headers + dylibs) as a sibling checkout:

```sh
cmake -S . -B build-gl4es-native -DCMAKE_BUILD_TYPE=Release -DIV_GL_BACKEND=gl4es
cmake --build build-gl4es-native -j8
```

Paths default to `../../gl4es` and `../../opengl-for-mac` relative to
this directory; override with `-DGL4ES_DIR=` / `-DGLES_DIR=`.

## Repository layout

```
lib/                     Core libInventor — upstream SGI code, minimal patches
libFL/  libimage/        Font + SGI .rgb image libraries (upstream)
libSoSDL/                The SDL2 windowing/viewer layer (replaces SoXt)
apps/sdldemos/           The ported demos and games
apps/sdltests/           Native smoke tests
tools/ppp/               Inventor's code generator (host tool)
tools/glues-patches/     Notes on glues fixes (now upstreamed to sgi-demos/glues)
tools/build-glues-gl4es.sh  Builds libglues for the native + web links
ARCHITECTURE.md          Full port writeup: design, failures, lessons
```

Linux and Windows native builds, and CI, are planned but not yet wired up.

## License

The SGI Open Inventor sources retain their original license (see
[COPYING](COPYING)). Port additions follow the same terms.
