#include "Main.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

float zNear = 0.5, zFar = 100000.0;
int mousePosX, mousePosY, actionPress, keys;
GLuint depthMap, textureColorBuffer;
GLuint depthCubemap;
struct sphere planet;
struct obj object;
struct ring planetRing;

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


GLuint loadTexture(char const * path, int alphaFlag)
{
    //Generate texture ID and load texture data 
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image;
    
    if(alphaFlag == 1) {
    	image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
    	glBindTexture(GL_TEXTURE_2D, textureID);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    	glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
    	image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    	// Assign texture to ID
    	glBindTexture(GL_TEXTURE_2D, textureID);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    	glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

GLuint generateTextureAttachment(int depth, int stencil, vec2 size) {
	GLuint textureID;
	GLenum attachment_type;
	if(!depth && !stencil)
		attachment_type = GL_RGB;
	else if(depth && !stencil)
		attachment_type = GL_DEPTH_COMPONENT;
	else if(!depth && stencil)
		attachment_type = GL_STENCIL_INDEX;
		
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if(!depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, getWindowWidth(), getWindowHeight(), 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	else if(depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, getWindowWidth(), getWindowHeight(), 0, attachment_type, GL_FLOAT, NULL);
		
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_BORDER
    glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

GLuint initInstanceBuffer(vec3 *vertices, int vertSize, vec3 *normals, int normSize, vec2 *texCoords, int texSize) {
	GLuint vbo, vao;
	float degToRad = M_PI / 180.0;
	
	int num = 1440;
	float posXArray[num];
	for(int i = 0; i < num; i++){
		posXArray[i] = -(((float)rand()/(float)(RAND_MAX)) * 15.0);
	}
	float posYArray[num];
	for(int i = 0; i < num; i++){
		posYArray[i] = -(((float)rand()/(float)(RAND_MAX)) * 15.0);
	}
	float posZArray[num];
	for(int i = 0; i < num; i++){
		posZArray[i] = -(((float)rand()/(float)(RAND_MAX)));
	}
	
	float rotXarray[num];
	for(int i = 0; i < num; i++){
		rotXarray[i] = -(((float)rand()/(float)(RAND_MAX)) * 360.0);
	}
	float rotYarray[num];
	for(int i = 0; i < num; i++){
		rotYarray[i] = -(((float)rand()/(float)(RAND_MAX)) * 360.0);
	}
	float rotZarray[num];
	for(int i = 0; i < num; i++){
		rotZarray[i] = -(((float)rand()/(float)(RAND_MAX)) * 360.0);
	}
	
	float scaleArray[num];
	for(int i = 0; i < num; i++){
		scaleArray[i] = -(((float)rand()/(float)(RAND_MAX)));
	}
	
	
	vec3 points[num];
	for(int i = 0; i < num; i++) {
		float deg = i * degToRad;
		points[i].x = cos(deg)*(63.710/2.0)+posXArray[i];
		points[i].y = sin(deg)*(63.710/2.0)+posYArray[i];
		points[i].z = posZArray[i];
	}
	
	mat4 positions[num];
	mat4 rotations[num];
	for(int i = 0; i < num; i++) {
		rotations[i] = multiplymat4(rotateX(rotXarray[i]), multiplymat4(rotateY(rotYarray[i]), rotateZ(rotZarray[i])));
	}
	for(int i = 0; i < num; i++) {
		positions[i] = multiplymat4(multiplymat4(multiplymat4(rotateX(65.0), translate(points[i].x+32.0, points[i].y, points[i].z)), rotations[i]), scale(scaleArray[i]/10.0));
	}
	
	vec3 translation = {65.0, 0.0, 0.0};
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertSize+normSize+texSize+(sizeof(mat4)*num), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normSize, normals);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize+normSize, texSize, texCoords);
	
	glBufferSubData(GL_ARRAY_BUFFER, vertSize+normSize+texSize, (sizeof(mat4)*num), &positions);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize));
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(vertSize+normSize+texSize));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(vertSize+normSize+texSize+sizeof(vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(vertSize+normSize+texSize+2*sizeof(vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(vertSize+normSize+texSize+3*sizeof(vec4)));
	glEnableVertexAttribArray(6);
	
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	
	glBindVertexArray(0);
	return vao;
}

void createShader(GLuint *shader, char *vert, char *frag)
{
	GLuint vertShader = LoadShader(vert, GL_VERTEX_SHADER);
    GLuint fragShader = LoadShader(frag, GL_FRAGMENT_SHADER);
    *shader = glCreateProgram();
    glAttachShader(*shader, vertShader);
    glAttachShader(*shader, fragShader);
    glLinkProgram(*shader);
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

GLuint initDepthShader() {
	GLuint shader;
	GLuint vertShader = LoadShader("shaders/depth.vert", GL_VERTEX_SHADER);
	GLuint geomShader = LoadShader("shaders/depth.geom", GL_GEOMETRY_SHADER);
	GLuint fragShader = LoadShader("shaders/depth.frag", GL_FRAGMENT_SHADER);
	shader = glCreateProgram();
	glAttachShader(shader, vertShader);
	glAttachShader(shader, geomShader);
	glAttachShader(shader, fragShader);
	glLinkProgram(shader);
	
	return shader;
}

GLuint initFramebufferShader() {
	GLuint shader;
	createShader(&shader, "shaders/framebuffer.vert",
		"shaders/framebuffer.frag");
	return shader;
}

GLuint initNoiseShader() {
	GLuint shader;
	createShader(&shader, "shaders/noise.vert",
		"shaders/noise.frag");
	return shader;
}

GLuint initTessShader() {
	GLuint shader;
	GLuint vertShader = LoadShader("shaders/tess.vert", GL_VERTEX_SHADER);
	GLuint tcshShader = LoadShader("shaders/tess.tcsh", GL_TESS_CONTROL_SHADER);
	GLuint teshShader = LoadShader("shaders/tess.tesh", GL_TESS_EVALUATION_SHADER);
	GLuint geomShader = LoadShader("shaders/tess.geom", GL_GEOMETRY_SHADER);
	GLuint fragShader = LoadShader("shaders/tess.frag", GL_FRAGMENT_SHADER);
	shader = glCreateProgram();
	glAttachShader(shader, vertShader);
	glAttachShader(shader, tcshShader);
	glAttachShader(shader, teshShader);
	glAttachShader(shader, geomShader);
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
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize));
	glEnableVertexAttribArray(2);
	
	glBindVertexArray(0);
	return vao;
}

GLuint initNoiseBuffer(float *points, vec2 *position, int pointSize, int vecSize) {
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, pointSize+vecSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, pointSize, points);
	glBufferSubData(GL_ARRAY_BUFFER, pointSize, vecSize, position);
	
	//for(int i = 0; i < 512*512; i++) {
		//printf("%f, %f, %f, \n", position[i].x, position[i].y, points[i]);
	//}
	//printf("\nSize is: %d: ", vecSize);
	
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(pointSize));
	glEnableVertexAttribArray(1);
	
	glBindVertexArray(0);
	return vao;
}

GLuint initFramebuffer() {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	vec2 size = {512.0, 512.0};
	textureColorBuffer = generateTextureAttachment(0, 0, size);
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

GLuint initDepthBuffer() {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	vec2 size = {getWindowWidth(), getWindowWidth()};
	depthMap = generateTextureAttachment(1, 0, size);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	  if(status != GL_FRAMEBUFFER_COMPLETE)
		  printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return fbo;
}

GLuint initCubeDepthbuffer() {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	depthCubemap = generateDepthCubemap(1024, 1024);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	  if(status != GL_FRAMEBUFFER_COMPLETE)
		  printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return fbo;
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

GLuint initNoise() {
	GLuint vao;
	clock_t start,end;
	float time_spent;
	start=clock();
	int arrSise = 512;
	
	int count = 0;
	vec2 pos[arrSise*arrSise];
	float sn[arrSise*arrSise];
	//float **sn = malloc(1024 * sizeof *sn + (1024 * (1024 * sizeof **sn)));
	float s = 0.0, n = 0.0;
	//float fi = 0.0, fj = 0.0;
	for(int i = 0; i < arrSise; i++) {
		for(int j = 0; j < arrSise; j++) {
			pos[count].x = i;
			pos[count].y = j;
			sn[count] = sNoise2d(i, j, &s, &n);
			count++;
		}
	}
	
	end=clock();
	time_spent=(((float)end - (float)start) / 1000000.0F );
	printf("\nSystem time is at %f seconds\n", time_spent);
	
	vao = initNoiseBuffer(sn, pos, sizeof(sn), sizeof(pos));
	return vao;
	
}

GLuint initObjectBuffer() {
	GLuint vao;
	object = ObjLoadModel("/Users/tjgreen/Documents/OpenGL/gl_tests/terrain/assets/chimera.obj");
	vec3 vna[object.vertexNumber];
	vec2 texCoords[object.vertexNumber];
	*vna = *generateSmoothNormals(vna, object.points, object.normals, object.vertexNumber);
	
	for(int i = 0; i < object.vertexNumber; i++) {
    	texCoords[i].x = 1.0;
    	texCoords[i].y = 0.0;
    }
    vao = initBuffers(object.points, object.size, object.normals, object.nsize, texCoords, sizeof(texCoords[0])*object.vertexNumber);
	
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
	planetRing = createRing(1, 1.0, 2.0);
	vao = initBuffers(planetRing.points, planetRing.size, planetRing.normals, planetRing.nsize, planetRing.texCoords, planetRing.texsize);
	return vao;
}

vec4 getCameraPosition(mat4 position) {
	mat4 mvTranspose = transposemat4(multiplymat4(position, getViewPosition()));
	vec4 inverseCamera = {-mvTranspose.m[3][0], -mvTranspose.m[3][1], -mvTranspose.m[3][2], -mvTranspose.m[3][3]};
	vec4 camPosition = multiplymat4vec4(mvTranspose, inverseCamera);
	
	return camPosition;
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

void drawAtmoshere(GLuint VAO, GLuint shader, GLuint sky, int vertices, mat4 m, vec3 position, float scale, float scaleFactor) {
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
	
	//printf("%f, %f\n", pow(camPosition.x, 2.0) + pow(camPosition.y, 2.0) + pow(camPosition.z, 2.0), pow(m.m[0][0], 2.0));
	//printf("%f, %f, %f, %f\n", pow(camPosition.x-m.m[0][0],2.0), pow(camPosition.y-m.m[0][1], 2.0), pow(camPosition.z-m.m[0][2], 2.0), pow(m.m[0][0], 2.0));
	
	//float scaleFactor = 3.25;
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
	//printf("%f, %f, %f\n", camPosition.x, camPosition.y, camPosition.z);
	
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
	
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
}

void drawTess(GLuint vao, GLuint shader, int vertices, GLuint texture, mat4 m, vec3 position) {
	glUseProgram(shader);
	initMVP(shader, m, getViewMatrix());
	glBindVertexArray(vao);
	
	mat4 positionMatrix = translatevec3(position);
	vec4 camPosition = getCameraPosition(positionMatrix);
	glUniform3f(glGetUniformLocation(shader, "camPosition"), camPosition.x, camPosition.y, camPosition.z);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
	glPatchParameteri(GL_PATCH_VERTICES, vertices);
	glDrawArrays(GL_PATCHES, 0, vertices);
	glBindVertexArray(0);
}

void drawNoise(GLuint vao, GLuint shader, int vertices, GLuint texture) {
	glDisable(GL_CULL_FACE);
	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, vertices);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
}

void draw(GLuint vao, GLuint shader, int vertices, GLuint texture, mat4 m, vec3 position) {
	glDisable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shader);
	initMVP(shader, m, getViewMatrix());
	glBindVertexArray(vao);
	mat4 positionMatrix = translatevec3(position);
	vec4 camPosition = getCameraPosition(positionMatrix);
	
	glUniform3f(glGetUniformLocation(shader, "cameraPos"), camPosition.x, camPosition.y, camPosition.z);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glDisable(GL_BLEND);
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

GLuint initCube() {
	GLuint vao;
	GLfloat vertices[] = {
        // Positions          
		 1.0, -1.0,  1.0, 
		-1.0, -1.0,  1.0, 
		 1.0, -1.0, -1.0, 
		 1.0, -1.0, -1.0, 
		-1.0, -1.0,  1.0, 
		-1.0, -1.0, -1.0, 

		-1.0,  1.0, -1.0, 
		-1.0,  1.0,  1.0, 
		 1.0,  1.0,  1.0, 
		 1.0,  1.0,  1.0, 
		 1.0,  1.0, -1.0, 
		-1.0,  1.0, -1.0, 

		-1.0, -1.0,  1.0, 
		 1.0, -1.0,  1.0, 
		 1.0,  1.0,  1.0, 
		 1.0,  1.0,  1.0, 
		-1.0,  1.0,  1.0, 
		-1.0, -1.0,  1.0, 

		 1.0, -1.0, -1.0, 
		 1.0,  1.0, -1.0, 
		 1.0,  1.0,  1.0, 
		 1.0,  1.0,  1.0, 
		 1.0, -1.0,  1.0, 
		 1.0, -1.0, -1.0, 

		-1.0, -1.0,  1.0, 
		-1.0,  1.0,  1.0, 
		-1.0,  1.0, -1.0, 
		-1.0,  1.0, -1.0, 
		-1.0, -1.0, -1.0, 
		-1.0, -1.0,  1.0, 

		-1.0,  1.0, -1.0, 
		 1.0,  1.0, -1.0, 
		 1.0, -1.0, -1.0, 
		 1.0, -1.0, -1.0, 
		-1.0, -1.0, -1.0, 
		-1.0,  1.0, -1.0
    };
    GLfloat texCoords[] = {
    	1.0f, 0.0f,
    	0.0f, 1.0f,
		1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        
        0.0f, 0.0f,
    	0.0f, 1.0f,
		1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        
        0.0f, 0.0f,
    	0.0f, 1.0f,
		1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        
        0.0f, 0.0f,
    	0.0f, 1.0f,
		1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        
        0.0f, 0.0f,
    	0.0f, 1.0f,
		1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        
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

int main(int argc, char *argv[]) 
{
	float theta = 0.0;
	GLfloat deltaTime = 0.0;
	GLfloat lastFrame = 0.0;
	chdir("/Users/tjgreen/Documents/OpenGL/gl_tests/terrain");
	GLFWwindow *window = setupGLFW();

	GLuint tessShader = initTessShader();
	GLuint skyShader = initSkyShader();
	GLuint atmosphereShader = initAtmosphereShader();
	GLuint ringShader = initLightingShader();
	//GLuint depthShader = initDepthShader();
	GLuint fboShader = initFramebufferShader();
	GLuint noiseRenderShader = initNoiseShader();
	GLuint instanceShader = initInstanceShader();
	
	//GLuint depthbuffer = initCubeDepthbuffer();
	GLuint earthTex = loadTexture("shaders/earth.jpg", 0);
	GLuint ringTex = loadTexture("shaders/ring.png", 1);
	GLuint sphereVAO = initSphere();
	GLuint ringVAO = initRing();
	GLuint objectVAO = initObjectBuffer();
	GLuint rockVAO = initRockBuffer("shaders/rock.obj");
	GLuint rock2VAO = initRockBuffer("shaders/rock2.obj");
	GLuint rock3VAO = initRockBuffer("shaders/rock3.obj");
	
	GLuint framebuffer = initFramebuffer();
	GLuint quadVAO = initQuad();
	GLuint sNoiseVAO = initNoise();
	
	vec3 lookAt[6] = {{1.0, 0.0, 0.0}, {-1.0, 0.0, 0.0},{0.0, 1.0, 0.0},
					{0.0, -1.0, 0.0},{0.0, 0.0, -1.0},{0.0, 0.0, 1.0}};
	vec4 lightPosition = {0.0, 0.0, 0.0, 1.0};
	
	mat4 shadowProjection = perspective(90.0, 1024.0/1024.0, zNear, zFar);
	mat4 lightSpaceMatrix[6];
	vec3 s0 = {-1.0, 1.0, 1.0};
	lightSpaceMatrix[0] = multiplymat4(shadowProjection, multiplymat4(scalevec3(s0) ,getLookAtMatrix(lightPosition, lookAt[0])));
	vec3 s1 = {1.0, 1.0, 1.0};
	lightSpaceMatrix[1] = multiplymat4(shadowProjection, multiplymat4(scalevec3(s1) ,getLookAtMatrix(lightPosition, lookAt[1])));
	vec3 s2 = {-1.0, 1.0, 1.0};
	lightSpaceMatrix[2] = multiplymat4(shadowProjection, multiplymat4(scalevec3(s2) ,getLookAtMatrix(lightPosition, lookAt[2])));
	vec3 s3 = {1.0, 1.0, 1.0};
	lightSpaceMatrix[3] = multiplymat4(shadowProjection, multiplymat4(scalevec3(s3) ,getLookAtMatrix(lightPosition, lookAt[3])));
	vec3 s4 = {-1.0, 1.0, 1.0};
	lightSpaceMatrix[4] = multiplymat4(shadowProjection, multiplymat4(scalevec3(s4) ,getLookAtMatrix(lightPosition, lookAt[4])));
	vec3 s5 = {1.0, 1.0, 1.0};
	lightSpaceMatrix[5] = multiplymat4(shadowProjection, multiplymat4(scalevec3(s5) ,getLookAtMatrix(lightPosition, lookAt[5])));
	
	GLuint cubeVAO = initCube();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawNoise(sNoiseVAO, noiseRenderShader, 512*512, ringTex);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	mat4 model, atmo;
	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	while(!glfwWindowShouldClose(window))
	{
		theta += 0.5;
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		doMovement(deltaTime);
		glClearColor(0.1, 0.0, 0.2, 1.0);
		
		
		vec3 translation = {65.0, 0.0, 0.0};
		float fScale = 63.710;
		float fScaleFactor = 1.025;
		
		
		/*glBindFramebuffer(GL_FRAMEBUFFER, depthbuffer);
			glClear(GL_DEPTH_BUFFER_BIT);
			model = multiplymat4(multiplymat4(translatevec3(translation), rotateX(65.0)), scale(fScale*1.5));
			draw(ringVAO, depthShader, planetRing.vertexNumber, ringTex, model, translation);
			model = multiplymat4(translatevec3(translation), scale(fScale));
			draw(sphereVAO, depthShader, planet.vertexNumber, earthTex, model, translation);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
		
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		
		//drawNoise(sNoiseVAO, noiseRenderShader, 512*512, ringTex);
		model = multiplymat4(translate(-10.0, 0.0, 0.0), multiplymat4(rotateX(theta), rotateY(theta)));
		draw(cubeVAO, ringShader, 36, earthTex, model, translation);
		
		model = multiplymat4(multiplymat4(translatevec3(translation), rotateX(65.0)), scale(fScale*1.5));
		draw(ringVAO, ringShader, planetRing.vertexNumber, ringTex, model, translation);
		model = multiplymat4(translatevec3(translation), scale(fScale));
		drawTess(sphereVAO, tessShader, planet.vertexNumber, textureColorBuffer, model, translation);
		atmo = multiplymat4(translatevec3(translation), scale(fScale*fScaleFactor));
		drawAtmoshere(sphereVAO, atmosphereShader, skyShader, planet.vertexNumber, atmo, translation, fScale, fScaleFactor);
		
		model = scale(25.0);
		draw(quadVAO, fboShader, 6, textureColorBuffer, model, translation);
		
		//vec4 cc = getCameraPosition(translate(-10.0, 0.0, 0.0));
		
		//model = multiplymat4(multiplymat4(translate(cc.x, cc.y, cc.z), scale(1.0)), getViewRotation());
		model = multiplymat4(translate(-75.0, 25.0, 0.0), scale(5.0));
		draw(objectVAO, ringShader, object.vertexNumber, earthTex, model, translation);
		
		glUseProgram(instanceShader);
		initMVP(instanceShader, model, getViewMatrix());
		vec4 cameraPos = getCameraPosition(model);
	
		glBindVertexArray(rockVAO);
		
		glUniform3f(glGetUniformLocation(instanceShader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glDrawArraysInstanced(GL_TRIANGLES, 0, object.vertexNumber, 1440);
		glBindVertexArray(0);
		
		glBindVertexArray(rock2VAO);
		
		glUniform3f(glGetUniformLocation(instanceShader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glDrawArraysInstanced(GL_TRIANGLES, 0, object.vertexNumber, 1440);
		glBindVertexArray(0);
		
		glBindVertexArray(rock3VAO);
		
		glUniform3f(glGetUniformLocation(instanceShader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glDrawArraysInstanced(GL_TRIANGLES, 0, object.vertexNumber, 1440);
		glBindVertexArray(0);
		
		
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