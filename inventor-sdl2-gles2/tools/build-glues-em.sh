#!/bin/sh
# Build libglues.a (GLU for OpenGL ES: SGI tessellator, NURBS, mipmap) for
# the Emscripten build of the Inventor demos.
#
# Usage: tools/build-glues-em.sh [path-to-glues-checkout] [output-dir]
#
# The glues checkout is found in this order: $1, ../glues or
# ~/Github/glues next to this repo, else cloned from
# https://github.com/sgi-demos/glues. Compiles straight from the
# checkout: since the reconciliation the fork carries the libtess
# GLdouble internals (standard GLU ABI) upstream — no patching needed.
#
# Output: <output-dir>/libglues.a  (default build-em/glues/libglues.a,
# where the top-level CMakeLists expects it).
set -e

REPO=$(cd "$(dirname "$0")/.." && pwd)
OUT=${2:-"$REPO/build-em/glues"}
SRC=$1

if [ -z "$SRC" ]; then
    for c in "$REPO/../glues" "$HOME/Github/glues"; do
        if [ -d "$c/source/libtess" ]; then SRC=$c; break; fi
    done
fi
if [ -z "$SRC" ]; then
    SRC="$OUT/glues-checkout"
    [ -d "$SRC" ] || git clone https://github.com/sgi-demos/glues.git "$SRC"
fi
echo "glues source: $SRC"

WORK="$SRC"

CFLAGS="-O2 -DNDEBUG -DLIBRARYBUILD -D__USE_SDL_GLES__=1 -sUSE_SDL=2"
INCS="-I$WORK/include -I$WORK/source"
NURBS_INCS="-I$WORK/source/libnurbs/interface -I$WORK/source/libnurbs/internals -I$WORK/source/libnurbs/nurbtess"

OBJ="$OUT/obj"
rm -rf "$OBJ"
mkdir -p "$OBJ"

for f in "$WORK"/source/*.c "$WORK"/source/libtess/*.c; do
    o="$OBJ/$(basename "$f" .c).o"
    echo "  CC  $(basename "$f")"
    emcc $CFLAGS $INCS -c "$f" -o "$o"
done

for f in "$WORK"/source/libnurbs/interface/*.cc \
         "$WORK"/source/libnurbs/internals/*.cc \
         "$WORK"/source/libnurbs/nurbtess/*.cc; do
    o="$OBJ/$(basename "$f" .cc).o"
    echo "  CXX $(basename "$f")"
    # gnu++14: the 1990s SGI sources still use 'register'
    em++ $CFLAGS -std=gnu++14 $INCS $NURBS_INCS -c "$f" -o "$o"
done

emar rcs "$OUT/libglues.a" "$OBJ"/*.o
echo "BUILT: $OUT/libglues.a"
