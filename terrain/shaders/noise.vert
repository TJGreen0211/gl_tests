#version 410

layout (location = 0) in vec4 vPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = vPosition;
}