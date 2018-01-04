#version 410

uniform vec3 lightPosition;
in vec4 fragPos;

void main()
{   
	float farPlane = 700.0;
	float lightDistance = length(fragPos.xyz - lightPosition.xyz);
	lightDistance = lightDistance / farPlane;
	gl_FragDepth = lightDistance;
}
