add_executable(drop
    drop.c++ TsBlock.c++ TsEvents.c++ TsField.c++ TsPiece.c++
    TsPieces.c++ TsRemoval.c++ TsTrickle.c++ TsViewer.c++)
target_compile_definitions(drop PRIVATE SDL_PORT)
target_include_directories(drop PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(drop PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(drop PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(drop PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(drop PROPERTIES SUFFIX ".html")
    target_link_options(drop PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0)
endif()
iv_gl_backend_fixup(drop)
