#include "LinearAlg.h"

const float DegreesToRadians = M_PI / 180.0;

mat4 IDENTITY_MATRIX = {
	{{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}}
};

mat4 ZERO_MATRIX = {{
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0}
}};


vec4 addvec4(vec4 v, vec4 u)
{
	vec4 temp = {v.x + u.x, v.y + u.y, v.z + u.z, v.w + u.w,};
	return temp;
}

vec3 addvec3(vec3 v, vec3 u)
{
	vec3 temp = {v.x + u.x, v.y + u.y, v.z + u.z};
	return temp;
}

vec2 addvec2(vec2 v, vec2 u)
{
	vec2 temp = {v.x + u.x, v.y + u.y };
	return temp;
}

vec4 scalarMultvec4(vec4 v, float u)
{
	vec4 temp = {v.x * u, v.y * u, v.z * u, v.w *u};
	return temp;
}

vec3 scalarMultvec3(vec3 v, float u)
{
	vec3 temp = {v.x * u, v.y * u, v.z * u};
	return temp;
}

vec2 scalarMultvec2(vec2 v, float u)
{
	vec2 temp = {v.x * u, v.y * u};
	return temp;
}

vec4 dividevec4(vec4 v, float u)
{
	vec4 temp = {v.x / u, v.y / u, v.z / u, v.w / u};
	return temp;
}

vec3 dividevec3(vec3 v, float u)
{
	vec3 temp = {v.x / u, v.y / u, v.z / u};
	return temp;
}

vec2 dividevec2(vec2 v, float u)
{
	vec2 temp = {v.x / u, v.y / u};
	return temp;
}


vec4 plusequalvec4(vec4 v, vec4 u)
{
	vec4 temp = {v.x += u.x, v.y += u.y, v.z += u.z, v.w += u.w};
	return temp;
}

vec3 plusequalvec3(vec3 v, vec3 u)
{
	vec3 temp = {v.x += u.x, v.y += u.y, v.z += u.z};
	return temp;
}

vec2 plusequalvec2(vec2 v, vec2 u)
{
	vec2 temp = {v.x += u.x, v.y += u.y};
	return temp;
}

vec4 minusequalvec4(vec4 v, vec4 u)
{
	vec4 temp = {v.x -= u.x, v.y -= u.y, v.z -= u.z, v.w -= u.w};
	return temp;
}

vec3 minusequalvec3(vec3 v, vec3 u)
{
	vec3 temp = {v.x -= u.x, v.y -= u.y, v.z -= u.z};
	return temp;
}

vec2 minusequalvec2(vec2 v, vec2 u)
{
	vec2 temp = {v.x -= u.x, v.y -= u.y};
	return temp;
}

float dotvec4(vec4 u, vec4 v) {
	return u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w;
}

float dotvec3(vec3 u, vec3 v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

float dotvec2(vec2 u, vec2 v) {
	return u.x*v.x + u.y*v.y;
}

float lengthvec4(vec4 v) {
	return sqrt(dotvec4(v, v));
}

float lengthvec3(vec3 v) {
	return sqrt(dotvec3(v, v));
}

float lengthvec2(vec2 v) {
	return sqrt(dotvec2(v, v));
}

vec4 normalizevec4(vec4 v) {
	float vecLength = lengthvec4(v);
	vec4 dividebyzero = {0.0, 0.0, 0.0, 0.0};
	if (vecLength == 0)
		return dividebyzero;
	float X, Y, Z;
	X = v.x / vecLength;
	Y = v.y / vecLength;
	Z = v.z / vecLength;
	vec4 u = {X, Y, Z, v.w};
	return u;
}

vec3 normalizevec3(vec3 v) {
	float vecLength = lengthvec3(v);
	vec3 dividebyzero = {0.0, 0.0, 0.0};
	if (vecLength == 0)
		return dividebyzero;
	float X, Y, Z;
	X = v.x / vecLength;
	Y = v.y / vecLength;
	Z = v.z / vecLength;
	vec3 u = {X, Y, Z};
	return u;
}

vec2 normalizevec2(vec2 v) {
	float vecLength = lengthvec2(v);
	vec2 dividebyzero = {0.0, 0.0};
	if (vecLength == 0)
		return dividebyzero;
	float X, Y;
	X = v.x / vecLength;
	Y = v.y / vecLength;
	vec2 u = {X, Y};
	return u;
}

vec4 crossvec4(vec4 v, vec4 u) {
	vec4 out = {v.y * u.z - v.z * u.y,
				v.z * u.x - v.x * u.z,
				v.x * u.y - v.y * u.x, 
				0.0};
	return out;
}

vec3 crossvec3(vec3 v, vec3 u) {
	vec3 out = {v.y * u.z - v.z * u.y,
				v.z * u.x - v.x * u.z,
				v.x * u.y - v.y * u.x};
	return out;
}

vec4 multiplyvec4(vec4 v, vec4 u)
{
	vec4 out = { u.x*v.x, u.y*v.y, u.z*v.z, u.w*v.w }; 
	return out;
}

vec3 multiplyvec3(vec3 v, vec3 u)
{
	vec3 out = { u.x*v.x, u.y*v.y, u.z*v.z }; 
	return out;
}

vec2 multiplyvec2(vec2 v, vec2 u)
{
	vec2 out = { u.x*v.x, u.y*v.y }; 
	return out;
}

mat4 rotateX(float theta)
{
	float angle = DegreesToRadians * theta;
	mat4 rotation = IDENTITY_MATRIX;
	rotation.m[2][2] = rotation.m[1][1] = cos(angle);
	rotation.m[2][1] = sin(angle);
	rotation.m[1][2] = -rotation.m[2][1];
	return rotation;
}

mat4 rotateY(float theta)
{
	float angle = DegreesToRadians * theta;
	mat4 rotation = IDENTITY_MATRIX;
	rotation.m[2][2] = rotation.m[0][0] = cos(angle);
	rotation.m[0][2] = sin(angle);
	rotation.m[2][0] = -rotation.m[0][2];
	return rotation;
}

mat4 rotateZ(float theta)
{
	float angle = DegreesToRadians * theta;
	mat4 rotation = IDENTITY_MATRIX;
	rotation.m[0][0] = rotation.m[1][1] = cos(angle);
	rotation.m[1][0] = sin(angle);
	rotation.m[0][1] = -rotation.m[1][0];
	return rotation;
}

mat4 translate(float x, float y, float z)
{
	mat4 c = IDENTITY_MATRIX;
	c.m[0][3] = x;
	c.m[1][3] = y;
	c.m[2][3] = z;
	return c;
}

mat4 translatevec4( vec4 v )
{
    return translate( v.x, v.y, v.z );
}

mat4 translatevec3( vec3 v )
{
    return translate( v.x, v.y, v.z );
}

mat4 translatevec2( vec2 v )
{
    return translate( v.x, v.y, 0.0 );
}

mat4 scalevec4(float x, float y, float z)
{
	mat4 c = IDENTITY_MATRIX;
	c.m[0][0] = x;
	c.m[1][1] = y;
	c.m[2][2] = z;
	return c;
}

mat4 scalevec3(vec3 v)
{
	return scalevec4(v.x, v.y, v.z);
}

mat4 scale(float value)
{
	return scalevec4(value, value, value);
}

mat4 multiplymat4(mat4 c, mat4 d)
{
	mat4 out = ZERO_MATRIX;
	for ( int i = 0; i < 4; ++i ) {
	    for ( int j = 0; j < 4; ++j ) {
		for ( int k = 0; k < 4; ++k ) {
		    out.m[i][j] += c.m[i][k] * d.m[k][j];
		}
	    }
	}
	
	return out;
}

vec4 multiplymat4vec4(mat4 a, vec4 v)
{
	vec4 vout;
	float temp = 0;
	float u[4] = {0};
	u[0] = v.x;
	u[1] = v.y;
	u[2] = v.z;
	u[3] = v.w;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			temp += a.m[i][j]*u[j];
		}
		u[i] = temp;
		temp = 0;
	}
	vout.x = u[0];
	vout.y = u[1];
	vout.z = u[2];
	vout.w = u[3];
	return vout;
}

mat4 perspective(float fovy, float aspect, float zNear, float zFar)
{
	float top = tan(fovy*DegreesToRadians/2) * zNear;
	float right = top * aspect;
	
	mat4 c = ZERO_MATRIX;
	c.m[0][0] = zNear/right;
	c.m[1][1] = zNear/top;
	c.m[2][2] = -(zFar + zNear)/(zFar - zNear);
	c.m[2][3] = -2.0*zFar*zNear/(zFar - zNear);
	c.m[3][2] = -1.0;
	c.m[3][3] = 0.0;
	return c;
}

mat4 frustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	mat4 c = ZERO_MATRIX;
	c.m[0][0] = 2.0*zNear/(right - left);
    c.m[0][2] = (right + left)/(right - left);
    c.m[1][1] = 2.0*zNear/(top - bottom);
    c.m[1][2] = (top + bottom)/(top - bottom);
    c.m[2][2] = -(zFar + zNear)/(zFar - zNear);
    c.m[2][3] = -2.0*zFar*zNear/(zFar - zNear);
    c.m[3][2] = -1.0;
    c.m[3][3] = 0.0;
    return c;
}

mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar )
{
    mat4 c = ZERO_MATRIX;
    c.m[0][0] = 2.0/(right - left);
    c.m[1][1] = 2.0/(top - bottom);
    c.m[2][2] = 2.0/(zNear - zFar);
    c.m[3][3] = 1.0;
    c.m[0][3] = -(right + left)/(right - left);
    c.m[1][3] = -(top + bottom)/(top - bottom);
    c.m[2][3] = -(zFar + zNear)/(zFar - zNear);
    return c;
}

mat4 lookAt(vec3 eye, vec3 at, vec3 up)
{
	vec3 umv = {eye.x - at.x, eye.y - at.y, eye.z - at.z};
	vec3 n = normalizevec3(umv);
	vec3 u = normalizevec3(crossvec3(up, n));
	vec3 v = normalizevec3(crossvec3(n, u));
	mat4 c = IDENTITY_MATRIX;
	c.m[0][0] = u.x;
	c.m[1][0] = u.y;
	c.m[2][0] = u.z;
	
	c.m[0][1] = v.x;
	c.m[1][1] = v.y;
	c.m[2][1] = v.z;
	
	c.m[0][2] = n.x;
	c.m[1][2] = n.y;
	c.m[2][2] = n.z;
	
	c.m[0][3] = 0.0f;
	c.m[1][3] = 0.0f;
	c.m[2][3] = 0.0f;
	c.m[3][3] = 1.0f;
	vec3 negativeEye = {-eye.x, -eye.y, -eye.x};
	mat4 d = translatevec3(negativeEye);
	mat4 out = multiplymat4(c, d);
	return out;	
}