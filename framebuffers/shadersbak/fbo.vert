#version 410

in vec4 vPosition;
in vec2 vTexCoords;

out vec2 texCoords;

void main()
{	
	gl_Position = vec4(vPosition.x, vPosition.y, 0.0, 1.0);
	texCoords = vTexCoords;
	//FragColor = vec4(0.0, 1.0, 1.0, 1.0);
}