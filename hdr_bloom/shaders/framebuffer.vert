#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out vec2 fTexCoords;

void main() {
	fTexCoords = vTexCoords;
	gl_Position = vPosition;
}
