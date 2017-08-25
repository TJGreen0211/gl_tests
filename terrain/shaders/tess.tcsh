#version 410
layout(vertices = 3) out;

in vec3 tPosition[];
in float tOffset[];
in vec2 cTex[];
out vec3 tcPosition[];
out vec2 eTex[];
out float eOffset[];

#define ID gl_InvocationID

void main()
{
	tcPosition[ID] = tPosition[ID];
	if(ID == 0) {
		gl_TessLevelInner[0] = 1.0;
	    gl_TessLevelOuter[0] = 1.0;
	    gl_TessLevelOuter[1] = 1.0;
	    gl_TessLevelOuter[2] = 1.0;
	}
	eTex[ID] = cTex[ID];
	eOffset[ID] = tOffset[ID];
}

