#version 410

out vec4 FragColor;

//layout (location = 2) in vec2 vTexCoords;
/*in vec3 gPatchDistance;
in vec3 gTriDistance;
in vec2 fTexCoords;
*/

in vec3 teNormal;
in vec3 tePosition;

uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPosition;
uniform vec3 camPosition;

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
	vec4 ray = normalize(model*vec4(tePosition,1.0) - vec4(camPosition, 1.0));
	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec4 lightPos = vec4(lightPosition, 1.0);
	vec3 lightDir = normalize(vec4(tePosition,1.0)*model - lightPos).xyz;
	vec3 fragPos = vec3(vec4(tePosition,1.0)*model).xyz;
	vec3 fE = -normalize(vec4(tePosition,1.0)*model).xyz;
	vec3 fN = normalize(teNormal*normalMatrix);
	vec3 fL = -normalize(lightDir);
	vec3 fH = -normalize((vec4(tePosition,1.0)*model - lightPos) + ray).xyz;
	
	vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(10.5, 10.5, 10.5, 1.0);
	float shininess = 25.0;
	
	float Kd = max(dot(fL, fN), 0.0);
	float Ks = pow(max(dot(fN, fH), 0.0), shininess);
	vec3 ambient = ambientProduct.xyz;
	vec3 diffuse = Kd * diffuseProduct.xyz;
	vec3 specular = Ks * specularProduct.xyz;
	
	if(dot(fL, fN) < 0.0) {
		specular = vec3(0.0, 0.0, 0.0);
	}
	//vec3 lighting = (ambient + (1.0 - shadow) *(diffuse+specular));

	//Tessellation Geometry Visualization
	//float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
    //float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
	//color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;

    FragColor = vec4(vec3(ambient + diffuse + specular), 1.0);
}
