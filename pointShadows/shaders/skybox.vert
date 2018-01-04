#version 410

in vec4 vPosition;
out vec3 TexCoords;

uniform mat4 ModelView;
uniform mat4 projection;

void main()
{
	vec4 pos = projection*ModelView*vPosition;
    gl_Position = pos.xyww;
    TexCoords = vPosition.xyz;
}