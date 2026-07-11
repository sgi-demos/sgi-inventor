add_executable(textomatic
    textomatic.c++ TextGraph.c++ TextWrapper.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/common/LineManip.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/common/LineManipHilight.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/common/Useful.c++
    TextGraph.h TextWrapper.h labels.h)
target_compile_definitions(textomatic PRIVATE SDL_PORT)
target_include_directories(textomatic PRIVATE .
    ${CMAKE_SOURCE_DIR}/apps/samples/common ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(textomatic PRIVATE SoSDL Inventor FLiv image
    JPEG::JPEG OpenGL::GLU Iconv::Iconv)
if (NOT EMSCRIPTEN)
    target_include_directories(textomatic PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(textomatic PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(textomatic PROPERTIES SUFFIX ".html")
    target_link_options(textomatic PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    add_custom_command(TARGET textomatic POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:textomatic>)
endif()
iv_gl_backend_fixup(textomatic)
