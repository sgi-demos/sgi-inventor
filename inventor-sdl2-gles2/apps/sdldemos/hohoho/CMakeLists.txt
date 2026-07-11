add_executable(hohoho card.c++ flakeset.c++ sincos.c++ swirl.c++)
target_compile_definitions(hohoho PRIVATE SDL_PORT)
target_include_directories(hohoho PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(hohoho PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(hohoho PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(hohoho PRIVATE ${SDL2_LIBRARIES})
endif()

if (EMSCRIPTEN)
    set_target_properties(hohoho PROPERTIES SUFFIX ".html")
    target_link_libraries(hohoho PRIVATE ${GLUES_LIBRARY})
    target_link_options(hohoho PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    # The card loads its .iv data by relative path; copy data next to
    # the binary so it can run from the build directory.
    add_custom_command(TARGET hohoho POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:hohoho>)
endif()
iv_gl_backend_fixup(hohoho)
