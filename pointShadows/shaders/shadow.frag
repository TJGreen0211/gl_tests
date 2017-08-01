#version 410

in vec2 texCoords;
in vec3 fE;
in vec3 fN;
in vec3 fL;
in vec3 fH;

out vec4 FragColor;

uniform float farPlane;
uniform vec3 lightPos;
uniform sampler2D texture1;
uniform samplerCube depthMap;

float shadowCalculation() {
	/*float bias = max(0.9 * (1.0 - dot(fN, fL)), 0.5);
	vec3 projCoords = fLight.xyz/fLight.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - 0.0001 > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	
	if(projCoords.z > 1.0)
		shadow = 0.0;
	return shadow;*/
	
	vec3 fragToLight = fE - lightPos;
	float closestDepth = texture(depthMap, fragToLight).r;
	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);
	float bias = max(0.9 * (1.0 - dot(fN, fL)), 0.5);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	FragColor = vec4(vec3(closestDepth / farPlane), 1.0);  
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

	float shadow = shadowCalculation();
	vec3 lighting = (ambient + (1.0 - shadow) *(diffuse+specular));
	//FragColor = vec4(lighting, 1.0);
}
