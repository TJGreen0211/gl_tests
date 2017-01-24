#version 410

uniform float time;
uniform vec3 camPosition;
uniform vec3 translation;

in vec4 fPosition;
in vec3 fNormal;
in mat4 m;
in mat4 v;

out vec4 FragColor;

const float PI = 3.14159265359;
const float degToRad = PI / 180.0;
const float MAX = 10000.0;

uniform float fInnerRadius;
uniform float fOuterRadius;

vec3 rayDirection(vec3 camPosition) {
	vec4 ray = m*fPosition - vec4(camPosition, 1.0);
	return normalize(vec3(ray));
}

float snoise(vec3 uv, float res)
{
	const vec3 s = vec3(1e0, 1e2, 1e3);
	
	uv *= res;
	
	vec3 uv0 = floor(mod(uv, res))*s;
	vec3 uv1 = floor(mod(uv+vec3(1.), res))*s;
	
	vec3 f = fract(uv); f = f*f*(3.0-2.0*f);

	vec4 v = vec4(uv0.x+uv0.y+uv0.z, uv1.x+uv0.y+uv0.z,
		      	  uv0.x+uv1.y+uv0.z, uv1.x+uv1.y+uv0.z);

	vec4 r = fract(sin(v*1e-1)*1e3);
	float r0 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	r = fract(sin((v + uv1.z - uv0.z)*1e-1)*1e3);
	float r1 = mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y);
	
	return mix(r0, r1, f.z)*2.-1.;
}

vec2 rayIntersection(vec3 p, vec3 dir, float radius ) {
	float b = dot( p, dir );
	float c = dot( p, p ) - radius * radius;
	
	float d = b * b - c;
	if ( d < 0.0 ) {
		return vec2( MAX, -MAX );
	}
	d = sqrt( d );
	
	float near = -b - d;
	float far = -b + d;
	
	return vec2(near, far);
}

void main (void)
{	
	mat4 modelmat = mat4(
		1.000000, 0.000000, 0.000000, translation.x, 
		0.000000, 1.00000, 0.000000, translation.y, 
		0.000000, 0.000000, 1.00000, translation.z, 
		0.000000, 0.000000, 0.000000, 1.000000);
	
	mat4 tv = transpose(modelmat*v);//[3].xyz * mat3(v);
	vec3 camPosition = vec3(-tv[3] * tv);//-transpose(mat3(v))*v[3].xyz;
	vec3 dir = rayDirection(camPosition);
	vec3 eye = camPosition;
	
	vec3 l = normalize(vec3(0.0, 0.0, 1.0));
	
	float color;
	
	for(int i = 1; i <= 7; i++)
	{
		float power = pow(2.0, float(i));
		color += snoise(vec3(0.0,-time*0.05, time*0.01), power);
	}
	
	FragColor = vec4(l += color, 1.0);
}