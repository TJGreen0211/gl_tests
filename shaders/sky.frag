# version 410

uniform vec4 time;
uniform vec3 camPosition;
uniform float sWidth;
uniform float sHeight;

in vec3 fPosition;
in mat3 m;

out vec4 FragColor;

const float PI = 3.14159265359;
const float degToRad = PI / 180.0;
const float MAX = 10000.0;

float K_R = 0.166;
const float K_M = 0.0025;
const float E = 14.3;
const vec3 C_R = vec3(0.3, 0.7, 1.0);
const float G_M = -0.85;

uniform float fInnerRadius;
uniform float fOuterRadius;

//float fInnerRadius = 0.4;
//float fOuterRadius = 0.5;
float SCALE_H = 4.0 / (fOuterRadius - fInnerRadius);
float SCALE_L = 1.0 / (fOuterRadius - fInnerRadius);

const int numOutScatter = 10;
const float fNumOutScatter = 10.0;
const int numInScatter = 10;
const float fNumInScatter = 10.0;

mat3 rot3xy( vec2 angle ) {
	vec2 c = cos( angle );
	vec2 s = sin( angle );
	
	return mat3(
		c.y      ,  0.0, -s.y,
		s.y * s.x,  c.x,  c.y * s.x,
		s.y * c.x, -s.x,  c.y * c.x
	);
}

vec3 ray_dir( float fov, vec2 size, vec2 pos ) {
	vec2 xy = pos - size * 0.5;

	float cot_half_fov = tan( ( 90.0 - fov * 0.5 ) * degToRad );	
	float z = size.y * 0.5 * cot_half_fov;
	
	return normalize( vec3( xy, -z ) );
}

vec3 rayDirection(vec3 camPosition) {
	vec3 ray = fPosition - camPosition;
	float far = length(ray);
	return ray /= far;
}

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


float density(vec3 p) {
	return exp(-(length(p) - fInnerRadius) * SCALE_H);
}

float optic(vec3 p, vec3 q) {
	vec3 step = (q - p) / fNumOutScatter;
	vec3 v = p + step * 0.5;
	
	float sum = 0.0;
	for(int i = 0; i < numOutScatter; i++) {
		sum += density(v);
		v += step;
	}
	sum *= length(step)*SCALE_L;
	return sum;
}

vec3 inScatter(vec3 o, vec3 dir, vec3 l) {
	
	float len = length(rayDirection(vec3(camPosition.x, 0.0, camPosition.z))) / fNumInScatter;
	vec3 step = dir * len;
	vec3 p = o;
	vec3 v = p;
	
	vec3 sum = vec3(0.0);
	for(int i = 0; i < numInScatter; i++) {
		//vec2 f = rayIntersection(v, l, fOuterRadius);
		vec3 u = camPosition;
		float n = (optic(p, v) + optic(v, u))*(PI * 4.0);
		sum += density(v)* exp(-n * ( K_R * C_R + K_M ));
		v += step;
	}
	sum *= len * SCALE_L;
	float c = dot(dir, -l);
	float cc = c * c;
	return sum * ( K_R * C_R * rayleighPhase( cc ) + K_M * miePhase( G_M, c, cc ) ) * E;
}

void main (void)
{
	//vec3 camPosition = inverse(mat3(v))*camPosition;//vec3(500.0, 0.0, 0.0);
	//vec3 dir = rayDirection(vec3(camPosition.x, 0.0, camPosition.z));
	vec3 dir = ray_dir( 45.0, vec2(sWidth, sHeight), gl_FragCoord.xy );
	vec3 eye = vec3(camPosition.x, 0.0, camPosition.z);
	
	vec3 l = normalize(vec3(0.0, 0.0, 1.0));
	
	mat3 rot = rot3xy( vec2( 0.0, time * 0.5 ) );
	//dir = rot * dir;
	//eye = rot * eye;
	l = rot * l;
	
	/*vec2 e = rayIntersection(eye, dir, fOuterRadius);
	if ( e.x > e.y ) {
		//discard;
	}
	vec2 f = rayIntersection(eye, dir, fInnerRadius);
	e.y = min(e.y, f.x);*/
	
	vec3 I = inScatter(eye, dir, l);
	
	FragColor = vec4(I, 1.0);
}