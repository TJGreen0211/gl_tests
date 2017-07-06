#include "Camera.h"

const float toRadians = M_PI / 180.0;

GLfloat Yaw = -90.0f;
GLfloat Pitch = 0.0f;
GLfloat MovementSpeed = 1000.1f;
GLfloat maxSpeed = 2000.0f;
GLfloat MouseSensitivity = 0.6f;
GLfloat Zoom = 45.0f;

float xpos = 0, ypos = 0, zpos = 0;
float lastx, lasty;

mat4 tr, ry, rx, rxry;
vec3 Position = {0.0, 0.0, 0.0};
vec3 Up = {0.0, 0.0, 1.0};
vec3 Front = {0.0, 0.0, -1.0};
vec3 Right = {1.0, 0.0, 0.0};

mat4 getViewMatrix()
{	
	rx = rotateX(Pitch);
	ry = rotateY(Yaw);
	rxry = multiplymat4(rx, ry);
	tr = translate(Position.x+Front.x, 0.0, Position.z+Front.z);
	return multiplymat4(rxry, tr);//lookAt(Position,addvec3(Position,Front), Up);//
}

mat4 getViewPosition()
{
	return tr;
}

mat4 getViewRotation()
{
	return rxry;
}

void updateCameraVectors()
{
	vec3 front;
	front.x = cos(toRadians * Yaw) * cos(toRadians * Pitch);
	front.y = sin(toRadians * Pitch);
	front.z = sin(toRadians * Yaw) * cos(toRadians * Pitch);
	Front = normalizevec3(front);
	
	Right = normalizevec3(crossvec3(Front, Up));
	Up = normalizevec3(crossvec3(Right, Front));
}

void processKeyboard(enum Camera_Movement direction, GLfloat deltaTime, GLfloat deltaSpeed)
{
	if(deltaSpeed > maxSpeed)
		deltaSpeed = maxSpeed;
	GLfloat velocity = MovementSpeed * deltaTime + deltaSpeed;
	
    if (direction == FORWARD)
        Position = plusequalvec3(Position, scalarMultvec3(Right, velocity));
    if (direction == BACKWARD)
        Position = minusequalvec3(Position, scalarMultvec3(Right, velocity));
    if (direction == LEFT)
        Position = minusequalvec3(Position, scalarMultvec3(Front, velocity));
    if (direction == RIGHT)
        Position = plusequalvec3(Position, scalarMultvec3(Front, velocity));
}

int constrainPitch;
void processMouseMovement(GLfloat xpos, GLfloat ypos)
{
	int diffx = xpos - lastx;
	int diffy = ypos - lasty;
	lastx = xpos;
	lasty = ypos;
	diffx *= MouseSensitivity;
	diffy *= MouseSensitivity;
	
	Yaw += diffx;
	Pitch += diffy;
	
	//if(constrainPitch == 1)
	//{
		if(Pitch > 89.0)
			Pitch = 89.0f;
		if(Pitch < -89.0)
			Pitch = -89.0f;
	//}
	updateCameraVectors();
}

float processMouseScroll(GLfloat yoffset, float zoom)
{
	if (zoom >= 1.0f && zoom <= 3.5f)
        zoom -= yoffset*0.1;
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 3.5f)
         zoom = 3.5f;
    return zoom;
}
