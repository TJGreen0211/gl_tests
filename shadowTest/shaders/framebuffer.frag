#version 410

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D texture1;
uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main() {
	float depthValue = texture(texture1, texCoords).r;
	//FragColor = vec4(vec3(linearizeDepth(depthValue) / farPlane)*vec3(1.0, 0.5, 0.2), 1.0);
	//FragColor = vec4(vec3(depthValue), 1.0); //orthographic
	//FragColor = vec4(depthValue/(depthValue+1.0), 1.0, 1.0, 1.0);
	vec3 fColor = vec3(texture(texture1, texCoords));
	FragColor = vec4(fColor, 1.0);
}