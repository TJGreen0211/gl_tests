#version 410

out vec4 FragColor;

//layout (location = 2) in vec2 vTexCoords;
in vec3 gPatchDistance;
in vec3 gTriDistance;
in vec2 fTexCoords;

in vec3 gE;
in vec3 gN;
in vec3 gL;
in vec3 gH;

uniform sampler2D texture1;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}


void main()
{
	vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(10.5, 10.5, 10.5, 1.0);
	float shininess = 25.0;
	
	//float Kd = max(dot(fL, fN), 0.0);
	//float Ks = pow(max(dot(fN, fH), 0.0), shininess);

	float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
    float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
	vec3 color = vec3(0.2, 0.6, 0.4);
	color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;
	//vec3 texColor = vec3(texture(texture1, fTexCoords));

    FragColor = vec4(color, 1.0);
}
