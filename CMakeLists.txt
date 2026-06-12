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
else()
    find_package(JPEG REQUIRED)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
endif()
if (EMSCRIPTEN)
    # SDL2/GL come from Emscripten ports via compile/link flags.
    set(SDL2_LIBRARIES "")
    set(SDL2_INCLUDE_DIRS "")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
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
else()
    add_subdirectory(tools)
    set(PPP_COMMAND ppp)
endif()
add_subdirectory(libimage)
add_subdirectory(libFL)
add_subdirectory(lib)
add_subdirectory(libSoSDL)
add_subdirectory(apps/sdltests)
add_subdirectory(apps/sdldemos/maze)
add_subdirectory(apps/sdldemos/slotcar)
