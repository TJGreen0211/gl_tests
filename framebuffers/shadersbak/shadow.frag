#version 410

in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{   
    FragColor = vec4(vec3(1.0 - texture(screenTexture, texCoords)), 1.0);
    //FragColor = vec4(0.0, 1.0, 1.0, 1.0);
    
}
