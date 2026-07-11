set(LINKATRON_SOURCES
    linkatron.c++ Axle.c++ ButtonStrip.c++ Buttons.c++ GraphPaper.c++
    LinkClasses.c++ LinkEngines.c++ MechPiece.c++ Motors.c++
    Shadow.c++ TableTop.c++ WorldInfo.c++ SoundSDL.c++)

add_executable(linkatron ${LINKATRON_SOURCES})
target_compile_definitions(linkatron PRIVATE SDL_PORT)
target_include_directories(linkatron PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(linkatron PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(linkatron PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(linkatron PRIVATE ${SDL2_LIBRARIES})
endif()

if (EMSCRIPTEN)
    set_target_properties(linkatron PROPERTIES SUFFIX ".html")
    target_link_libraries(linkatron PRIVATE ${GLUES_LIBRARY})
    target_link_options(linkatron PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    # linkagesResourceFile.iv, parts/, sounds/ and samples/ load by
    # relative path; copy data next to the binary.
    add_custom_command(TARGET linkatron POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:linkatron>)
endif()
iv_gl_backend_fixup(linkatron)
