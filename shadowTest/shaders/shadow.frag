#version 410

in vec2 texCoords;
in vec3 fE;
in vec3 fN;
in vec3 fL;
in vec3 fH;
in vec4 fLightSpace;

out vec4 FragColor;

uniform sampler2D texture1;

float shadowCalculation(vec4 fLight) {
	vec3 projCoords = fLight.xyz/fLight.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(texture1, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	return shadow;
}

void main()
{   
	vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.5, 0.5, 0.5, 1.0);
	float shininess = 1.0;
	
	float Kd = max(dot(fL, fN), 0.0);
	float Ks = pow(max(dot(fN, fH), 0.0), shininess);
	vec3 color = vec3(texture(texture1, texCoords));
	vec3 ambient = ambientProduct.xyz * color;
	vec3 diffuse = Kd * diffuseProduct.xyz * color;
	vec3 specular = Ks * specularProduct.xyz;
	if(dot(fL, fN) < 0.0) {
		specular = vec3(0.0, 0.0, 0.0);
	}

	float shadow = shadowCalculation(fLightSpace);
	vec3 lighting = (ambient+(1.0-shadow) * (diffuse+specular));
	FragColor = vec4(lighting, 1.0);
}
