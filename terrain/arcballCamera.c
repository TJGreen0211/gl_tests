#include "arcballCamera.h"

const float toRadians = M_PI / 180.0;

GLfloat Yaw = -90.0f;
GLfloat Pitch = 0.0f;
GLfloat MovementSpeed = 10.1f;
GLfloat maxSpeed = 2000.0f;
GLfloat MouseSensitivity = 0.6f;
GLfloat mouseZoom = 1.0f;

float lastx, lasty;

mat4 tr, rxry;
vec3 rotation = {0.0, 0.0, 0.0};
vec3 Position = {0.0, 1.0, 0.0};
vec3 Up = {0.0, 0.0, 1.0};
vec3 Front = {0.0, 0.0, -1.0};
vec3 Right = {1.0, 0.0, 0.0};

mat4 getViewMatrix()
{
	float rad = 180.0 / M_PI;
	float arcYaw, arcPitch;

	tr = translate(rotation.x+Position.x, rotation.y+Position.y, rotation.z+Position.z);
	vec3 d = {rotation.x - 0.0, rotation.y - 0.0, rotation.z - 0.0};
	d = normalizevec3(d);
	arcYaw = asin(-d.y) * rad;
	arcPitch = atan2(d.x, -d.z) * rad;
	rxry = multiplymat4(rotateX(arcYaw), rotateY(arcPitch));

	return multiplymat4(rxry, tr);
}

vec3 getCamera() {
	return Position;
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
	Front.x = rxry.m[2][0];
	Front.y = rxry.m[2][1];
	Front.z = rxry.m[2][2];
	Front = normalizevec3(Front);

	Right.x = rxry.m[0][0];
	Right.y = rxry.m[0][1];
	Right.z = rxry.m[0][2];
	Right = normalizevec3(Right);

	Up.x = rxry.m[1][0];
	Up.y = rxry.m[1][1];
	Up.z = rxry.m[1][2];
	Up = normalizevec3(Up);
}

void processKeyboard(enum Camera_Movement direction, GLfloat deltaTime, GLfloat deltaSpeed)
{
	if(deltaSpeed > maxSpeed)
		deltaSpeed = maxSpeed;
	GLfloat velocity = MovementSpeed * deltaTime + deltaSpeed;

    if (direction == FORWARD)
        Position = plusequalvec3(Position, scalarMultvec3(Front, velocity));
    if (direction == BACKWARD)
        Position = minusequalvec3(Position, scalarMultvec3(Front, velocity));
    if (direction == LEFT)
        Position = minusequalvec3(Position, scalarMultvec3(Right, velocity));
    if (direction == RIGHT)
        Position = plusequalvec3(Position, scalarMultvec3(Right, velocity));
}

int constrainPitch;
void processMouseMovement(GLfloat xpos, GLfloat ypos, int resetFlag)
{
	vec3 mouseArc = {xpos, ypos, 0.0};
	mouseArc.y = -mouseArc.y;
	float mouseArc2 = mouseArc.x * mouseArc.x + mouseArc.y * mouseArc.y;

	if(mouseArc2 <= 1*1)
		mouseArc.z = sqrt(1*1 - mouseArc2);
	else
		mouseArc = normalizevec3(mouseArc);

	if(resetFlag) {
		lastx = xpos;
		lasty = ypos;
	}
	else {
		int diffx = xpos - lastx;
		int diffy = ypos - lasty;
		lastx = xpos;
		lasty = ypos;
		diffx *= MouseSensitivity;
		diffy *= MouseSensitivity;

		Yaw += diffx;
		Pitch += diffy;

		/*if(Pitch > 89.0)
			Pitch = 89.0f;
		if(Pitch < -89.0)
			Pitch = -89.0f;*/

		rotation.x = mouseZoom * cos(Yaw/50.0) * sin(-Pitch/50.0);
		rotation.y = mouseZoom * cos(-Pitch/50.0);
		rotation.z = mouseZoom * sin(Yaw/50.0) * sin(-Pitch/50.0);

		updateCameraVectors();
	}
}

float processMouseScroll(GLfloat yoffset, float zoom)
{
	if (mouseZoom >= 1.0f && mouseZoom <= 20.0f)
        mouseZoom -= yoffset;
    if (mouseZoom <= 1.0f)
        mouseZoom = 1.0f;
    if (mouseZoom >= 20.0f)
         mouseZoom = 20.0f;

    rotation.x = mouseZoom * cos(Yaw/50.0) * sin(-Pitch/50.0);
	rotation.y = mouseZoom * cos(-Pitch/50.0);
	rotation.z = mouseZoom * sin(Yaw/50.0) * sin(-Pitch/50.0);

    return mouseZoom;
}
