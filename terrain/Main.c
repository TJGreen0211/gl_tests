#include "Main.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

float zNear = 0.5, zFar = 100000.0;
int mousePosX, mousePosY, actionPress, keys, mouseZoomOffset;
GLuint depthMap, textureColorBuffer, textureColorBuffer3D, sunNoiseTexture;
struct sphere planet;
struct obj object, ship;
struct ring planetRing;
struct quadCube qc, qc2;
mat4 lightView;

GLuint generateDepthCubemap(int width, int height)
{
	GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

GLuint generateTextureAttachment3D(int depth, int stencil, vec2 size) {
	GLuint textureID;
	GLenum attachment_type;
	if(!depth && !stencil)
		attachment_type = GL_RGB;
	else if(depth && !stencil)
		attachment_type = GL_DEPTH_COMPONENT;
	else if(!depth && stencil)
		attachment_type = GL_STENCIL_INDEX;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	if(!depth && !stencil)
		glTexImage3D(GL_TEXTURE_3D, 0, attachment_type, getWindowWidth(), getWindowHeight(), 1, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	else if(depth && !stencil)
		glTexImage3D(GL_TEXTURE_3D, 0, attachment_type, getWindowWidth(), getWindowHeight(), 1, 0, attachment_type, GL_FLOAT, NULL);

	else
		glTexImage3D(GL_TEXTURE_3D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 1, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_BORDER
    glBindTexture(GL_TEXTURE_3D, 0);
	return textureID;
}

GLuint initInstanceBuffer(vec3 *vertices, int vertSize, vec3 *normals, int normSize, vec2 *texCoords, int texSize) {
	GLuint vbo, vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertSize+normSize+texSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normSize, normals);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize+normSize, texSize, texCoords);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return vao;
}

GLuint initInstanceShader() {
	GLuint shader;
	createShader(&shader, "shaders/instance.vert",
		"shaders/instance.frag");
	return shader;
}

GLuint initSkyShader() {
	GLuint shader;
	createShader(&shader, "shaders/sky.vert",
		"shaders/sky.frag");
	return shader;
}

GLuint initAtmosphereShader() {
	GLuint shader;
	createShader(&shader, "shaders/atmosphere.vert",
		"shaders/atmosphere.frag");
	return shader;
}

GLuint initLightingShader() {
	GLuint shader;
	createShader(&shader, "shaders/light.vert",
		"shaders/light.frag");
	return shader;
}

GLuint initWaterShader() {
	GLuint shader;
	createShader(&shader, "shaders/water.vert",
		"shaders/water.frag");
	return shader;
}

GLuint initDepthShader() {
	GLuint shader;
	createShader(&shader, "shaders/depth.vert",
		"shaders/depth.frag");

	return shader;
}

GLuint initFramebufferShader() {
	GLuint shader;
	createShader(&shader, "shaders/framebuffer.vert",
		"shaders/framebuffer.frag");
	return shader;
}

GLuint initTessShader() {
	GLuint shader;
	GLuint vertShader = LoadShader("shaders/tess.vert", GL_VERTEX_SHADER);
	GLuint tcshShader = LoadShader("shaders/tess.tcsh", GL_TESS_CONTROL_SHADER);
	GLuint teshShader = LoadShader("shaders/tess.tesh", GL_TESS_EVALUATION_SHADER);
	//GLuint geomShader = LoadShader("shaders/tess.geom", GL_GEOMETRY_SHADER);
	GLuint fragShader = LoadShader("shaders/tess.frag", GL_FRAGMENT_SHADER);
	shader = glCreateProgram();
	glAttachShader(shader, vertShader);
	glAttachShader(shader, tcshShader);
	glAttachShader(shader, teshShader);
	//glAttachShader(shader, geomShader);
	glAttachShader(shader, fragShader);
	glLinkProgram(shader);

	return shader;
}

vec3 *generateNormals(vec3 normals[], float *vertices, int size) {
	vec3 one, two;
	int c = 0;
	for(int i = 0; i < size; i+=9) {
		one.x = vertices[i+3] - vertices[i];
		one.y = vertices[i+4] - vertices[i+1];
		one.z = vertices[i+5] - vertices[i+2];

		two.x = vertices[i+6] - vertices[i+3];
		two.y = vertices[i+7] - vertices[i+4];
		two.z = vertices[i+8] - vertices[i+5];
		vec3 normal = normalizevec3(crossvec3(one, two));
		normals[c] = normal; c++;
		normals[c] = normal; c++;
		normals[c] = normal; c++;
	}

	return normals;
}

GLuint initBufferTangents(vec3 *vertices, int vertSize, vec3 *normals, int normSize, vec3 *tangents, int tanSize, vec2 *texCoords, int texSize) {
	GLuint vbo, vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertSize+normSize+texSize+tanSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normSize, normals);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize+normSize, texSize, texCoords);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize+normSize+texSize, tanSize, tangents);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize+texSize));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	return vao;
}

GLuint initBuffers(vec3 *vertices, int vertSize, vec3 *normals, int normSize, vec2 *texCoords, int texSize) {
	GLuint vbo, vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertSize+normSize+texSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normSize, normals);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize+normSize, texSize, texCoords);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return vao;
}

GLuint initFramebuffer(GLuint *textureID) {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	*textureID = generateTextureAttachment(0, 0, 1024.0, 512.0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *textureID, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024.0, 512.0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: Framebuffer is not complete");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}

GLuint initFramebuffer3D() {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	vec2 size = {1024.0, 512.0};
	textureColorBuffer = generateTextureAttachment3D(0, 0, size);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, getWindowWidth(), getWindowHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: Framebuffer is not complete");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}

GLuint initDepthbuffer() {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	depthMap = generateTextureAttachment(1, 0, getWindowWidth(), getWindowHeight());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	  if(status != GL_FRAMEBUFFER_COMPLETE)
		  printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}

GLuint initSubQuad() {
	int divisions = 200;
	float fdivisions = 200.0;

	GLuint vao;
	vec3 vertices[divisions*divisions*6];
	vec2 texCoords[divisions*divisions*6];

	vec3 start = {1.0, 1.0, -1.0};
	float offset = 2.0/(fdivisions);
	int index = 0;
	for(int i = 0; i < divisions; i++) {
		start.x = 1.0;
		for(int j = 0; j < divisions; j++) {
			vec3 face0 = {start.x,   	  start.y,   start.z};
			vec3 face1 = {start.x-offset, start.y-offset, start.z};
			vec3 face2 = {start.x,        start.y-offset, start.z};
			vec3 face3 = {start.x-offset, start.y,   start.z};

			vertices[index++] = face2;
			texCoords[index-1].x = (face2.x+1.0)/2.0;
			texCoords[index-1].y = (face2.y+1.0)/2.0;
			vertices[index++] = face1;
			texCoords[index-1].x = (face1.x+1.0)/2.0;
			texCoords[index-1].y = (face1.y+1.0)/2.0;
			vertices[index++] = face0;
			texCoords[index-1].x = (face0.x+1.0)/2.0;
			texCoords[index-1].y = (face0.y+1.0)/2.0;

			vertices[index++] = face1;
			texCoords[index-1].x = (face1.x+1.0)/2.0;
			texCoords[index-1].y = (face1.y+1.0)/2.0;
			vertices[index++] = face3;
			texCoords[index-1].x = (face3.x+1.0)/2.0;
			texCoords[index-1].y = (face3.y+1.0)/2.0;
			vertices[index++] = face0;
			texCoords[index-1].x = (face0.x+1.0)/2.0;
			texCoords[index-1].y = (face0.y+1.0)/2.0;

			start.x = start.x - offset;
		}
		start.y -= offset;
	}

    //*normArray = *generateNormals(normArray, vertices, numVertices);
    vao = initBuffers(vertices, sizeof(vertices), vertices, sizeof(vertices), texCoords, sizeof(texCoords));
    return vao;
}

GLuint initQuad() {
	GLuint vao;
	float vertices[] = {
		-1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f
	};

	float texCoords[] = {
		0.0f, 0.0f,
    	0.0f, 1.0f,
		1.0f, 1.0f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
	};
	int numVertices = (sizeof(vertices)/sizeof(vertices[0]));
    int numTexCoords = (sizeof(texCoords)/sizeof(texCoords[0]));
	int vecSize = numVertices/3;
	int texSize = numTexCoords/2;

    vec3 vertArray[vecSize];
    vec3 normArray[vecSize];
    vec2 texArray[texSize];
    int c = 0;
    for(int i = 0; i < numVertices; i+=3) {
    	vertArray[c].x = vertices[i];
    	vertArray[c].y = vertices[i+1];
    	vertArray[c].z = vertices[i+2];
    	c++;
    }
    c = 0;
    for(int i = 0; i < numTexCoords; i+=2) {
    	texArray[c].x = texCoords[i];
    	texArray[c].y = texCoords[i+1];
    	c++;
    }
    *normArray = *generateNormals(normArray, vertices, numVertices);
    vec3 vna[vecSize];
	*vna = *generateSmoothNormals(vna, vertArray, normArray, vecSize);
    vao = initBuffers(vertArray, sizeof(vertices), vna, sizeof(vertices), texArray, sizeof(texCoords));
    return vao;
}

GLuint initQuadCube(int divisions) {
	GLuint vao;
	createCube(divisions, &qc);
	vec2 texCoords[qc.vertexNumber];
	vec3 vna[qc.vertexNumber];
	vec3 tangent[qc.vertexNumber];
	*tangent = *generateTangents(qc.vertexNumber, qc.points, tangent);

	for(int i = 0; i < qc.vertexNumber; i++) {
    	texCoords[i].x = (atan2(qc.points[i].y, qc.points[i].x) / 3.1415926 + 1.0) * 0.5;
    	texCoords[i].y = asin(qc.points[i].z) / 3.1415926 + 0.5;
    }
    *vna = *generateSmoothNormals(vna, qc.points, qc.normals, qc.vertexNumber);
    //vao = initBuffers(qc.points, qc.size, vna, qc.nsize, texCoords, sizeof(texCoords[0])*qc.vertexNumber);
    vao = initBufferTangents(qc.points, qc.size, vna, qc.nsize, tangent, qc.nsize, texCoords, sizeof(texCoords[0])*qc.vertexNumber);

    return vao;
}

GLuint initObjectBuffer(char *path) {
	GLuint vao;
	ship = ObjLoadModel(path);
	vec3 vna[ship.vertexNumber];
	vec2 texCoords[ship.vertexNumber];
	*vna = *generateSmoothNormals(vna, ship.points, ship.normals, ship.vertexNumber);

	for(int i = 0; i < ship.vertexNumber; i++) {
    	texCoords[i].x = 1.0;
    	texCoords[i].y = 0.0;
    }
    vao = initBuffers(ship.points, ship.size, ship.normals, ship.nsize, texCoords, sizeof(texCoords[0])*ship.vertexNumber);

	return vao;
}

GLuint initRockBuffer(char *path) {
	GLuint vao;
	object = ObjLoadModel(path);
	vec3 vna[object.vertexNumber];
	vec2 texCoords[object.vertexNumber];
	*vna = *generateSmoothNormals(vna, object.points, object.normals, object.vertexNumber);

	for(int i = 0; i < object.vertexNumber; i++) {
    	texCoords[i].x = 1.0;
    	texCoords[i].y = 0.0;
    }
    vao = initInstanceBuffer(object.points, object.size, object.normals, object.nsize, texCoords, sizeof(texCoords[0])*object.vertexNumber);

	return vao;
}

GLuint initSphere() {
	planet = tetrahedron(5, &planet);
	GLuint vao;
    vec3 vna[planet.vertexNumber];
    vec2 texCoords[planet.vertexNumber];
    *vna = *generateSmoothNormals(vna, planet.points, planet.normals, planet.vertexNumber);

    for(int i = 0; i < planet.vertexNumber; i++) {
    	texCoords[i].x = (atan2(planet.points[i].y, planet.points[i].x) / 3.1415926 + 1.0) * 0.5;
    	texCoords[i].y = asin(planet.points[i].z) / 3.1415926 + 0.5;
    }

	vao = initBuffers(planet.points, planet.size, planet.normals, planet.nsize, texCoords, sizeof(texCoords[0])*planet.vertexNumber);
	return vao;
}

GLuint initRing() {
	GLuint vao;
	planetRing = createRing(1, 1.5, 2.0);
	vao = initBuffers(planetRing.points, planetRing.size, planetRing.normals, planetRing.nsize, planetRing.texCoords, planetRing.texsize);
	return vao;
}

vec4 getCameraPosition(mat4 position) {
	mat4 mvTranspose = transposemat4(multiplymat4(position, getViewPosition()));
	vec4 inverseCamera = {-mvTranspose.m[3][0], -mvTranspose.m[3][1], -mvTranspose.m[3][2], -mvTranspose.m[3][3]};
	vec4 camPosition = multiplymat4vec4(mvTranspose, inverseCamera);

	return camPosition;
}

vec4 getPositionModelspace(mat4 position, mat4 model) {
	mat4 mvTranspose = transposemat4(multiplymat4(position, model));
	vec4 inverseM = {-mvTranspose.m[3][0], -mvTranspose.m[3][1], -mvTranspose.m[3][2], -mvTranspose.m[3][3]};
	vec4 positionModelspace = multiplymat4vec4(mvTranspose, inverseM);

	return positionModelspace;
}

mat4 getLookAtMatrix(vec4 lightPosition, vec3 lookAt) {
	mat4 rxry;
	vec3 d = {lightPosition.x - lookAt.x, lightPosition.y - lookAt.y, lightPosition.z - lookAt.z};
	d = normalizevec3(d);
	float rad = 180.0 / M_PI;
	float lightYaw = asin(-d.y) * rad;
	float lightPitch = atan2(d.x, d.z) * rad;
	printf("Yaw: %f, Pitch: %f\n", lightYaw, lightPitch);
	/*if(lookAt.x == 1.0 || lookAt.z == -1.0) {
		//printf("%f, %f, %f\n", lookAt.x, lookAt.y, lookAt.z);
		//rxry = multiplymat4(rotateZ(180.0), multiplymat4(rotateX(lightYaw), rotateY(lightPitch)));
		rxry = multiplymat4(rotateY(lightPitch), multiplymat4(rotateZ(180.0), rotateX(lightYaw)));
	}
	else {
	vec3 s = {-1.0, 1.0, 1.0};
		rxry = multiplymat4( scalevec3(s), multiplymat4(rotateX(lightYaw), rotateY(lightPitch)));
	}*/
	rxry = multiplymat4(rotateX(lightYaw), rotateY(lightPitch));
	return multiplymat4(rxry, translatevec4(lightPosition));
}

void initMVP(int shader, mat4 m, mat4 v) {
	mat4 p = perspective(45.0, getWindowWidth()/getWindowHeight(), zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &p.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &m.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &v.m[0][0]);
}

void drawAtmosphere(GLuint VAO, GLuint shader, GLuint sky, int vertices, mat4 m, vec3 position, float scale, float scaleFactor, vec4 lightPosition) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	mat4 positionMatrix = translatevec3(position);
	vec4 camPosition = getCameraPosition(positionMatrix);

	if(pow(camPosition.x, 2.0) + pow(camPosition.y, 2.0) + pow(camPosition.z, 2.0) < pow(m.m[0][0], 2.0)) {
		glUseProgram(sky);
		glCullFace(GL_FRONT);
	}
	else
		glUseProgram(shader);

	initMVP(shader, m, getViewMatrix());

	float fOuter = scale*scaleFactor;
	float fInner = scale;
	vec3 C_R = {0.3, 0.7, 1.0};
	float E = 14.3;

	glUniform1f(glGetUniformLocation(shader, "fInnerRadius"), fInner);
	glUniform1f(glGetUniformLocation(shader, "fOuterRadius"), fOuter);
	glUniform3f(glGetUniformLocation(shader, "camPosition"), camPosition.x, camPosition.y, camPosition.z);
	glUniform3f(glGetUniformLocation(shader, "C_R"), C_R.x, C_R.y, C_R.z);
	glUniform1f(glGetUniformLocation(shader, "E"), E);
	glUniform1f(glGetUniformLocation(shader, "time"), glfwGetTime());

	glUniform3f(glGetUniformLocation(shader, "camPosition"), camPosition.x, camPosition.y, camPosition.z);
	glUniform3f(glGetUniformLocation(shader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);

	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
}

void drawInstanced(GLuint vao, GLuint vbo, GLuint shader, int vertexNumber, int drawAmount, vec3 *positions, mat4 *rotations, mat4 model, float *scaleArray, float theta, vec4 lightPosition) {
	vec4 cameraPos = getCameraPosition(model);

	/*for(int i = 0; i < numDraws; i++){
		float deg = i * degToRad;
		positions[i].x = cos(deg)*innerRad+(-(((float)rand()/(float)(RAND_MAX)) * width));
		positions[i].y = sin(deg)*innerRad+(-(((float)rand()/(float)(RAND_MAX)) * width));
		positions[i].z = -(((float)rand()/(float)(RAND_MAX)) * height);
	}*/

	vec3 translation[drawAmount];
	for(int i = 0; i < drawAmount; i++){

		translation[i].x = cos(scaleArray[i]+theta/85.0)*140.0+positions[i].x;
		translation[i].y = positions[i].y;
		translation[i].z = sin(scaleArray[i]+theta/85.0)*140.0+positions[i].z;
	}

	/*vec3 translation;
	translation.x = (65.0*1.5) * cos(theta/75.0);
	translation.y = 0.0;
	translation.z = (65.0*1.5) * sin(theta/75.0);*/
	vec3 center = {65.0, 0.0, 0.0};

	mat4 modelArr[vertexNumber];
	for(int i = 0; i < vertexNumber; i++) {
		//modelArr[i] = multiplymat4(multiplymat4(translatevec3(translation), rotations[i]), scale(scaleArray[i]/10.0));
		//modelArr[i] = translate(pos[i].x+65.0/2.0, pos[i].y, pos[i].z);
		modelArr[i] = multiplymat4(multiplymat4(multiplymat4(multiplymat4(translatevec3(center), rotateX(-10.0)), translatevec3(translation[i])), rotations[i]), scale(scaleArray[i]/700.0));
		//modelArr[i] = translatevec3(translation[i]);
		//modelArr[i] = multiplymat4(multiplymat4(translate(positions[i].x+65.0/2.0, positions[i].y, positions[i].z), rotations[i]), scale(scaleArray[i]/10.0));
	}
	//multiplymat4(multiplymat4(multiplymat4(positionMatrix, translatevec3(translation)), scale(15.0)),rotateX(90.0));

	glBindVertexArray(vao);
	initMVP(shader, model, getViewMatrix());

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(sizeof(vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(2*sizeof(vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(3*sizeof(vec4)));
	glEnableVertexAttribArray(6);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4)*vertexNumber, &modelArr, GL_STATIC_DRAW);

	glUniform3f(glGetUniformLocation(shader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(shader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertexNumber, drawAmount);
	glBindVertexArray(0);
}

void drawTess(GLuint vao, GLuint shader, int vertices, GLuint texture, GLuint texture2, GLuint normal, GLuint displacement, mat4 m, vec3 position, vec4 lightPosition, mat4 lightSpaceMatrix) {
	glUseProgram(shader);
	initMVP(shader, m, getViewMatrix());
	glBindVertexArray(vao);

	mat4 positionMatrix = translatevec3(position);
	vec4 camPosition = getCameraPosition(positionMatrix);
	glUniform3f(glGetUniformLocation(shader, "camPosition"), camPosition.x, camPosition.y, camPosition.z);
	glUniform3f(glGetUniformLocation(shader, "translation"), position.x, position.y, position.z);
	glUniform3f(glGetUniformLocation(shader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
	glUniformMatrix4fv(glGetUniformLocation(shader, "lightSpace"), 1, GL_FALSE, &lightSpaceMatrix.m[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "noiseTexture"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(glGetUniformLocation(shader, "depthMap"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normal);
	glUniform1i(glGetUniformLocation(shader, "normalTex"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, displacement);
	glUniform1i(glGetUniformLocation(shader, "displacementTex"), 4);

	glPatchParameteri(GL_PATCH_VERTICES, vertices);
	glDrawArrays(GL_PATCHES, 0, vertices);
	glBindVertexArray(0);
}

void draw(GLuint vao, GLuint shader, int vertices, GLuint texture, GLuint normal, mat4 m, vec3 position, vec4 lightPosition, mat4 lightSpaceMatrix) {
	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shader);
	initMVP(shader, m, getViewMatrix());
	glBindVertexArray(vao);
	mat4 positionMatrix = translatevec3(position);
	vec4 camPosition = getCameraPosition(positionMatrix);

	glUniform3f(glGetUniformLocation(shader, "cameraPos"), camPosition.x, camPosition.y, camPosition.z);
	glUniform3f(glGetUniformLocation(shader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
	glUniformMatrix4fv(glGetUniformLocation(shader, "lightSpace"), 1, GL_FALSE, &lightSpaceMatrix.m[0][0]);
	glUniform1f(glGetUniformLocation(shader, "time"), glfwGetTime()/100.0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(glGetUniformLocation(shader, "depthMap"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normal);
	glUniform1i(glGetUniformLocation(shader, "noiseTexture"), 2);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glDisable(GL_BLEND);
}

vec4 rotateLight(GLuint vao, GLuint shader, int vertices, GLuint texture, float theta, vec3 position)
{
	glUseProgram(shader);
	glBindVertexArray (vao);
	mat4 positionMatrix = translatevec3(position);
	vec4 camPosition = getCameraPosition(positionMatrix);

	vec3 translation;
	translation.x = (-400.0);// * cos(theta/75.0);
	translation.y = 0.0;
	translation.z = (-400.0);// * sin(theta/75.0);

	mat4 model = multiplymat4(multiplymat4(multiplymat4(positionMatrix, translatevec3(translation)), scale(15.0)),rotateX(90.0));
	initMVP(shader, model, getViewMatrix());

	glUniform3f(glGetUniformLocation(shader, "cameraPos"), camPosition.x, camPosition.y, camPosition.z);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
	glBindVertexArray(0);

	vec4 l = {translation.x, translation.y, translation.z, 1.0};
	return l;
}

void drawOrbit(GLuint vao, GLuint shader, int vertices, GLuint texture, float theta, mat4 m, vec3 position)
{
	glUseProgram(shader);
	glBindVertexArray (vao);
	mat4 positionMatrix = translatevec3(position);
	vec4 camPosition = getCameraPosition(positionMatrix);

	vec3 translation;
	translation.x = (200.0) * cos(theta/100.0);
	translation.y = 0.0;
	translation.z = (200.0) * sin(theta/100.0);

	mat4 model = multiplymat4(multiplymat4(multiplymat4(positionMatrix, translatevec3(translation)), scale(10.0)),rotateX(90.0));
	initMVP(shader, model, getViewMatrix());

	glUniform3f(glGetUniformLocation(shader, "cameraPos"), camPosition.x, camPosition.y, camPosition.z);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
}

void drawDepth(GLuint vao, GLuint shader, int vertices, vec4 lightPosition, mat4 m, mat4 l) {
	glUseProgram(shader);
	glBindVertexArray(vao);
	initMVP(shader, m, l);
	glUniformMatrix4fv(glGetUniformLocation(shader, "lightSpace"), 1, GL_FALSE, &l.m[0][0]);
	glUniform3f(glGetUniformLocation(shader, "lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
}

void drawNoiseFramebuffer() {

}

void doMovement(float deltaTime) {
	float deltaSpeed = 1.0;
	if(keys == GLFW_KEY_W && actionPress == GLFW_PRESS)
        processKeyboard(FORWARD, deltaTime, deltaSpeed);
    if(keys == GLFW_KEY_S && actionPress == GLFW_PRESS)
        processKeyboard(BACKWARD, deltaTime, deltaSpeed);
    if(keys == GLFW_KEY_A && actionPress == GLFW_PRESS)
        processKeyboard(LEFT, deltaTime, deltaSpeed);
    if(keys == GLFW_KEY_D && actionPress == GLFW_PRESS)
        processKeyboard(RIGHT, deltaTime, deltaSpeed);
}

float getDeltaTime(float lastFrame) {
	GLfloat currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	doMovement(deltaTime);
	return deltaTime;
}

vec3 *getRandomPositions(vec3 *positions, int numDraws) {
	float width = 30.0;
	float height = 1.0;

	for(int i = 0; i < numDraws; i++){
		positions[i].x = -(((float)rand()/(float)(RAND_MAX)) * width);
		positions[i].y = -((float)rand()/(float)(RAND_MAX)) * height;
		positions[i].z = -(((float)rand()/(float)(RAND_MAX)) * width);
	}

	return positions;
}

mat4 *getRandomRotations(mat4 *rotations, int numDraws) {
	vec3 xAxis = {1.0, 0.0, 0.0};
	vec3 yAxis = {0.0, 1.0, 0.0};
	vec3 zAxis = {0.0, 0.0, 1.0};

	quaternion xRot[numDraws], yRot[numDraws];
	for(int i = 0; i < numDraws; i++){
		xRot[i] = angleAxis((-(((float)rand()/(float)(RAND_MAX)) * M_PI)), xAxis, zAxis);
		yRot[i] = angleAxis((-(((float)rand()/(float)(RAND_MAX)) * M_PI)), yAxis, zAxis);
	}

	for(int i = 0; i < numDraws; i++) {
		rotations[i] = quaternionToRotation(quatMultiply(xRot[i], yRot[i]));
	}

	return rotations;
}

time_t getFileLastChangeTime(const char *path) {
	struct stat file_stat;
	int err = stat(path, &file_stat);
	if(err != 0) {
		perror(" [file_is_modified] stat");
        exit(0);
	}
	return file_stat.st_mtime;
}

int checkFileChange(const char *path, time_t oldMTime) {
	struct stat file_stat;
	int err = stat(path, &file_stat);
	if(err != 0) {
		perror(" [file_is_modified] stat");
        exit(0);
	}
	return file_stat.st_mtime > oldMTime;
}

int checkShaderChange(GLuint shader, char *vert, char *frag, time_t vertTime, time_t fragTime) {
	if(checkFileChange(vert, vertTime)) {
		printf("FILE CHANGED\n");
		return 1;
	}
	if(checkFileChange(frag, fragTime)) {
		printf("FILE CHANGED\n");
		return 1;
	}
	return 0;
}

/*void *testThread(void *dx) {
	complex *t_dx = (complex*)dx;
	for(int i = 0; i < 100; i++) {
		printf("%f, %f\n", t_dx->real, t_dx->im);
	}
	free(dx);
	return NULL;
}*/

int main(int argc, char *argv[])
{
	float theta = 0.0;
	printf("\tWorkdir: %s\n", getenv("PWD"));
	chdir("/Users/tjgreen/Documents/OpenGL/gl_tests/terrain");
	GLFWwindow *window = setupGLFW();

	initializeWaves(256);
	initializeNoise();

	/*pthread_t waveThread;
	if(pthread_create(&waveThread, NULL, testThread, tildeh0k)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	if(pthread_join(waveThread, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}*/


	GLuint tessShader = initTessShader();
	GLuint skyShader = initSkyShader();
	GLuint atmosphereShader = initAtmosphereShader();
	GLuint ringShader = initLightingShader();
	GLuint waterShader = initWaterShader();
	GLuint depthShader = initDepthShader();
	GLuint fboShader = initFramebufferShader();
	GLuint instanceShader = initInstanceShader();

	GLuint earthTex = loadTexture("assets/earth.jpg", 0);
	GLuint moonTex = loadTexture("assets/moon.jpg", 0);
	GLuint planetTex = loadTexture("assets/europa.jpg", 0);
	GLuint planetNorm = loadTexture("assets/NormalMap.png", 0);
	GLuint planetDisp = loadTexture("assets/DisplacementMap.png", 0);

	GLuint ringTex = loadTexture("assets/ring.png", 1);
	GLuint sphereVAO = initSphere();
	GLuint ringVAO = initRing();
	GLuint objectVAO = initObjectBuffer("assets/rifter.obj");
	GLuint rockVAO = initRockBuffer("assets/rock.obj");
	GLuint rock2VAO = initRockBuffer("assets/rock2.obj");
	GLuint rock3VAO = initRockBuffer("assets/rock3.obj");
	GLuint subQuadVAO = initSubQuad();

	GLuint quadCubeVAO = initQuadCube(30);

	GLuint depthbuffer = initDepthbuffer();
	GLuint framebuffer = initFramebuffer(&textureColorBuffer);
	GLuint sunFramebuffer = initFramebuffer(&sunNoiseTexture);
	//GLuint framebuffer3D = initFramebuffer3D();
	GLuint quadVAO = initQuad();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	int instancedDraws = 720;

	vec3 pos1[instancedDraws];
	*pos1 = *getRandomPositions(pos1, instancedDraws);
	vec3 pos2[instancedDraws];
	*pos2 = *getRandomPositions(pos2, instancedDraws);
	vec3 pos3[instancedDraws];
	*pos3 = *getRandomPositions(pos3, instancedDraws);

	mat4 rotations[instancedDraws];
	*rotations = *getRandomRotations(rotations, instancedDraws);

	float scaleArray[instancedDraws];
	for(int i = 0; i < instancedDraws; i++){
		scaleArray[i] = -((float)rand()/(float)(RAND_MAX)) * 360.0;
	}

	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);

	mat4 model, atmo;
	glViewport(0, 0, getWindowWidth(), getWindowHeight());

	float deltaTime = 0.0;
	float lastFrame = 0.0;
	float rad = 180.0 / M_PI;
	vec3 translation = {65.0, 0.0, 0.0};
	mat4 lightProjection = ortho(-400.0, 400.0, -400.0, 400.0, zNear, zFar);
	//mat4 lightProjection = perspective(90.0, getWindowWidth()/getWindowHeight(), zNear, zFar);

	char *vertCheck = "shaders/water.vert";
	char *fragCheck = "shaders/water.frag";
	time_t vertTime = getFileLastChangeTime(vertCheck);
	time_t fragTime = getFileLastChangeTime(fragCheck);

	GLuint dxWaveTex;
	glGenTextures(1, &dxWaveTex);
	GLuint dyWaveTex;
	glGenTextures(1, &dyWaveTex);
	GLuint dzWaveTex;
	glGenTextures(1, &dzWaveTex);

	while(!glfwWindowShouldClose(window))
	{
		if(checkShaderChange(tessShader, vertCheck, fragCheck, vertTime, fragTime)) {
			waterShader = initWaterShader();
			vertTime = getFileLastChangeTime(vertCheck);
			fragTime = getFileLastChangeTime(fragCheck);
		}

		clock_t start,end;
		float time_spent;
		start=clock();
		generateWaves(256, &dxWaveTex, &dyWaveTex, &dzWaveTex);
		end=clock();
		time_spent=(((float)end - (float)start) / 1000000.0F );
		//printf("\nSystem time is at %f\n seconds", time_spent);

		generateNoiseTexture(framebuffer, 0);
		generateNoiseTexture(sunFramebuffer, 1);
		theta += 0.5;

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		doMovement(deltaTime);


		float fScale = 63.710;
		float fScaleFactor = 1.25;//1.025;

		//int terrainMaxLOD = (int)(log(fScale)/log(2));

		//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		//glClearColor(1.0, 1.0, 1.0, 1.0);

		//Light calculations per frame
		vec4 lightPosition = rotateLight(quadCubeVAO, ringShader, qc.vertexNumber, sunNoiseTexture, theta, translation);
		vec3 lightPositionXYZ = {lightPosition.x, lightPosition.y, lightPosition.z};
		vec3 d = {lightPosition.x - translation.x, lightPosition.y - translation.y, lightPosition.z - translation.z};
		d = normalizevec3(d);
		float lightYaw = asin(-d.y) * rad;
		float lightPitch = atan2(d.x, -d.z) * rad;
		//printf("yaw:%f, pitch:%f\n", lightYaw, lightPitch);
		mat4 rxry = multiplymat4(rotateX(lightYaw), rotateY(lightPitch));
		vec4 negativeLight = {lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w};
		lightView = multiplymat4(rxry, translatevec4(negativeLight));
		mat4 lightSpaceMatrix = multiplymat4(lightProjection, lightView);

		glBindFramebuffer(GL_FRAMEBUFFER, depthbuffer);
			glClear(GL_DEPTH_BUFFER_BIT);
			vec3 position;
			position.x = (200.0) * cos(theta/100.0);
			position.y = 0.0;
			position.z = (200.0) * sin(theta/100.0);
			mat4 positionMatrix = translatevec3(translation);
			model = multiplymat4(multiplymat4(multiplymat4(positionMatrix, translatevec3(position)), scale(10.0)),rotateX(90.0));
			drawDepth(quadCubeVAO, depthShader, qc.vertexNumber, lightPosition, model, lightSpaceMatrix);
			model = multiplymat4(multiplymat4(translatevec3(translation), rotateX(80.0)), scale(fScale*1.5));
			glDisable(GL_CULL_FACE);
			drawDepth(ringVAO, depthShader, planetRing.vertexNumber, lightPosition, model, lightSpaceMatrix);
			glEnable(GL_CULL_FACE);
			model = multiplymat4(translatevec3(translation), scale(fScale));
			drawDepth(quadCubeVAO, depthShader, qc.vertexNumber, lightPosition, model, lightSpaceMatrix);
			//model = multiplymat4(translate(25.0, 0.0, -90.0), scale(10.0));
			//drawDepth(quadVAO, depthShader, 6, lightPosition, model, lightSpaceMatrix);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(0.1, 0.0, 0.2, 1.0);

		model = multiplymat4(translate(-25.0, 5.0, 5.0), scale(10.0));
		//draw(quadCubeVAO, ringShader, qc.vertexNumber, moonTex, model, translation, lightPosition);

		//draw(quadCubeVAO, ringShader, qc.vertexNumber, sunNoiseTexture, model, translation);
		drawOrbit(quadCubeVAO, ringShader, qc.vertexNumber, moonTex, theta, model, translation);
		//Planet ring
		model = multiplymat4(multiplymat4(translatevec3(translation), rotateX(80.0)), scale(fScale*1.5));
		draw(ringVAO, ringShader, planetRing.vertexNumber, ringTex, planetNorm, model, translation, lightPosition, lightSpaceMatrix);
		//Planet
		mat4 matR = multiplymat4(rotateY(theta/5.0), rotateX(45.0));
		model = multiplymat4(translatevec3(translation), scale(fScale));
		mat4 m = multiplymat4(model,matR);
		drawTess(quadCubeVAO, tessShader, qc.vertexNumber, textureColorBuffer, planetTex, planetNorm, planetDisp, m, translation, lightPosition, lightSpaceMatrix);
		//Water
		model = multiplymat4(translatevec3(translation), scale(fScale*1.01));
		m = multiplymat4(model,matR);
		draw(quadCubeVAO, waterShader, qc.vertexNumber, dyWaveTex, dxWaveTex, m, translation, lightPosition, lightSpaceMatrix);
		//Sun
		model = multiplymat4(multiplymat4(multiplymat4(positionMatrix, translatevec3(lightPositionXYZ)), scale(250.0)),rotateX(90.0));
		draw(quadCubeVAO, ringShader, qc.vertexNumber, sunNoiseTexture, planetNorm, model, lightPositionXYZ, lightPosition, lightSpaceMatrix);
		//FBO Visualizer
		model = multiplymat4(translate(-75.0, 25.0, 0.0), scale(10.0));
		draw(quadVAO, fboShader, 6, textureColorBuffer, planetNorm, model, translation, lightPosition, lightSpaceMatrix);
		//Object
		vec3 arcBallPos = getCamera();
		float modelRotationAngle = 0.0;
		model = multiplymat4(multiplymat4(translate(-arcBallPos.x, -arcBallPos.y, -arcBallPos.z), rotateX(modelRotationAngle)), scale(0.5));
		draw(objectVAO, ringShader, ship.vertexNumber, earthTex, planetNorm, model, lightPositionXYZ, lightPosition, lightSpaceMatrix);

		//Flat quad
		model = multiplymat4(translate(-100.0, 0.0, 0.0), scale(100.0));
		draw(subQuadVAO, waterShader, 200*200*6, dyWaveTex, dxWaveTex, model, lightPositionXYZ, lightPosition, lightSpaceMatrix);

		//glUseProgram(instanceShader);
		//drawInstanced(rockVAO, positionsVBO, instanceShader, object.vertexNumber, instancedDraws, pos1, rotations, model, scaleArray, theta, lightPosition);
		//drawInstanced(rock2VAO, positionsVBO, instanceShader, object.vertexNumber, instancedDraws, pos2, rotations, model, scaleArray, theta, lightPosition);
		//drawInstanced(rock3VAO, positionsVBO, instanceShader, object.vertexNumber, instancedDraws, pos3, rotations, model, scaleArray, theta, lightPosition);

		//Atmosphere
		//atmo = multiplymat4(translatevec3(translation), scale(fScale*fScaleFactor));
		//draw(quadCubeVAO, ringShader, qc.vertexNumber, earthTex, atmo, translation, lightPosition, lightSpaceMatrix);
		//drawAtmosphere(sphereVAO, atmosphereShader, skyShader, planet.vertexNumber, atmo, translation, fScale, fScaleFactor, lightPosition);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	if(action == GLFW_PRESS)
    	actionPress = GLFW_PRESS;
    else if(action == GLFW_RELEASE)
    	actionPress = 0;

    if (key == GLFW_KEY_W && action == GLFW_PRESS){
    	keys = GLFW_KEY_W;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS){
    	keys = GLFW_KEY_S;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS){
    	keys = GLFW_KEY_A;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS){
    	keys = GLFW_KEY_D;
    }
    if(action == GLFW_RELEASE)
    	keys = 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	//xpos = 1.0*xpos/getWindowWidth()*2 - 1.0;
	//ypos =  1.0*ypos/getWindowHeight()*2 - 1.0;
	if (state == GLFW_PRESS)
	{
		processMouseMovement(xpos, ypos, 0);
	}
	else {
		processMouseMovement(xpos, ypos, 1);
	}
	mousePosX = xpos;
	mousePosY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	mouseZoomOffset = processMouseScroll(yoffset, mouseZoomOffset);
}
