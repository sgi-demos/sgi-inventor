set(SPACECADET_SOURCES
    Main.c++ Blaster.c++ Chamber.c++ HighScore.c++ Meter.c++
    Mine.c++ Panel.c++ Ring.c++ Scene.c++ Setup.c++ Ship.c++
    Tunnel.c++ XStuff.c++ SoundSDL.c++)

add_executable(spacecadet ${SPACECADET_SOURCES})
target_compile_definitions(spacecadet PRIVATE SDL_PORT)
target_include_directories(spacecadet PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(spacecadet PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(spacecadet PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(spacecadet PRIVATE ${SDL2_LIBRARIES})
endif()

if (EMSCRIPTEN)
    set_target_properties(spacecadet PROPERTIES SUFFIX ".html")
    target_link_options(spacecadet PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    # scenes/, textures/ and the sound WAVs load by relative path; copy
    # data next to the binary so it runs from the build directory.
    add_custom_command(TARGET spacecadet POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:spacecadet>)
endif()
iv_gl_backend_fixup(spacecadet)
