#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec3 vTangent;

/*
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPosition;

out vec3 fE;
out vec3 fN;
out vec3 fL;
out vec3 fH;
*/

out vec3 tPosition;
out vec3 tNormal;
out vec3 tTangent;

void main()
{
	/*
	vec4 ray = normalize(model*vPosition - vec4(camPosition, 1.0));
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec4 lightPos = vec4(100.0, 0.0, 1.0, 1.0);
	vec3 lightDir = normalize(vPosition*model - lightPos).xyz;
	fE = normalize(vPosition*model).xyz;
	fN = normalize(vNormal*normalMatrix);
	fL = normalize(lightDir);
	fH = normalize((vPosition*model - lightPos) + ray).xyz;
	*/
	
	tPosition = vPosition.xyz;
	tNormal = vNormal;
	tTangent = vTangent;
}