add_executable(qmorf
    qmorf.c++ QuadThing.c++ Background.c++ FlashBackground.c++
    QuadThing.h Background.h)
target_compile_definitions(qmorf PRIVATE SDL_PORT)
target_include_directories(qmorf PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(qmorf PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(qmorf PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(qmorf PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(qmorf PROPERTIES SUFFIX ".html")
    target_link_options(qmorf PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_SOURCE_DIR}/data/models/CyberHeads@/heads)
else()
    add_custom_command(TARGET qmorf POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data/models/CyberHeads $<TARGET_FILE_DIR:qmorf>/heads)
endif()
iv_gl_backend_fixup(qmorf)
