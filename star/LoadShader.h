#ifndef LOADSHADER_H
#define LOADSHADER_H
#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

GLuint LoadShader(char *shaderPath, int shaderType);

#endif