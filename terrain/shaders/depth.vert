#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

out vec4 fragPos;

void main()
{	
	gl_Position = vPosition*model*lightSpace;
	fragPos = vPosition*model;
}