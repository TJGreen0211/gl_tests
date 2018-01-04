#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out vec4 fPosition;
out mat4 m;
out mat4 v;
out vec3 fNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{	
	fPosition = vPosition;
	fNormal = vNormal;
	m = model;
	v = view;
	gl_Position = vPosition*model*view*projection;
}