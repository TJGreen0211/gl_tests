#version 410

in vec3 fN;
in vec3 fL;
in vec3 fE;
in vec3 fH;
out vec4 FragColor;

void main()
{	
	vec4 AmbientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 DiffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 SpecularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.5, 0.5, 0.5, 1.0);
	float Shininess = 25.0;
    vec3 N = normalize(fN);
    
    float Kd = max(dot(fL, N), 0.0);
    float Ks = pow(max(dot(N, fH), 0.0), Shininess);
    
    vec3 ambient = AmbientProduct.xyz;
    vec3 diffuse = Kd * DiffuseProduct.xyz;
    vec3 specular = Ks * SpecularProduct.xyz;

    // discard the specular highlight if the light's behind the vertex
    if( dot(fL, N) < 0.0 ) {
		specular = vec3(0.0, 0.0, 0.0);
    }
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
