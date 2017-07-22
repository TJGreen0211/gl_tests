#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPosition;

uniform sampler2D texture1;

out vec3 fE;
out vec3 fN;
out vec3 fL;
out vec3 fH;

out vec3 tPosition;
out vec2 cTex;
out float tOffset;

void main()
{
	vec4 ray = normalize(model*vPosition - vec4(camPosition, 1.0));
	tOffset = texture(texture1, vTexCoords).a;
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec4 lightPos = vec4(100.0, 0.0, 1.0, 1.0);
	vec3 lightDir = normalize(vPosition*model - lightPos).xyz;
	fE = normalize(vPosition*model).xyz;
	fN = normalize(vNormal*normalMatrix);
	fL = normalize(lightDir);
	fH = normalize((vPosition*model - lightPos) + ray).xyz;
	
	tPosition = vPosition.xyz;
	//gl_Position = vPosition;
	cTex = vTexCoords;
}