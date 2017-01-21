#version 410

in vec4 vPosition;
in vec3 vNormal;

out vec3 fN;
out vec3 fE;
out vec3 fL;
out vec3 fH;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{	
	//mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec4 lightPos = vec4(0.0, 0.0, -100.0, 1.0);
	vec3 lightDir = normalize(model*vPosition - lightPos).xyz;
	gl_Position = vPosition*model*view*projection;
	
	fE = normalize(model*vPosition).xyz;
    fN = normalize(vNormal);
    fL = vPosition.xyz;
	if( lightPos.w != 0.0 ) {
		fL = normalize(lightDir);
    }
	fH = normalize(fL - fE);
}