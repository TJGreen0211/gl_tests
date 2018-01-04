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

const float SEA_HEIGHT = 0.6;
const float SEA_CHOPPY = 4.0;
const float SEA_SPEED = 0.8;
const float SEA_FREQ = 0.16;
const vec3 SEA_BASE = vec3(0.1,0.19,0.22);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6);
float SEA_TIME;

uniform vec3 lightPosition;
uniform vec3 cameraPos;
uniform float time;
uniform sampler2D texture1;
uniform sampler2D depthMap;
uniform sampler2D noiseTexture;

float waterlevel = 70.0;
float wavegain   = 1.0;
float large_waveheight = 1.0;
float small_waveheight = 1.0;
const mat2 octaveMat = mat2(1.6,1.2,-1.2,1.6);

const float PI	 	= 3.141592;

float hash( vec2 p ) {
    float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}

float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
    vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( 
                mix( hash( i + vec2(0.0,0.0) ), hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), hash( i + vec2(1.0,1.0) ), u.x), 
                u.y);
}

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
	//shadow = currentDepth - 0.02 > closestDepth ? 1.0 : 0.0;
	//if(fragToLight.z > 1.0)
	//	shadow = 0.0;
	
	return shadow /= 9.0;
}


//Linear waves
float water(vec2 p) {
	float height = waterlevel;
	
	float t = time * 100.0;
	//Adds randomness to wave function
	vec2 shift1 = 0.001*vec2(t*160.0*2.0, t*120.0*2.0);
	vec2 shift2 = 0.001*vec2(t*190.0*2.0, -t*130.0*2.0 );
	
	float wave = 0.0;
	//Course waves
	wave += sin(p.x*0.021 + shift2.x)*4.5;
	wave += sin(p.x*0.0172+p.y*0.01 + shift2.x*1.121)*4.0;
	wave -= sin(p.x*0.00104+p.y*0.005 + shift2.x*0.121)*4.0;
	//Smaller waves
	wave += sin(p.x*0.02221+p.y*0.01233+shift2.x*3.437)*5.0;
	wave += sin(p.x*0.03112+p.y*0.01122+shift2.x*4.269)*2.5 ;
	wave *= large_waveheight;
	//wave -= fbm(p*0.004-shift2*.5)*small_waveheight*24.;
	
	float amp = 6.0*small_waveheight;
	shift1 *= 0.3;
	//Wave detail
	for(int i = 0; i < 7; i++) {
		wave -= abs(sin((noise(p*0.01+shift1)-.5)*3.14))*amp;
		amp *= 0.51;
		shift1 *= 1.841;
		p *= octaveMat*0.9331;
	}
	height += wave;
	return height;
}

/*

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
    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
    fresnel = pow(fresnel,3.0) * 0.65;
        
    vec3 reflected = getSkyColor(reflect(eye,n));    
    vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
    
    vec3 color = mix(refracted,reflected,fresnel);
    
    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    color += vec3(specular(n,l,eye,60.0));
    
    return color;
}*/

float seaOctave(vec2 uv, float choppy) {
	uv += noise(uv);
	vec2 wv = 1.0-abs(sin(uv));
	vec2 swv = abs(cos(uv));
	wv = mix(wv, swv, wv);
	return pow(1.0-pow(wv.x*wv.y, 0.65), choppy);
}

float map(vec3 p, int iterations) {
	float freq = SEA_FREQ;
	float choppy = SEA_CHOPPY;
	float height = SEA_HEIGHT;
	vec2 uv = p.xy;
	uv.x *= 0.75;
	float d, h = 0;
	for(int i = 0; i < iterations; i++) {
		d = seaOctave((uv+time*100.0)*freq, choppy);
		d += seaOctave((uv-time*100.0)*freq, choppy);
		h += d*height;
		uv *= octaveMat;
		freq *= 1.9;
		height *= 0.22;
		choppy = mix(choppy, 1.0, 0.2);
	}
	return p.z - h;
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
	n.z = map(p ,5);
	n.x = map(vec3(p.x+eps,p.y,p.z), 5) - n.z ;
	n.y = map(vec3(p.x,p.y+eps,p.z), 5) - n.z;
	n.z = eps;
	return normalize(n);	
}

vec2 rayIntersection(vec3 p, vec3 dir, float radius ) {
	float b = dot( p, dir );
	float c = dot( p, p ) - radius * radius;
	
	float d = b * b - c;
	if ( d < 0.0 ) {
		return vec2( 10000.0, -10000.0 );
	}
	d = sqrt( d );
	
	float near = -b - d;
	float far = -b + d;
	
	return vec2(near, far);
}

float rayMarch(vec3 eye, vec3 dir, vec3 fragPos) {
	vec3 point;
	float dist = 0.0;
	float tmin = 0.0;
	float tmax = 1000.0;
	for(int i = 0; i < 8; i++) {
		//tmin = x*(1-a)+y*a;
		vec3 p = eye+dir*tmin;
		float d = map(p,3);
		//Object hit
		if(d < 0.0) {
			dist = tmin;
			return dist;
		}
		tmin += tmax/8.0;
	}
	return dist;
}

void main()
{   
	float EPSILON_NRM	= 0.1/1024.0;
	//vec3 color = vec3(0.1, 0.3, 1.0);
	vec4 ambientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
	vec4 diffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
	vec4 specularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.5, 0.5, 0.5, 1.0);
	float shininess = 25.0;
	
	vec3 v, p;
	vec3 ray = normalize(fragPos - cameraPos);
	float dist = length(fragPos - cameraPos);
	//float dist = heightMapTracing(cameraPos,ray,p);
	//float dist = rayMarch(cameraPos, ray, fragPos);
	vec3 wpos = cameraPos + dist*ray;
	
	//vec2 f = rayIntersection(cameraPos, ray, 63.710);
    vec3 waterFrag = vec3(fTexCoords.xy*150.0, 1.0);
    vec3 waterNormal = getNormal(waterFrag, dist*EPSILON_NRM);
    
	v.x = dot(lightDir, fT);
	v.y = dot(lightDir, fB);
	v.z = dot(lightDir, fN);
	vec3 fL = -normalize(v);
	
	vec3 halfVector = vec3((fragPos - lightPosition) + ray).xyz;
	v.x = dot(halfVector, fT);
	v.y = dot(halfVector, fB);
	v.z = dot(halfVector, fN);
	vec3 fH = -normalize(v);
	
	float Kd = max(dot(fL, fN), 0.0);
	float Ks = pow(max(dot(fN, fH), 0.0), shininess);
	vec4 color = vec4(texture(texture1, fTexCoords)).rgba;
	
	//float Kd = max(dot(fL, waterNormal), 0.0);
	//float Ks = pow(max(dot(waterNormal, fH), 0.0), shininess);
	//vec3 color = vec3(texture(texture1, fTexCoords));
	//vec3 ambient = ambientProduct.xyz;
	//vec3 diffuse = SEA_BASE+ pow(dot(fL, waterNormal)*0.4+0.6, 80.0)*SEA_WATER_COLOR*0.12;//Kd * diffuseProduct.xyz;
	//float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
	//vec3 specular = vec3(Ks * (60.0 + 8.0) / (3.1415 * 8.0));
	//if(dot(fL, waterNormal) < 0.0) {
	//	specular = vec3(0.0, 0.0, 0.0);
	//}
	
	vec3 ambient = ambientProduct.xyz * color.rgb;
	vec3 diffuse = Kd * diffuseProduct.xyz * color.rgb;
	vec3 specular = Ks * specularProduct.xyz;
	if(dot(fL, fN) < 0.0) {
		specular = vec3(0.0, 0.0, 0.0);
	}

	//vec3 lighting = (ambient * (diffuse+specular));
	float gamma = 2.2;
	float shadow = shadowCalculation(fLightSpace);
	//vec3 color = pow(ambient+(1.0-shadow)*(diffuse+specular), vec3(1.0/gamma));
	//vec2 xdiff = vec2(0.1, 0.0)*1.0*4.;
    //vec2 ydiff = vec2(0.0, 0.1)*1.0*4.;
    //vec2 f = fTexCoords*1500.0;
	//vec3 a = normalize(vec3(water(f-xdiff) - water(f+xdiff), water(f-ydiff) - water(f+ydiff), 1.0));
	//FragColor = vec4(wpos, 1.0);
	//FragColor = vec4(color, 1.0);
	FragColor = vec4(pow(ambient+(1.0-shadow)*(diffuse+specular), vec3(1.0/gamma)), color.a);
}
