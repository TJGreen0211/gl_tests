# version 410

in vec4 vPosition;
in vec3 vNormal;

out vec3 fPosition;
out vec3 fNormal;

out mat3 m;
out mat3 v;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	fPosition = vec3(vPosition);
	fNormal = vNormal;
	m = mat3(model);
	v = mat3(view);
	
	gl_Position = projection*view*model*vPosition;
}