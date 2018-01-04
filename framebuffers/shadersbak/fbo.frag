#version 410

in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{   
    FragColor = texture(screenTexture, texCoords);
    //FragColor = vec4(0.0, 1.0, 1.0, 1.0);
    
}
