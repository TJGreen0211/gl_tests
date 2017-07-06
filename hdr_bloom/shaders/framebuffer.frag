#version 410

out vec4 FragColor;

in vec2 fTexCoords;

uniform sampler2D hdrBuffer;
uniform int hdr;

void main() {
	float exposure = 1.0;
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrBuffer, fTexCoords).rgb;
	if(hdr == 1) {
		//reinhard = (vec3 result = hdrColor/(hdrColor + vec3(1.0));)
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		result = pow(result, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
	}
	else { 
		vec3 result = pow(hdrColor, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
	}
}