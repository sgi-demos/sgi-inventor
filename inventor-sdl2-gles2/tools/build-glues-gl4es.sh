#!/bin/sh
# Build FULL glues (core + libtess + libnurbs) against gl4es, for the
# gl4es backend of the Inventor demos — native (mac/ANGLE) and web.
#
# Usage: tools/build-glues-gl4es.sh [native|web|all] [glues-checkout] [gl4es-checkout]
#
# Differences from tools/build-glues-em.sh (the glemu-backend script):
#   - compiles against gl4es' <GL/gl.h>/<GL/glu.h> (-DGLUES_GL4ES), so
#     glues' internal GL calls resolve to gl4es (gl4es_gl* on Apple/web);
#   - force-includes GL/glu_mangle.h so the exported glu* definitions get
#     the same mglu* names consumers compiled with -DUSE_MGL_NAMESPACE see;
#   - builds a native variant too (desktop GLU disappears with ANGLE).
#
# The glues checkout is never modified: sources are copied and the
# libtess GLdouble patch (tools/glues-patches/) is applied to the copy.
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
[ -n "$GLUES_SRC" ] || { echo "ERROR: glues checkout not found"; exit 1; }
[ -n "$GL4ES" ]     || { echo "ERROR: gl4es checkout not found"; exit 1; }
echo "glues: $GLUES_SRC"
echo "gl4es: $GL4ES"

OUT="$REPO/build-gl4es/glues"
WORK="$OUT/src"
rm -rf "$WORK"
mkdir -p "$WORK"
cp -R "$GLUES_SRC/source" "$WORK/"
(cd "$WORK" && patch -p1 -s < "$REPO/tools/glues-patches/libtess-gldouble.patch")

# -DGLUES_GL4ES: glues.h includes gl4es' <GL/gl.h> (mangles gl*->gl4es_gl*
#   on Apple/Emscripten).
# -include GL/glu_mangle.h: exported glu* definitions become mglu*.
# gnu++14 for the 1990s libnurbs sources ('register').
COMMON="-O2 -DNDEBUG -DLIBRARYBUILD -DGLUES_GL4ES -I$GL4ES/include -include GL/glu_mangle.h -I$WORK/source"
NURBS_INCS="-I$WORK/source/libnurbs/interface -I$WORK/source/libnurbs/internals -I$WORK/source/libnurbs/nurbtess"

build_variant() {  # $1 = native|web
    if [ "$1" = web ]; then CC="emcc"; CXX="em++"; AR="emar"; else CC=cc; CXX=c++; AR=ar; fi
    OBJ="$OUT/obj-$1"
    rm -rf "$OBJ"
    mkdir -p "$OBJ"
    for f in "$WORK"/source/*.c "$WORK"/source/libtess/*.c; do
        echo "  [$1] CC  $(basename "$f")"
        $CC $COMMON -c "$f" -o "$OBJ/$(basename "$f" .c).o"
    done
    for f in "$WORK"/source/libnurbs/interface/*.cc \
             "$WORK"/source/libnurbs/internals/*.cc \
             "$WORK"/source/libnurbs/nurbtess/*.cc; do
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
