#version 410

in vec2 texCoords;
in vec3 fE;
in vec3 fN;
in vec3 fL;
in vec3 fH;

out vec4 FragColor;

uniform sampler2D texture1;

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

	FragColor = vec4(ambient+diffuse+specular, 1.0);
}
