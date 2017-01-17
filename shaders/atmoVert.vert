#version 410

in vec4 vPosition;
in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPosition;
uniform float fInnerRadius;
uniform float fOuterRadius;

const float PI = 3.14159265359;
vec3 invWaveLength = vec3(0.3, 0.7, 1.0);
vec3 lightPos = normalize(vec3(0.0, 0.0, 1.0));
float K_R = 0.166;
float K_M = 0.0025;
float E = 14.3;
float scaleOver = 4.0 / (fOuterRadius - fInnerRadius);
float fScale = 1.0 / (fOuterRadius - fInnerRadius);
int numInScatter = 10;
float fNumInScatter = 10.0;
int numOutScatter = 10;
float fNumOutScatter = 10.0;

out vec3 color;
out vec3 secondaryColor;
out vec3 direction;
out vec3 testFrag;

float scale(float fCos)	
{	
	float x = 1.0 - fCos;
	return 0.25 * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));	
}

vec3 rayDir(vec3 cam) {
	vec3 ray = (model*vPosition).xyz - cam;
	return normalize(ray);
}

vec2 rayIntersection(vec3 p, vec3 dir, float r) {
	float b = dot( p, dir );
	float c = dot( p, p ) - r * r;
	
	float d = b * b - c;
	if ( d < 0.0 ) {
		return vec2( 10000, -10000 );
	}
	d = sqrt( d );
	
	float near = -b - d;
	float far = -b + d;
	
	return vec2(near, far);
}

float density(vec3 p) {
	return exp(-(length(p) - fInnerRadius) * scaleOver);
}

float optic(vec3 p, vec3 q) {
	vec3 step = (q - p) / fNumOutScatter;
	vec3 v = p + step * 0.5;
	
	float sum = 0.0;
	for(int i = 0; i < numOutScatter; i++) {
		sum += density(v);
		v += step;
	}
	sum *= length(step)*fScale;
	return sum;
}

void main()
{	
	/*float len = (e.y - e.x) / fNumInScatter;
	vec3 step = dir * len;
	vec3 p = o + dir * e.x;
	vec3 v = p + dir * (len * 0.5);
	
	vec3 sum = vec3(0.0);
	for(int i = 0; i < numInScatter; i++) {
		vec2 f = rayIntersection(v, l, fOuterRadius);
		vec3 u = v + l * f.y;
		float n = (optic(p, v) + optic(v, u))*(PI * 4.0);
		sum += density(v)* exp(-n * ( K_R * C_R + K_M ));
		v += step;
	}*/
	
	vec3 camPosition = vec3(1.0, 0.0, 1000.0);
	
	vec3 ray = rayDir(camPosition);
	vec2 nearT = rayIntersection(camPosition, ray, fOuterRadius);
	float near = nearT.x;
	
	vec3 start = camPosition + ray * near;
	float far = length(ray);
	far -= near;
	float startAngle = dot(ray, start) / fOuterRadius;
	float startDepth = exp(-1.0 / 0.25);
	float startOffset = startDepth*scale(startAngle);
	
	float sampleLength = (nearT.y-nearT.x) / fNumInScatter;
	float scaledLength = sampleLength * fScale;
	vec3 sampleRay = ray * sampleLength;
	vec3 p = camPosition + ray * near;
	vec3 samplePoint = p + ray * (sampleLength * 0.5);
	
	vec3 frontColor = vec3(0.0);
	for(int i = 0; i < numInScatter; i++) 
	{
		vec2 f = rayIntersection(samplePoint, lightPos, fOuterRadius);
		vec3 u = samplePoint + lightPos * f.y;
		float n = (optic(camPosition, samplePoint) + optic(samplePoint, u))*(PI * 4.0);
		frontColor += density(samplePoint)* exp(-n * ( K_R * invWaveLength + K_M ));
		samplePoint += sampleRay;
		
		if(isnan(n) || isinf(n))
			testFrag = vec3(1.0, 0.0, 0.0);
		else if(n == 0.0)
			testFrag = vec3(0.0, 0.0, 1.0);
		else
			testFrag = vec3(0.0, 1.0, 0.0);
	}
	
	secondaryColor = frontColor * K_M*E;
	color = frontColor * (invWaveLength * K_R*E);
	direction = camPosition - (model*vPosition).xyz;
	
	gl_Position = projection*view*model*vPosition;
}