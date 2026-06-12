set(SLOTCAR_SOURCES
    slotcar.c++ Simulation.c++ MouseInput.c++
    Network.c++ Packet.c++ RaceCar.c++ State.c++
    Track.c++ Curve.c++ Straight.c++ StartFinish.c++
    Sky.c++ RobotCar.c++ LODD.c++ Car.c++
    SoundSDL.c++)

add_executable(slotcar ${SLOTCAR_SOURCES})
target_compile_definitions(slotcar PRIVATE SDL_PORT)
target_include_directories(slotcar PRIVATE ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(slotcar PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    find_package(SDL2 REQUIRED)
    target_include_directories(slotcar PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(slotcar PRIVATE ${SDL2_LIBRARIES})
endif()

if (EMSCRIPTEN)
    set_target_properties(slotcar PROPERTIES SUFFIX ".html")
    target_link_libraries(slotcar PRIVATE /home/claude/glues/build-em/libglues.a)
    target_link_options(slotcar PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        -sLEGACY_GL_EMULATION=1 -sGL_UNSAFE_OPTS=0
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    # The game loads its .iv data by relative path; copy data next to the
    # binary so it can run from the build directory.
    add_custom_command(TARGET slotcar POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:slotcar>)
endif()
