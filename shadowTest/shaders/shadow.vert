#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;
uniform vec3 cameraPos;

out vec2 texCoords;
out vec3 fE;
out vec3 fN;
out vec3 fL;
out vec3 fH;
out vec4 fLightSpace;

void main()
{	
	gl_Position = vPosition*model*view*projection;
	vec4 ray = normalize(model*vPosition - vec4(cameraPos, 1.0));
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec4 lightPos = vec4(50.0, -100.0, 0.0, 1.0);
	vec3 lightDir = normalize(vPosition*model - lightPos).xyz;
	fE = normalize(vPosition*model).xyz;
	fN = normalize(vNormal*normalMatrix);
	fL = normalize(lightDir);
	fH = normalize((vPosition*model - lightPos) + ray).xyz;
	
	fLightSpace = vPosition*model*lightSpace*projection;
	texCoords = vTexCoords;
	
	//vColor = vec3(1.0, 0.5, 0.2);
}