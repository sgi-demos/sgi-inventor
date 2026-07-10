add_library(SoSDL STATIC
    SoSDL.c++ SoSDLRenderArea.c++ SoSDLExaminerViewer.c++
    $<$<AND:$<BOOL:${EMSCRIPTEN}>,$<NOT:$<BOOL:${IV_GL4ES}>>>:gl1stubs.c>
    SoSDL.h SoSDLRenderArea.h SoSDLExaminerViewer.h
)
set_property(TARGET SoSDL PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(SoSDL PUBLIC .)
target_link_libraries(SoSDL PUBLIC Inventor ${SDL2_LIBRARIES} OpenGL::GL)
