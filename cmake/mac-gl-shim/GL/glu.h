/* macOS compatibility shim: <GL/glu.h> -> <OpenGL/glu.h>. */
#ifndef IV_MAC_GL_SHIM_GLU_H
#define IV_MAC_GL_SHIM_GLU_H
#define GL_SILENCE_DEPRECATION 1
#include <OpenGL/glu.h>
#endif
