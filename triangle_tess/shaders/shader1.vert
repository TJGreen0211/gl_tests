#version 410

in vec4 vPosition;
out vec3 tPosition;

void main()
{
	tPosition = vPosition.xyz;
	//gl_Position = vPosition;
}