#version 410

in vec2 fTexCoords;
in vec3 fE;
in vec3 fN;
in vec3 fL;
in vec3 fH;
in vec3 worldPos;

out vec4 FragColor;

uniform sampler2D texture1;

vec3 albedo = vec3(0.5, 0.1, 0.0);
float metallic = 0.9;
float roughness = 0.4;
float ao = 1.0;

const float PI = 3.14159265359;

float distributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;
	
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
	float r = roughness + 1.0;
	float k = (r*r) / 8.0;
	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness); 
	float ggx1 = geometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{   
	vec4 lightPos = vec4(0.0, 70.0, 0.0, 1.0);
	vec3 F0 = vec3(0.04);
	vec3 Lo = vec3(0.0);
	F0 = mix(F0, albedo, metallic);
	
	//per light radiance
	float dist = length(worldPos - lightPos.xyz);
	float attenuation = 1.0 / (dist * dist);
	vec3 radiance = vec3(0.9, 0.7, 0.5) * attenuation;
	
	//Cook Torrance BRDF
	float NDF = distributionGGX(fN, fH, roughness);
	float G = geometrySmith(fN, fE, fL, roughness);
	vec3 F = fresnelSchlick(max(dot(fH, fE), 0.0), F0);
	
	vec3 numerator = NDF * G * F;
	float denominator = 4 * max(dot(fN, fE), 0.0) * max(dot(fN, fL), 0.0) + 0.001;
	vec3 specular = numerator / denominator;
	
	FragColor = vec4(vec3(G), 1.0);
	
	vec3 Ks = F;
	vec3 Kd = vec3(1.0) - Ks;
	//Kd *= 1.0 - metallic;
	float NdotL = max(dot(fN, fL), 0.0);
	
	Lo += (Kd * albedo / PI * specular) * radiance * NdotL;

	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;
	//color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));


	//FragColor = vec4(color, 1.0);//vec4(ambient+diffuse+specular, 1.0);
}
