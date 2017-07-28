#ifndef CAMERA_H
#define CAMERA_H
#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include "LinearAlg.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

mat4 getViewMatrix();
mat4 getViewPosition();
mat4 getViewRotation();

void updateCameraVectors();
void processKeyboard(enum Camera_Movement direction, GLfloat deltaTime , GLfloat deltaSpeed);
void processMouseMovement(float xpos, float ypos, int resetFlag);
float processMouseScroll(float yoffset, float zoom);

#endif