add_executable(revo
    revo.c++ profile.c++ RevClass.c++ RevClass.h
    ${CMAKE_SOURCE_DIR}/apps/samples/common/LineManip.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/common/LineManipHilight.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/common/Useful.c++)
target_compile_definitions(revo PRIVATE SDL_PORT)
target_include_directories(revo PRIVATE . ${CMAKE_SOURCE_DIR}/apps/samples/common
    ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(revo PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(revo PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(revo PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(revo PROPERTIES SUFFIX ".html")
    target_link_options(revo PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0)
endif()
iv_gl_backend_fixup(revo)
