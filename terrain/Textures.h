#ifndef TEXTURES_H
#define TEXTURES_H
#ifdef __APPLE__
#define __gl_h_
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include "SOIL/SOIL.h"

GLuint loadCubemap(char **faces);
GLuint loadTexture(char const * path, int alphaFlag);
GLuint loadSpriteTexture(char const * path);
GLuint generateTextureAttachment(int depth, int stencil, int sizex, int sizey);

#endif
