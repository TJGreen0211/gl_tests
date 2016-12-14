#version 410

in vec4 vPosition;
in vec3 vNormal;

out vec3 fN;
out vec3 fE;
out vec3 fL;
out vec3 fH;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPosition;
uniform float time;

mat3 rot3xy( vec2 angle ) {
	vec2 c = cos( angle );
	vec2 s = sin( angle );
	
	return mat3(
		c.y      ,  0.0, -s.y,
		s.y * s.x,  c.x,  c.y * s.x,
		s.y * c.x, -s.x,  c.y * c.x
	);
}

void main()
{	
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec4 LightPosition = vec4(0.0, 0.0, 1.0, 1.0);
	mat3 rot = rot3xy( vec2( 0.0, time * 0.5 ) );
	//LightPosition = vec4(LightPosition.xyz * rot, 1.0);
	mat4 ModelView = view*model;
	gl_Position = projection*ModelView*vPosition;
	
	vec4 LP = (LightPosition-model*vPosition);
	vec3 vertexPosition = (ModelView*vPosition).xyz;
	vec3 lightDir = (LP-vPosition).xyz;
	//lightDir = lightDir * rot;
	
    fN = normalMatrix*vNormal;
    fL = vPosition.xyz;

	if( LightPosition.w != 0.0 ) {
		fL = normalize(lightDir);//LightPosition.xyz - vPosition.xyz;
    }

	fE = normalize(vertexPosition);
	
	vertexPosition = normalize(vertexPosition);
	
	vec3 halfVector = normalize(LP.xyz + fL);
	fH = halfVector;
}