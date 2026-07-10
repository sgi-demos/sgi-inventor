# Slimmed top-level build for the SDL2/GLES2 port of SGI Open Inventor.
# Builds only: tools/ppp (host codegen), libimage, libFL, core libInventor.
# No Xt/Motif/GLw/SoXt. X11 still linked by core for now (severed in M2).
# Original upstream build preserved in CMakeLists.upstream.txt.
cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_EXTENSIONS OFF)

project("Open Inventor SDL"
    VERSION 2.1.6.1
    LANGUAGES CXX C)

set(INVENTOR_SOVERSION 1)
include(GNUInstallDirs)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if (CMAKE_BUILD_TYPE STREQUAL "")
   set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Static libs keep the eventual Emscripten link simple.
set(BUILD_SHARED_LIBS FALSE)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-write-strings)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wno-c++11-compat-deprecated-writable-strings)
    add_compile_options(-Wno-backslash-newline-escape)
    add_compile_options(-Wno-dangling-else)
endif()

# GL backend selection:
#   desktop - native: the platform's desktop OpenGL (fixed function);
#             web: emscripten LEGACY_GL_EMULATION with the vendored patch.
#   gl4es   - GL1.x -> GLES2 translation via gl4es on BOTH targets
#             (native mac renders through ANGLE); one GL path everywhere,
#             stock (unpatched) emscripten on the web.
set(IV_GL_BACKEND "desktop" CACHE STRING "GL backend: desktop | gl4es")
set_property(CACHE IV_GL_BACKEND PROPERTY STRINGS desktop gl4es)
if (IV_GL_BACKEND STREQUAL "gl4es")
    set(IV_GL4ES TRUE)
else()
    set(IV_GL4ES FALSE)
endif()

if (IV_GL_BACKEND STREQUAL "gl4es")
    set(GL4ES_DIR "${CMAKE_SOURCE_DIR}/../../gl4es" CACHE PATH
        "gl4es checkout (include/ + lib/libGL-{native,web}.a from build-all.sh)")
    set(IV_GLUES_GL4ES_DIR "${CMAKE_SOURCE_DIR}/build-gl4es/glues" CACHE PATH
        "full glues built by tools/build-glues-gl4es.sh")
    # gl4es headers must shadow every other <GL/*.h>; on Apple/Emscripten
    # they mangle gl* -> gl4es_gl* and (with USE_MGL_NAMESPACE, needed
    # because glu.h checks it before gl.h defines it) glu* -> mglu*.
    include_directories(BEFORE SYSTEM "${GL4ES_DIR}/include")
    add_definitions(-DUSE_MGL_NAMESPACE=1 -DIV_GL4ES)
endif()

find_package(BISON REQUIRED)
if (EMSCRIPTEN)
    # Ports headers (SDL2, FreeType) must be visible at compile time too,
    # not just link time, or ft2build.h / SDL.h are not found.
    add_compile_options(-sUSE_SDL=2 -sUSE_FREETYPE=1)
    # JPEG/GL/Freetype come from Emscripten ports (-sUSE_LIBJPEG etc.);
    # provide empty imported targets so target_link_libraries resolves.
    add_library(JPEG::JPEG INTERFACE IMPORTED)
    add_definitions(-DIV_NO_JPEG)
    add_library(OpenGL::GL INTERFACE IMPORTED)
    add_library(OpenGL::GLU INTERFACE IMPORTED)
    add_library(Freetype::Freetype INTERFACE IMPORTED)
    if (IV_GL_BACKEND STREQUAL "gl4es")
        target_link_libraries(OpenGL::GL INTERFACE "${GL4ES_DIR}/lib/libGL-web.a")
        target_link_libraries(OpenGL::GLU INTERFACE
            "${IV_GLUES_GL4ES_DIR}/libglues-web.a" OpenGL::GL)
    endif()
elseif (IV_GL_BACKEND STREQUAL "gl4es")
    find_package(JPEG REQUIRED)
    # Native GLES2 comes from ANGLE (no native ES driver on macOS);
    # opengl-for-mac packages the headers + libGLESv2/libEGL dylibs.
    set(GLES_DIR "${CMAKE_SOURCE_DIR}/../../opengl-for-mac" CACHE PATH
        "ANGLE GLES2 headers + libGLESv2/libEGL (opengl-for-mac checkout)")
    add_library(OpenGL::GL INTERFACE IMPORTED)
    target_link_libraries(OpenGL::GL INTERFACE
        "${GL4ES_DIR}/lib/libGL-native.a"
        "${GLES_DIR}/lib/libGLESv2.dylib" "${GLES_DIR}/lib/libEGL.dylib")
    # glues precedes gl4es on the link line (its glu* call gl4es_gl*).
    add_library(OpenGL::GLU INTERFACE IMPORTED)
    target_link_libraries(OpenGL::GLU INTERFACE
        "${IV_GLUES_GL4ES_DIR}/libglues-native.a" OpenGL::GL)
    # The ANGLE dylibs carry "./libX.dylib" install names (extracted from
    # Chrome); executables must rewrite them to @rpath and carry an rpath
    # to the ANGLE lib dir. Apply to each demo/test target.
    function(iv_gl_backend_fixup tgt)
        set_property(TARGET ${tgt} APPEND PROPERTY BUILD_RPATH "${GLES_DIR}/lib")
        foreach(dylib libGLESv2.dylib libEGL.dylib)
            add_custom_command(TARGET ${tgt} POST_BUILD
                COMMAND install_name_tool -change ./${dylib} @rpath/${dylib}
                        $<TARGET_FILE:${tgt}> 2>/dev/null || true)
        endforeach()
    endfunction()
else()
    find_package(JPEG REQUIRED)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    if (APPLE)
        # The tree includes <GL/gl.h>; Apple ships <OpenGL/gl.h>.
        include_directories(BEFORE SYSTEM "${CMAKE_SOURCE_DIR}/cmake/mac-gl-shim")
        add_definitions(-DGL_SILENCE_DEPRECATION)
    endif()
endif()
if (NOT COMMAND iv_gl_backend_fixup)
    function(iv_gl_backend_fixup tgt)  # no-op on other backends
    endfunction()
endif()
if (EMSCRIPTEN)
    # SDL2/GL come from Emscripten ports via compile/link flags.
    set(SDL2_LIBRARIES "")
    set(SDL2_INCLUDE_DIRS "")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    # Use absolute paths so the linker finds SDL2 outside the default
    # search path (e.g. Homebrew's /opt/homebrew/lib on macOS).
    set(SDL2_LIBRARIES ${SDL2_LINK_LIBRARIES})
endif()
if (EMSCRIPTEN)
    add_library(Iconv::Iconv INTERFACE IMPORTED)  # emscripten libc has iconv
else()
    find_package(Iconv REQUIRED)
endif()

macro(ADD_DIRVAR VAR DIR)
    set(${VAR} ${DIR})
    add_definitions(-D${VAR}="${CMAKE_INSTALL_PREFIX}/${DIR}")
endmacro()

add_dirvar(IVBINDIR "${CMAKE_INSTALL_BINDIR}")
add_dirvar(IVHELPDIR "${CMAKE_INSTALL_DATADIR}/inventor/help")
add_dirvar(IVFONTPATH "${CMAKE_INSTALL_DATADIR}/inventor/fonts")
set(IVDATAROOT "${CMAKE_INSTALL_DATADIR}/inventor/data")
add_dirvar(IVDATADIR "${IVDATAROOT}/models")
add_dirvar(IVMATERIALSDIR "${IVDATAROOT}/materials")
add_dirvar(IVTEXTURESDIR "${IVDATAROOT}/textures")
add_dirvar(IVDEMODATADIR "${IVDATAROOT}/demos")
add_dirvar(IVDEMOBINDIR "${CMAKE_INSTALL_LIBEXECDIR}/inventor")
add_definitions(-DPDFVIEWER="xdg-open")

set(IVHDRTOP Inventor)

if (EMSCRIPTEN)
    # Cross build: codegen must run on the build host. Point
    # PPP_COMMAND at a natively built ppp (from a native build dir).
    if (NOT PPP_COMMAND)
        set(PPP_COMMAND ${CMAKE_SOURCE_DIR}/build/tools/ppp/ppp)
    endif()
    if (IV_GL_BACKEND STREQUAL "gl4es")
        # gl4es + glues come in via OpenGL::GL / OpenGL::GLU above;
        # FULL_ES2 exposes the GLES2 entry points gl4es calls into.
        set(GLUES_LIBRARY "")
        set(IV_EM_GL_LINK_OPTIONS -sFULL_ES2=1)
    else()
        # GLU for OpenGL ES (SGI tessellator/NURBS/mipmap), built by
        # tools/build-glues-em.sh into the build dir by default.
        set(GLUES_LIBRARY "${CMAKE_BINARY_DIR}/glues/libglues.a"
            CACHE FILEPATH "wasm libglues.a built by tools/build-glues-em.sh")
        # Requires the vendored patch applied to emscripten's libglemu.js
        # (tools/emscripten-patches/).
        set(IV_EM_GL_LINK_OPTIONS -sLEGACY_GL_EMULATION=1 -sGL_UNSAFE_OPTS=0)
    endif()
else()
    add_subdirectory(tools)
    set(PPP_COMMAND ppp)
endif()
add_subdirectory(libimage)
add_subdirectory(libFL)
add_subdirectory(lib)
add_subdirectory(libSoSDL)
if (NOT EMSCRIPTEN)
    # Native smoke tests; the browser targets are the demos themselves.
    add_subdirectory(apps/sdltests)
endif()
add_subdirectory(apps/sdldemos/maze)
add_subdirectory(apps/sdldemos/slotcar)
