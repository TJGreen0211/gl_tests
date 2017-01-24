#ifndef MAIN_H
#define MAIN_H
#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
//#include <GL/glew.h>
#include "LinearAlg.h"
#include "LoadShader.h"
#include "Camera.h"
#include "Sphere.h"


#define BUFFER_OFFSET( offset )   ((char*)NULL+(offset))

#endif