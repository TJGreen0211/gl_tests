#ifndef MAIN_H
#define MAIN_H
#ifdef __APPLE__
#define __gl_h_
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "LoadShader.h"
#include "LinearAlg.h"
#include "arcballCamera.h"
#include "Window.h"
#include "SOIL/SOIL.h"
#include "Sphere.h"
#include "Geometry.h"
#include "SimplexNoise.h"
#include "Obj.h"
#include "QuadCube.h"
#include "Water.h"
#include "Common.h"
#include "Textures.h"

#define BUFFER_OFFSET(offset) ((char*)NULL+(offset))

#endif
