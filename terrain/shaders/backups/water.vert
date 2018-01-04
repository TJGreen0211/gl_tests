#version 410

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;
layout (location = 3) in vec3 vTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;
uniform vec3 lightPosition;

out vec4 fLightSpace;
out vec3 fragPos;
out vec3 fN;
out vec3 fT;
out vec3 fB;
out vec3 lightDir;

out vec2 fTexCoords;

void main()
{	
	gl_Position = vPosition*model*view*projection;
	
	vec3 v;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec4 lightPos = vec4(lightPosition, 1.0);
	
	fT = normalize(vec3(vec4(vTangent, 0.0) * model));
	fN = normalize(vec3(vec4(vNormal, 0.0)) * normalMatrix);
	fT = normalize(fT - dot(fT, fN) * fN);
	fB = cross(fN, fT);
	
	lightDir = normalize(vPosition*model - lightPos).xyz;
	
	fLightSpace = vPosition*model*lightSpace;
	fragPos = vec3(vPosition*model).xyz;
	//fTexCoords = vTexCoords;
	
	fTexCoords = vec2((atan(vPosition.x, vPosition.y) / 3.1415926 + 1.0) * 0.5,
                        (asin(vPosition.z) / 3.1415926 + 0.5));
	
	//vColor = vec3(1.0, 0.5, 0.2);
}