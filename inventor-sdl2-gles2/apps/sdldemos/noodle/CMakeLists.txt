set(CYLINDER_DIR ${CMAKE_SOURCE_DIR}/apps/nodes/GeneralizedCylinder)

add_executable(noodle
    noodle.c++ profile.c++ Interface.c++ WorldInfo.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/common/LineManip.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/common/LineManipHilight.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/common/Useful.c++
    ${CYLINDER_DIR}/NurbMaker.c++
    ${CYLINDER_DIR}/Triangulator.c++
    ${CYLINDER_DIR}/GeneralizedCylinder.c++
    Interface.h WorldInfo.h MyGrids.h PullDowns.h)
target_compile_definitions(noodle PRIVATE SDL_PORT)
target_include_directories(noodle PRIVATE . ${CYLINDER_DIR}
    ${CMAKE_SOURCE_DIR}/apps/samples/common ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(noodle PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(noodle PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(noodle PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(noodle PROPERTIES SUFFIX ".html")
    target_link_options(noodle PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    add_custom_command(TARGET noodle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:noodle>)
endif()
iv_gl_backend_fixup(noodle)
