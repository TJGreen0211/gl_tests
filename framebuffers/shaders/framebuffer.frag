#version 410

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D texture1;

void main() {
	vec3 fColor = vec3(texture(texture1, texCoords));
	vec3 vColor = vec3(1.0, 0.5, 0.2);
	FragColor = vec4(fColor*vColor, 1.0);
}