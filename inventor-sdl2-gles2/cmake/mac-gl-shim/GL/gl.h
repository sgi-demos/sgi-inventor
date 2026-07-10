/* macOS compatibility shim: the tree includes <GL/gl.h> (Linux/IRIX
 * convention); Apple ships the same headers as <OpenGL/gl.h>. */
#ifndef IV_MAC_GL_SHIM_GL_H
#define IV_MAC_GL_SHIM_GL_H
#define GL_SILENCE_DEPRECATION 1
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif
