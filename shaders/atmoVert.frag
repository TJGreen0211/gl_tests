#version 410

in vec3 color;
in vec3 secondaryColor;
in vec3 direction;
in vec3 testFrag;

vec3 lightPos = normalize(vec3(0.0, 0.0, 1.0));
float G_M = -0.85;
float exposure = 2.0;

out vec4 FragColor;

// Mie
// g : ( -0.75, -0.999 )
//      3 * ( 1 - g^2 )               1 + c^2
// F = ----------------- * -------------------------------
//      2 * ( 2 + g^2 )     ( 1 + g^2 - 2 * g * c )^(3/2)
float miePhase( float g, float c, float cc ) {
	float gg = g * g;
	
	float a = ( 1.0 - gg ) * ( 1.0 + cc );

	float b = 1.0 + gg - 2.0 * g * c;
	b *= sqrt( b );
	b *= 2.0 + gg;	
	
	return 1.5 * a / b;
}

// Reyleigh
// g : 0
// F = 3/4 * ( 1 + c^2 )
float rayleighPhase( float cc ) {
	return 0.75 * ( 1.0 + cc );
}

void main() {
	float c = dot(lightPos, direction) / length(direction);
	float cc = c*c;
	float rayleigh = rayleighPhase(cc);
	float mie = miePhase(G_M, c, cc);
	
	FragColor.rgb = 1.0 - exp(-exposure *(rayleigh*color+mie*secondaryColor));
	FragColor.a = 1.0;
	//FragColor = vec4(testFrag,1.0);
}