set(PBN_COMMON Grid.c++ Line.c++ Spec.c++ Solver.c++ Change.c++ Message.c++)

add_executable(pbnsolve pbnsolve.c++ ${PBN_COMMON})
target_compile_definitions(pbnsolve PRIVATE SDL_PORT)
target_include_directories(pbnsolve PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(pbnsolve PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(pbnsolve PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(pbnsolve PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(pbnsolve PROPERTIES SUFFIX ".html")
    target_link_libraries(pbnsolve PRIVATE ${GLUES_LIBRARY})
    target_link_options(pbnsolve PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    add_custom_command(TARGET pbnsolve POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:pbnsolve>)
endif()
iv_gl_backend_fixup(pbnsolve)

add_executable(pbncreate pbncreate.c++ ${PBN_COMMON})
target_compile_definitions(pbncreate PRIVATE SDL_PORT)
target_include_directories(pbncreate PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(pbncreate PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(pbncreate PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(pbncreate PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(pbncreate PROPERTIES SUFFIX ".html")
    target_link_libraries(pbncreate PRIVATE ${GLUES_LIBRARY})
    target_link_options(pbncreate PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
endif()
iv_gl_backend_fixup(pbncreate)
