add_executable(ivread ivread.c++)
target_link_libraries(ivread PRIVATE Inventor FLiv image JPEG::JPEG)
set(SDLTESTS ivread ivview ivinteract)
if (NOT IV_GL4ES)
    # ivcone is the raw-SDL M2 smoke test: it creates its own GL window
    # without libSoSDL, so it never runs the gl4es bootstrap. Desktop only.
    add_executable(ivcone ivcone.c++)
    target_link_libraries(ivcone PRIVATE Inventor FLiv image JPEG::JPEG ${SDL2_LIBRARIES} OpenGL::GL OpenGL::GLU)
    list(APPEND SDLTESTS ivcone)
endif()
add_executable(ivview ivview.c++)
target_link_libraries(ivview PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
add_executable(ivinteract ivinteract.c++)
target_link_libraries(ivinteract PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)

foreach(t ${SDLTESTS})
    iv_gl_backend_fixup(${t})
endforeach()
