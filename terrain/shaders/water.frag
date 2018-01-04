#version 410

in vec4 fLightSpace;
in vec3 fragPos;
in vec3 fN;
in vec3 fT;
in vec3 fB;
in vec3 lightDir;
in vec3 vColor;
in vec2 fTexCoords;

out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 cameraPos;
uniform sampler2D texture1;
uniform sampler2D depthMap;
uniform sampler2D noiseTexture;

float shadowCalculation(vec4 fLight) {
	vec3 fragToLight = fragPos - lightPosition;
	//fragToLight = fLight.xyz * 0.5 + 0.5;
	//fragToLight = fragToLight/700.0;
	float closestDepth = texture(depthMap, vec2(fLight.xyz * 0.5 + 0.5)).r;
	float currentDepth = length(fragToLight)/700.0;
	//closestDepth *= 700.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(depthMap, vec2(fLight.xyz * 0.5 + 0.5) + vec2(x, y) * texelSize).r;
			shadow += currentDepth - 0.02 > pcfDepth ? 1.0 : 0.0;
		}
	}

	return shadow /= 9.0;
}

void main()
{
	/*vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.9, 0.9, 0.9, 1.0);
	float shininess = 250.0;

	vec3 v;
	vec3 ray = normalize(fragPos - cameraPos);
	float dist = length(fragPos - cameraPos);

	v.x = dot(lightDir, fT);
	v.y = dot(lightDir, fB);
	v.z = dot(lightDir, fN);
	vec3 fL = -normalize(v);

	vec3 halfVector = vec3((fragPos - lightPosition) + ray).xyz;
	v.x = dot(halfVector, fT);
	v.y = dot(halfVector, fB);
	v.z = dot(halfVector, fN);
	vec3 fH = -normalize(v);

	float Kd = max(dot(fL, waterNorm), 0.0);
	float Ks = pow(max(dot(waterNorm, fH), 0.0), shininess);

	vec3 ambient = ambientProduct.xyz;
	vec3 diffuse = Kd * diffuseProduct.xyz;
	vec3 specular = Ks * specularProduct.xyz;
	if(dot(fL, waterNorm) < 0.0) {
		specular = vec3(0.0, 0.0, 0.0);
	}

	float shadow = shadowCalculation(fLightSpace);
	vec3 finalColor = ambient+(1.0-shadow)*(diffuse+specular)*vec3(.3, .5, .5);*/

	vec3 tex1 = vec3(texture(noiseTexture, fTexCoords));
	FragColor = vec4(vColor, 1.0);
	//FragColor = vec4(waterNorm, 1.0);
}
