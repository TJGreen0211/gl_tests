#ifndef CAMERA_H
#define CAMERA_H
#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "LinearAlg.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};


mat4 getViewMatrix();
vec3 getCameraPosition();
vec2 getCameraRotation();

void updateCameraVectors();
void processKeyboard(enum Camera_Movement direction, GLfloat deltaTime);
void processMouseMovement(GLfloat xpos, GLfloat ypos);
float processMouseScroll(GLfloat yoffset, float zoom);

#endif