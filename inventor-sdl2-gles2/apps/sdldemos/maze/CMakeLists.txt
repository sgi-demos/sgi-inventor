add_executable(maze maze.c++)
target_compile_definitions(maze PRIVATE NO_AUDIO)
target_link_libraries(maze PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (EMSCRIPTEN)
    set_target_properties(maze PROPERTIES SUFFIX ".html")
    target_link_libraries(maze PRIVATE ${GLUES_LIBRARY})
    target_link_options(maze PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        -sLEGACY_GL_EMULATION=1 -sGL_UNSAFE_OPTS=0
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0)
endif()
