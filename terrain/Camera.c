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
	tr = translate(Position.x+Front.x, Position.y+Front.y, Position.z+Front.z);
	return multiplymat4(rxry, tr);//lookAt(Position,addvec3(Position,Front), Up);//
	//return tr;
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
	
	printf("%f, %f, %f\n", Front.x, Front.y, Front.z);
	
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

void rotateCamera(float angle, float x, float y, float z) {
	quaternion temp, qView, result;
	
	temp.x = x * sin(angle/2);
	temp.y = y * sin(angle/2);
	temp.z = z * sin(angle/2);
	temp.w = cos(angle/2);
	
	qView.x = Front.x;
	qView.y = Front.y;
	qView.y = Front.z;
	qView.w = 0.0;
	
	result = quatMultiply(quatMultiply(temp, qView), quatConjugate(temp));
	
	Front.x = result.x;
	Front.y = result.y;
	Front.z = result.z;
	
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
	
	vec3 mouseDirection = {0.0, 0.0, 0.0};
	int middleX = getWindowWidth()/2;
	int middleY = getWindowHeight()/2;
	
	double currentRotX = 0.0;
	
	mouseDirection.x = (middleX - xpos)*MouseSensitivity;
	mouseDirection.y = (middleY - ypos)*MouseSensitivity;
	
	currentRotX += mouseDirection.y;
	
	if(currentRotX > 1) {
		currentRotX = 1;
	}
	if(currentRotX < -1) {
		currentRotX = -1;
	}
	else {
		vec3 sub = {Front.x - Position.x, Front.y - Position.y, Front.z - Position.z};
		vec3 axis = normalizevec3(crossvec3(sub, Up));
		
		//rotateCamera(Pitch, sub.x, sub.y, sub.z);
		//rotateCamera(Yaw, 0.0, 1.0, 0.0);
	}
	
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
