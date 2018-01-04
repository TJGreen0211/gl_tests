#version 410

in vec2 fTexCoords;
in vec3 fragPos;
in vec3 fN;
in vec3 lightDir;
in vec3 fH;
in vec3 fT;
in vec3 fB;
in vec4 fLightSpace;

out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 cameraPos;
uniform float time;
uniform sampler2D texture1;
uniform sampler2D depthMap;
uniform sampler2D noiseTexture;

const float PI	 	= 3.141592;
// sea
const float SEA_HEIGHT = 0.6;
const float SEA_CHOPPY = 4.0;
const float SEA_SPEED = 100.8;
const float SEA_FREQ = 0.16;
const vec3 SEA_BASE = vec3(0.1,0.19,0.22);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6);
#define SEA_TIME (1.0 + time * SEA_SPEED)
#define EPSILON_NRM (0.1 / 1024.0)
const mat2 octaveMat = mat2(1.6,1.2,-1.2,1.6);

/*
fLight.xyz = fLight.xyz * 0.5 + 0.5;
float closestDepth = texture(depthMap, fLight.xy).r;
float currentDepth = fLight.z;
float shadow = 0.0;
vec2 texelSize = 1.0 / textureSize(depthMap, 0);
for(int x = -1; x <= 1; ++x) {
	for(int y = -1; y <= 1; ++y) {
		float pcfDepth = texture(depthMap, fLight.xy + vec2(x, y) * texelSize).r;
		shadow += currentDepth - 0.0001 > pcfDepth ? 1.0 : 0.0;
	}
}
shadow /= 9.0;

if(fLight.z > 1.0)
	shadow = 0.0;
return shadow;
*/

float shadowCalculation(vec4 fLight) {
	vec3 fragToLight = fragPos - lightPosition;
	//fragToLight = fLight.xyz * 0.5 + 0.5;
	//fragToLight = fragToLight/700.0;
	float closestDepth = texture(depthMap, vec2(fLight.xyz * 0.5 + 0.5)).r;
	float currentDepth = length(fragToLight)/700.0;
	//closestDepth *= 700.0;
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(depthMap, vec2(fLight.xyz * 0.5 + 0.5) + vec2(x, y) * texelSize).r;
			shadow += currentDepth - 0.02 > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	
	
	//shadow = currentDepth - 0.02 > closestDepth ? 1.0 : 0.0;
	//if(fragToLight.z > 1.0)
	//	shadow = 0.0;
	
	return shadow;
}

float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

float seaOctave(vec2 uv, float choppy) {
	uv += noise(uv);
	vec2 wv = 1.0-abs(sin(uv));
	vec2 swv = abs(cos(uv));
	wv = mix(wv, swv, wv);
	return pow(1.0-pow(wv.x * wv.y, 0.65), choppy);
}

float map(vec3 p, int iter) {
	float freq = SEA_FREQ;
	float amp = SEA_HEIGHT;
	float choppy = SEA_CHOPPY;
	vec2 uv = p.xz; uv.x *= 0.75;
	
	float d, h = 0.0;
	for(int i = 0; i < iter; i++) {
		d = seaOctave((uv+SEA_TIME)*freq, choppy);
		d += seaOctave((uv-SEA_TIME)*freq, choppy);
		h += d * amp;
		uv *= octaveMat; freq *= 1.9; amp *= 0.22;
		choppy = mix(choppy, 1.0, 0.2);
	}
	return p.y - h;
}

float heightMapTracing(vec3 eye, vec3 dir, out vec3 p) {
	float tm = 0.0;
	float tx = 1000.0;
	float hx = map(eye + dir * tx, 3);
	if(hx > 0.0)
		return tx;
	float hm = map(eye + dir * tm, 3);
	float tmid;
	for(int i = 0; i < 8; i++) {
		tmid = mix(tm, tx, hm/(hm-hx));
		p = eye + dir * tmid;
		float hmid = map(p, 3);
		if(hmid < 0.0) {
			tx = tmid;
			hx = hmid;
		} else {
			tm = tmid;
			hm = hmid;
		}
	}
	return tmid;
}

vec3 getNormal(vec3 p, float eps) {
	vec3 n;
	n.y = map(p, 5);    
    n.x = map(vec3(p.x+eps,p.y,p.z), 5) - n.y;
    n.z = map(vec3(p.x,p.y,p.z+eps), 5) - n.y;
    n.y = eps;
    return normalize(n);
}

// lighting
float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}
float specular(vec3 n,vec3 l,vec3 e,float s) {    
    float nrm = (s + 8.0) / (PI * 8.0);
    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

// sky
vec3 getSkyColor(vec3 e) {
    e.y = max(e.y,0.0);
    return vec3(pow(1.0-e.y,2.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4);
}


vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {
	float fresnel = clamp(1.0 - dot(n, -eye), 0.0, 1.0);
	fresnel = pow(fresnel, 3.0) * 0.65;
	
	vec3 reflected = getSkyColor(reflect(eye,n)); 
	vec3 refracted = SEA_BASE + diffuse(n, l, 80.0) * SEA_WATER_COLOR * 0.12;
	vec3 color = mix(refracted, reflected, fresnel);
	float atten = max(1.0 - dot(dist, dist) * 0.001, 0.0);
	color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
	
	color += vec3(specular(n, l, eye, 60.0));
	
	return color;
}

void main()
{   
	vec3 color = vec3(0.1, 0.3, 1.0);
	vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.5, 0.5, 0.5, 1.0);
	float shininess = 25.0;
	
	vec3 v;
	vec3 ray = normalize(fragPos - cameraPos);
	
	v.x = dot(lightDir, fT);
	v.y = dot(lightDir, fB);
	v.z = dot(lightDir, fN);
	vec3 fL = -normalize(lightDir);
	
	vec3 halfVector = vec3((fragPos - lightPosition) + ray).xyz;
	v.x = dot(halfVector, fT);
	v.y = dot(halfVector, fB);
	v.z = dot(halfVector, fN);
	vec3 fH = -normalize(halfVector);
	
	//vec3 N = normalize(2.0*vec3(texture(noiseTexture, fTexCoords))-1.0);
	float Kd = max(dot(fL, fN), 0.0);
	float Ks = pow(max(dot(fN, fH), 0.0), shininess);
	//vec3 color = vec3(texture(texture1, fTexCoords));
	vec3 ambient = ambientProduct.xyz * color.rgb;
	vec3 diffuse = Kd * diffuseProduct.xyz * color.rgb;
	vec3 specular = Ks * specularProduct.xyz;
	if(dot(fL, fN) < 0.0) {
		specular = vec3(0.0, 0.0, 0.0);
	}
	
	vec3 eye = -cameraPos;
	vec3 p;
	heightMapTracing(eye, ray, p);
	vec3 dist = p - eye;
	vec3 n = getNormal(p, dot(dist,dist) * EPSILON_NRM);
	vec3 light = normalize(vec3(0.0,1.0,0.8)); 
	vec3 waterColor = getSeaColor(p,n,light,ray,dist);

	vec3 lighting = (ambient * (diffuse+specular));
	float gamma = 2.2;
	float shadow = shadowCalculation(fLightSpace);
	FragColor = vec4(pow(ambient+(1.0-shadow)*(diffuse+specular), vec3(1.0/gamma)), 1.0);
	//vec3 lighting = (ambient+(1.0-shadow) * (diffuse+specular));
	//FragColor = vec4(vec3(n), 1.0);
	FragColor = vec4(pow(waterColor, vec3(0.75)), 1.0);
}
