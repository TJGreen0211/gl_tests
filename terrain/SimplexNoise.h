#ifndef SIMPLEXNOISE_H
#define SIMPLEXNOISE_H
#ifdef __APPLE__
#define __gl_h_
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "LinearAlg.h"
#include "LoadShader.h"

#define BUFFER_OFFSET(offset) ((char*)NULL+(offset))

void initializeNoise();
void generateNoiseTexture(GLuint framebuffer, int animated);

#endif
