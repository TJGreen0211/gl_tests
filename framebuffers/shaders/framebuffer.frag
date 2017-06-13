#version 410

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D texture1;

void main() {
	vec3 fColor = vec3(texture(texture1, texCoords));
	FragColor = vec4(fColor, 1.0);
}