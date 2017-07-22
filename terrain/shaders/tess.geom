#version 410

uniform mat4 model;
uniform mat4 view;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];
in vec3 tePatchDistance[3];
in vec2 gTex[3];

out vec3 gPatchDistance;
out vec3 gTriDistance;
out vec2 fTexCoord;

void main()
{	
	gPatchDistance = tePatchDistance[0];
	gTriDistance = vec3(1, 0, 0);
	gl_Position = gl_in[0].gl_Position; EmitVertex();
	
	gPatchDistance = tePatchDistance[1];
	gTriDistance = vec3(0, 1, 0);
	gl_Position = gl_in[1].gl_Position; EmitVertex();
	
	gPatchDistance = tePatchDistance[2];
	gTriDistance = vec3(0, 0, 1);
	gl_Position = gl_in[2].gl_Position; EmitVertex();
	
	fTexCoord = gTex[0];
	
	EndPrimitive();
}