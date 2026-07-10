# SGI Open Inventor — SDL2 / OpenGL ES 2 / WebAssembly port

Genuine 1990s SGI Inventor demos, running natively on modern machines and
in any web browser — built from the actual SGI source lineage, not a
reimplementation. Part of the [sgi-demos](https://github.com/sgi-demos)
historical preservation project.

Two complete SGI demos are ported so far:

| Demo | Origin | Native | Browser |
|---|---|---|---|
| **maze** | SGI demo, 1994 | ✅ | ✅ |
| **slotcar** ("SlotCars") | Bell/Immel, SGI 1994 — with SoText3 title, textures, robot cars, engine sound | ✅ | ✅ |

The scene-graph core is kept byte-identical where possible; the Xt/Motif
windowing layer is replaced by a small SDL2 layer (`libSoSDL`), and
Emscripten's `LEGACY_GL_EMULATION` is patched to handle the GL1 idioms
Inventor actually uses. See [ARCHITECTURE.md](ARCHITECTURE.md) for the full
design and the war stories; the upstream open-inventor README is preserved
as [README.upstream.md](README.upstream.md).

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
build/apps/sdldemos/maze/maze          # tilt the maze, roll the marble
build/apps/sdldemos/slotcar/slotcar    # click Start, race
```

maze: drag tilts the board. slotcar: mouse up/down = speed, left/right
click = change lanes, arrow keys = camera, stop at the start/finish line
for a new race.

Also built: `build/apps/sdltests/{ivread,ivcone,ivview,ivinteract}` —
smoke tests (`ivview <file.iv>` is a handy generic viewer, e.g. against
`data/models/*.iv`).

Debug aid: `IV_DUMP_DIR=/some/dir build/apps/sdldemos/maze/maze` writes
each rendered frame as a PPM — ground truth for headless testing.

## Browser (WebAssembly) build

Three one-time setup steps, then a normal `emcmake` build.

**1. Emscripten with the GL-emulation patch.** The build needs an
[emscripten](https://github.com/emscripten-core/emscripten) source
checkout (tested with 4.0.12) with our `LEGACY_GL_EMULATION` patch
applied — stock emulation renders Inventor gray/black/jumbled (see
[tools/emscripten-patches/README.txt](tools/emscripten-patches/README.txt)):

```sh
cd <your-emscripten-checkout>
patch -p1 < <this-repo>/tools/emscripten-patches/library_glemu-4.0.12-inventor.patch
```

**2. Native build first.** The wasm build runs Inventor's `ppp` code
generator on the host, and expects it at `build/tools/ppp/ppp` (already
there if you did the native build above; override with `-DPPP_COMMAND=`).

**3. GLU for GLES (glues).** SoText3/NURBS need the SGI GLU tessellator.
One script builds it for wasm (it looks for a
[glues](https://github.com/sgi-demos/glues) checkout at `../glues` or
`~/Github/glues`, cloning one otherwise):

```sh
tools/build-glues-em.sh          # -> build-em/glues/libglues.a
```

Then configure and build:

```sh
emcmake cmake -S . -B build-em -DCMAKE_BUILD_TYPE=Release
cmake --build build-em -j8
```

Serve and play (wasm requires HTTP, not file://):

```sh
python3 -m http.server 8000
# http://localhost:8000/build-em/apps/sdldemos/maze/maze.html
# http://localhost:8000/build-em/apps/sdldemos/slotcar/slotcar.html
```

The only expected console output is Emscripten's two boilerplate
"using emscripten GL emulation" warnings.

Note: emscripten's system JS libraries are not in the build dependency
graph — after re-applying or changing the glemu patch, delete
`build-em/apps` (or the whole `build-em` except `glues/`) to force a
relink.

## Repository layout

```
lib/                     Core libInventor — upstream SGI code, minimal patches
libFL/  libimage/        Font + SGI .rgb image libraries (upstream)
libSoSDL/                The SDL2 windowing/viewer layer (replaces SoXt)
apps/sdldemos/           The ported demos: maze, slotcar
apps/sdltests/           Native smoke tests
tools/ppp/               Inventor's code generator (host tool)
tools/emscripten-patches/  LEGACY_GL_EMULATION patch + root-cause notes
tools/glues-patches/     libtess GLdouble patch for glues
tools/build-glues-em.sh  Builds libglues.a for the wasm link
tools/harness/           Headless Node mock-WebGL test harness
ARCHITECTURE.md          Full port writeup: design, failures, lessons
```

Linux and Windows native builds, and CI, are planned but not yet wired up.

## License

The SGI Open Inventor sources retain their original license (see
[COPYING](COPYING)). Port additions follow the same terms.
