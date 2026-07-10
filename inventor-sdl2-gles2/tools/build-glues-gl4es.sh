#!/bin/sh
# Build FULL glues (core + libtess + libnurbs) against gl4es, for the
# gl4es backend of the Inventor demos — native (mac/ANGLE) and web.
#
# Usage: tools/build-glues-gl4es.sh [native|web|all] [glues-checkout] [gl4es-checkout]
#
# Compiles straight from the glues checkout: since the reconciliation,
# https://github.com/sgi-demos/glues carries the libtess GLdouble
# internals (standard GLU ABI) and the gl4es hardware-mipmap fix
# (enabled by -DGLUES_USE_HW_MIPMAP below) — no patching needed.
#
# Output: build-gl4es/glues/libglues-native.a and/or libglues-web.a
set -e

REPO=$(cd "$(dirname "$0")/.." && pwd)
WHAT=${1:-all}
GLUES_SRC=$2
GL4ES=${3:-}

if [ -z "$GLUES_SRC" ]; then
    for c in "$REPO/../glues" "$HOME/Github/glues"; do
        if [ -d "$c/source/libtess" ]; then GLUES_SRC=$c; break; fi
    done
fi
if [ -z "$GL4ES" ]; then
    for c in "$REPO/../gl4es" "$HOME/Github/gl4es"; do
        if [ -d "$c/include/GL" ]; then GL4ES=$c; break; fi
    done
fi
if [ -z "$GLUES_SRC" ]; then
    GLUES_SRC="$REPO/build-gl4es/glues/glues-checkout"
    [ -d "$GLUES_SRC" ] || git clone https://github.com/sgi-demos/glues.git "$GLUES_SRC"
fi
[ -n "$GL4ES" ] || { echo "ERROR: gl4es checkout not found"; exit 1; }
echo "glues: $GLUES_SRC"
echo "gl4es: $GL4ES"

OUT="$REPO/build-gl4es/glues"
SRC="$GLUES_SRC/source"

# -DGLUES_GL4ES: glues.h includes gl4es' <GL/gl.h> and self-mangles the
#   glu* definitions to mglu* on Apple/Emscripten (matching consumers
#   compiled with -DUSE_MGL_NAMESPACE against gl4es's <GL/glu.h>).
# -DGLUES_USE_HW_MIPMAP: POT gluBuild2DMipmaps defers to gl4es'
#   GL_GENERATE_MIPMAP (CPU chains sample black under gl4es).
# gnu++14 for the 1990s libnurbs sources ('register').
# -include GL/glu_mangle.h: some TUs (glues_mipmap.c) reach their
# definitions before glues.h's self-mangle; force-include keeps every
# glu* definition uniformly mangled (idempotent - include-guarded).
COMMON="-O2 -DNDEBUG -DLIBRARYBUILD -DGLUES_GL4ES -DGLUES_USE_HW_MIPMAP -I$GL4ES/include -include GL/glu_mangle.h -I$SRC"
NURBS_INCS="-I$SRC/libnurbs/interface -I$SRC/libnurbs/internals -I$SRC/libnurbs/nurbtess"

build_variant() {  # $1 = native|web
    if [ "$1" = web ]; then CC="emcc"; CXX="em++"; AR="emar"; else CC=cc; CXX=c++; AR=ar; fi
    OBJ="$OUT/obj-$1"
    rm -rf "$OBJ"
    mkdir -p "$OBJ"
    for f in "$SRC"/*.c "$SRC"/libtess/*.c; do
        echo "  [$1] CC  $(basename "$f")"
        $CC $COMMON -c "$f" -o "$OBJ/$(basename "$f" .c).o"
    done
    for f in "$SRC"/libnurbs/interface/*.cc \
             "$SRC"/libnurbs/internals/*.cc \
             "$SRC"/libnurbs/nurbtess/*.cc; do
        echo "  [$1] CXX $(basename "$f")"
        $CXX $COMMON -std=gnu++14 $NURBS_INCS -c "$f" -o "$OBJ/$(basename "$f" .cc).o"
    done
    $AR rcs "$OUT/libglues-$1.a" "$OBJ"/*.o
    echo "BUILT: $OUT/libglues-$1.a"
}

case "$WHAT" in
    native) build_variant native ;;
    web)    build_variant web ;;
    all)    build_variant native; build_variant web ;;
    *) echo "usage: $0 [native|web|all] [glues-dir] [gl4es-dir]"; exit 1 ;;
esac
