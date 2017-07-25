#include "Camera.h"

const float toRadians = M_PI / 180.0;

GLfloat Yaw = -90.0f;
GLfloat Pitch = 0.0f;
GLfloat MovementSpeed = 10.1f;
GLfloat maxSpeed = 2000.0f;
GLfloat MouseSensitivity = 0.6f;
GLfloat Zoom = 45.0f;

float xpos = 0, ypos = 0, zpos = 0;
float lastx, lasty;

mat4 tr, ry, rx, rxry;
vec3 Position = {10.0, -10.0, -10.0};
vec3 Up = {0.0, 1.0, 0.0};
vec3 Front = {0.0, 0.0, -1.0};
vec3 Right = {1.0, 0.0, 0.0};

mat4 rotm;

mat4 getViewMatrix()
{	
	rx = rotateX(Pitch);
	ry = rotateY(Yaw);
	rxry = multiplymat4(rx, ry);
	tr = translate(Position.x, Position.y, Position.z);
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
	
	//printf("%f, %f, %f\n", Front.x, Front.y, Front.z);
	
	//Right = normalizevec3(crossvec3(Front, Up));
	//Up = normalizevec3(crossvec3(Front, Right));
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
void processMouseMovement(GLfloat xpos, GLfloat ypos)
{
	//vec3 front;
	int diffx = xpos - lastx;
	int diffy = ypos - lasty;
	lastx = xpos;
	lasty = ypos;
	diffx *= MouseSensitivity;
	diffy *= MouseSensitivity;
	
	Yaw += diffx;
	Pitch += diffy;
	
	//printf("%f, %f", Yaw, Pitch);
	
	vec3 mouseDirection = {0.0, 0.0, 0.0};
	int middleX = getWindowWidth()/2;
	int middleY = getWindowHeight()/2;
	
	double currentRotX = 0.0;
	
	mouseDirection.x = (middleX - xpos)*MouseSensitivity;
	mouseDirection.y = (middleY - ypos)*MouseSensitivity;
	
	currentRotX += mouseDirection.y;
	
	/*if(currentRotX > 1) {
		currentRotX = 1;
	}
	if(currentRotX < -1) {
		currentRotX = -1;
	}
	else {*/
		//vec3 sub = {Front.x - Position.x, Front.y - Position.y, Front.z - Position.z};
		//vec3 axis = normalizevec3(crossvec3(sub, Up));
		
	
	//vec3 test = {0.0, 1.0, 0.0};
	//one = rotateCamera(90.0*toRadians, test);
	
	//float theta = 2.0*atan2(sqrt(one.x*one.x+one.y*one.y+one.z*one.z), one.w);
	//printf("one %f, %f, %f\n", one.x, one.y, one.z);
	
	//printf("theta: %f, thetar: %f, Pitch: %f, Pitchr: %f\n", theta, theta*toRadians, Pitch, Pitch*toRadians);
	
		
	//}
	
	//if(constrainPitch == 1)
	//{
		if(Pitch > 89.0)
			Pitch = 89.0f;
		if(Pitch < -89.0)
			Pitch = -89.0f;
	//}
	//updateCameraVectors();
	//Front = normalizevec3(front);
	
	vec3 axis = {0.0, 1.0, 0.0};
	vec3 point = {1.0, 0.0, 0.0};
	quaternion test = angleAxis(90.0*toRadians, axis, point);
	printf("quaternion: %f, %f, %f, %f\n", test.w, test.x, test.y, test.z);
	
	//quaternion q = { 0.707107, 0.000000, 0.707107, 0.000000};
	//quaternion conjq = quatConjugate(q);
	//printf("quaternion Conjugate: %f, %f, %f, %f\n", conjq.w, conjq.x, conjq.y, conjq.z);
	
	//Right = normalizevec3(crossvec3(Front, Up));
	//Up = normalizevec3(crossvec3(Right, Front));
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
