#version 410

in vec4 vPosition;
in vec3 vNormal;

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
	gl_Position = projection*view*model*vPosition;
}