set(PUCK_SOURCES
    main.c++ Player.c++ Rink.c++ MovingObject.c++
    Msg.c++ Simulation.c++ Server.c++ SoSocket.c++
    SoundSDL.c++)

add_executable(puck ${PUCK_SOURCES})
target_compile_definitions(puck PRIVATE SDL_PORT)
target_include_directories(puck PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(puck PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(puck PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(puck PRIVATE ${SDL2_LIBRARIES})
endif()

if (EMSCRIPTEN)
    set_target_properties(puck PROPERTIES SUFFIX ".html")
    target_link_options(puck PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/data)
else()
    # The game loads data/*.iv and data/sounds/*.wav by relative path;
    # copy data next to the binary so it can run from the build dir.
    add_custom_command(TARGET puck POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:puck>/data)
endif()
iv_gl_backend_fixup(puck)
