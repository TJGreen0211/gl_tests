#version 410
layout(vertices = 3) out;

in vec3 tPosition[];
in vec3 tNormal[];
in vec3 tTangent[];
out vec3 tcPosition[];
out vec3 tcNormal[];
out vec3 tcTangent[];

uniform mat4 model;
uniform mat4 view;
uniform vec3 translation;

#define ID gl_InvocationID

float level(vec3 v0, vec3 v1) {
	//return clamp(distance(v0, v1)/32.0, 2.0, 16.0);
	return clamp(512.0/length(v0 - v1), 2.0, 64.0);
}

void main()
{
	mat4 modelmat = mat4(
		1.000000, 0.000000, 0.000000, translation.x, 
		0.000000, 1.000000, 0.000000, translation.y, 
		0.000000, 0.000000, 1.000000, translation.z, 
		0.000000, 0.000000, 0.000000, 1.000000);
	mat4 tv = transpose(modelmat*view);
	vec4 camPosition = vec4(-tv[3] * tv);
	
	vec3 tessPos = vec3(vec4(tPosition[ID], 1.0)*model);
	
	float l = level(tessPos, vec3(camPosition));
	//float l = rayLength(camPosition, tPosition[ID]);

	tcPosition[ID] = tPosition[ID];
	tcNormal[ID] = tNormal[ID];
	tcTangent[ID] = tTangent[ID];
	if(ID == 0) {
		gl_TessLevelInner[0] = l;
	    gl_TessLevelOuter[0] = l-1.0;
	    gl_TessLevelOuter[1] = l-1.0;
	    gl_TessLevelOuter[2] = l-1.0;
	}
}

