set(GVIEW_DIR ${CMAKE_SOURCE_DIR}/apps/sdldemos/gview)

add_executable(sceneviewer
    SceneViewer.c++ SoSceneViewer.c++ SvManipList.c++
    ${CMAKE_SOURCE_DIR}/apps/samples/widgets/MyFileRead.c++
    ${GVIEW_DIR}/TextPrompt.c++
    ${GVIEW_DIR}/FieldEditor.c++
    ${GVIEW_DIR}/Error.c++

    SoSceneViewer.h SoSceneMenu.h SvManipList.h)
target_compile_definitions(sceneviewer PRIVATE SDL_PORT)
target_include_directories(sceneviewer PRIVATE . ${GVIEW_DIR}
    ${CMAKE_SOURCE_DIR}/libSoSDL)
target_link_libraries(sceneviewer PRIVATE SoSDL Inventor FLiv image JPEG::JPEG OpenGL::GLU)
if (NOT EMSCRIPTEN)
    target_include_directories(sceneviewer PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(sceneviewer PRIVATE ${SDL2_LIBRARIES})
endif()
if (EMSCRIPTEN)
    set_target_properties(sceneviewer PROPERTIES SUFFIX ".html")
    target_link_options(sceneviewer PRIVATE
        -sUSE_SDL=2 -sUSE_FREETYPE=1
        ${IV_EM_GL_LINK_OPTIONS}
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/)
else()
    add_custom_command(TARGET sceneviewer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:sceneviewer>)
endif()
iv_gl_backend_fixup(sceneviewer)
