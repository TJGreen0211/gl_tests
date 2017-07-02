#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vColor;

out vec3 tPosition;

void main()
{
	tPosition = vPosition.xyz;
	//gl_Position = vPosition;
}