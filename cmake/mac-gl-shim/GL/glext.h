/* macOS compatibility shim: <GL/glext.h> -> <OpenGL/glext.h>. */
#ifndef IV_MAC_GL_SHIM_GLEXT_H
#define IV_MAC_GL_SHIM_GLEXT_H
#define GL_SILENCE_DEPRECATION 1
#include <OpenGL/glext.h>
#endif
