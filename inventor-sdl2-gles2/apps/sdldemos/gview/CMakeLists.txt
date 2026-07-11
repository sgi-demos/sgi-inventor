add_executable(gview
    gview.c++
    DisplayGraph.c++ Error.c++ FieldEditor.c++
    GraphIcon.c++ GraphViewer.c++ NodeCreator.c++
    TextPrompt.c++

    Actions.h Menu.h MotifHelp.h
    DisplayGraph.h Error.h FieldEditor.h
    GraphIcon.h GraphViewer.h NodeCreator.h
    TextPrompt.h)
target_compile_definitions(gview PRIVATE SDL_PORT)
target_include_directories(gview PRIVATE . ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(gview PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(gview PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(gview PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(gview PROPERTIES SUFFIX ".html")
    target_link_options(gview PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    add_custom_command(TARGET gview POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:gview>)
endif()
iv_gl_backend_fixup(gview)
