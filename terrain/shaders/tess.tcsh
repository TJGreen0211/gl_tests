#version 410
layout(vertices = 3) out;

in vec3 tPosition[];
out vec3 tcPosition[];

#define ID gl_InvocationID

void main()
{
	tcPosition[ID] = tPosition[ID];
	if(ID == 0) {
		gl_TessLevelInner[0] = 3.0;
	    gl_TessLevelOuter[0] = 2.0;
	    gl_TessLevelOuter[1] = 2.0;
	    gl_TessLevelOuter[2] = 2.0;
	}
}

