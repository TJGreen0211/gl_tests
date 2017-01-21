#version 410

in vec3 fN; //normal
in vec3 fL; //lightDir
in vec3 fE; //viewDir
in vec3 fH; //halfwayDir
out vec4 FragColor;

void main()
{	
	vec4 AmbientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 DiffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 SpecularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.1, 0.1, 0.1, 1.0);
	float Shininess = 100.0;
    
    float Kd = max(dot(fL, fN), 0.0);
    float Ks = pow(max(dot(fN, fH), 0.0), Shininess);
    
    vec3 ambient = AmbientProduct.xyz;
    vec3 diffuse = Kd * DiffuseProduct.xyz;
    vec3 specular = Ks * SpecularProduct.xyz;

    // discard the specular highlight if the light's behind the vertex
    if( dot(fL, fN) < 0.0 ) {
		specular = vec3(0.0, 0.0, 0.0);
    }
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
