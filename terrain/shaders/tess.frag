#version 410

out vec4 FragColor;

//layout (location = 2) in vec2 vTexCoords;
/*in vec3 gPatchDistance;
in vec3 gTriDistance;
in vec2 fTexCoords;
*/

in vec2 longlat;
in vec3 teNormal;
in vec3 tePosition;
in vec3 teTangent;
in vec3 teN;
in vec3 teShadow;
in vec4 teLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPosition;
uniform vec3 camPosition;

uniform sampler2D texture1;
uniform sampler2D depthMap;
uniform sampler2D normalTex;
uniform sampler2D displacementTex;

float shadowCalculation(vec4 fLight) {
	vec3 fragToLight = teShadow - lightPosition;
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
	shadow /= 9.0;

	return shadow;
}

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir) {
	float height = texture(displacementTex, texCoords).r;
	vec2 p = viewDir.xy / viewDir.z * (height *0.1);
	return texCoords - p;
}

void main()
{
	vec4 ray = normalize(model*vec4(tePosition,1.0) - vec4(camPosition, 1.0));
	//vec2 texCoords = parallaxMapping(longlat, ray.xyz);
	vec4 color = vec4(texture(texture1, longlat)).rgba;

	mat3 normalMatrix = transpose(inverse(mat3(model)));

	vec3 T = normalize(vec3(vec4(teTangent, 0.0) * model));
	vec3 N = normalize(vec3(vec4(teNormal, 0.0)) * normalMatrix);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	vec3 v;
	vec3 fragPos = vec3(vec4(tePosition,1.0)*model).xyz;
	vec4 lightPos = vec4(lightPosition, 1.0);
	vec3 lightDir = normalize(vec4(tePosition,1.0)*model - lightPos).xyz;
	v.x = dot(lightDir, T);
	v.y = dot(lightDir, B);
	v.z = dot(lightDir, N);
	vec3 fL = -normalize(v);

	/*vec3 vertexPosition = vec3(vec4(tePosition,1.0)*model).xyz;
	v.x = dot(vertexPosition, T);
	v.y = dot(vertexPosition, B);
	v.z = dot(vertexPosition, N);
	vec3 fE = -normalize(v);*/

	vec3 halfVector = vec3((vec4(tePosition, 1.0)*model - lightPos) + ray).xyz;
	v.x = dot(halfVector, T);
	v.y = dot(halfVector, B);
	v.z = dot(halfVector, N);
	vec3 fH = -normalize(v);

	vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(10.5, 10.5, 10.5, 1.0);
	float shininess = 250.0;

	//vec3 N = vec3(texture(normalTex, texCoords));
	//N = normalize(N * 2.0 - 1.0);

	float Kd = max(dot(fL, teN), 0.0);
	float Ks = pow(max(dot(teN, fH), 0.0), shininess);
	vec3 ambient = ambientProduct.xyz *color.rgb;
	vec3 diffuse = Kd * diffuseProduct.xyz * color.rgb;
	vec3 specular = Ks * specularProduct.xyz;

	if(dot(fL, teN) < 0.0) {
		specular = vec3(0.0, 0.0, 0.0);
	}

	//Tessellation Geometry Visualization
	//float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
    //float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
	//color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;
	float shadow = shadowCalculation(teLightSpace);

	float gamma = 2.2;
    //FragColor = vec4(vec3(ambient + diffuse + specular), 1.0);
    FragColor = vec4(ambient+(1.0-shadow)*(diffuse+specular), color.a);
    //FragColor = vec4(teNormal, 1.0);
}
