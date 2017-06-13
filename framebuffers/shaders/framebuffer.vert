#version 410

in vec4 vPosition;
in vec4 vNormal;
in vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoords;

void main() {
	gl_Position = vPosition*model*view*projection;
	texCoords = vTexCoords;
}