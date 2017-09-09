#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;
uniform vec3 lightPosition;

out vec4 fLightSpace;
out vec3 fragPos;
out vec3 fE;
out vec3 fN;
out vec3 fL;
out vec2 fTexCoords;

void main()
{	
	gl_Position = vPosition*model*view*projection;
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec4 lightPos = vec4(lightPosition, 1.0);
	vec3 lightDir = normalize(vPosition*model - lightPos).xyz;
	fE = -normalize(vPosition*model).xyz;
	fN = normalize(vNormal*normalMatrix);
	fL = -normalize(lightDir);
	
	fLightSpace = vPosition*model*lightSpace;
	fragPos = vec3(vPosition*model).xyz;
	fTexCoords = vTexCoords;
	
	//vColor = vec3(1.0, 0.5, 0.2);
}