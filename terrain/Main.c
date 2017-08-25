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
struct quadCube qc;

int perm[256]= {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};
  
int grad3[16][3] = {{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
                   {1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
                   {1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0}, // 12 cube edges
                   {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}}; // 4 more to make 16
                   
int grad4[32][4]= {{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1}, // 32 tesseract edges
                   {0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
                   {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
                   {-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
                   {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
                   {-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
                   {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
                   {-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}};
                   
unsigned char simplex4[][4] = {{0,64,128,192},{0,64,192,128},{0,0,0,0},
  {0,128,192,64},{0,0,0,0},{0,0,0,0},{0,0,0,0},{64,128,192,0},
  {0,128,64,192},{0,0,0,0},{0,192,64,128},{0,192,128,64},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{64,192,128,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {64,128,0,192},{0,0,0,0},{64,192,0,128},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{128,192,0,64},{128,192,64,0},
  {64,0,128,192},{64,0,192,128},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{128,0,192,64},{0,0,0,0},{128,64,192,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {128,0,64,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {192,0,64,128},{192,0,128,64},{0,0,0,0},{192,64,128,0},
  {128,64,0,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {192,64,0,128},{0,0,0,0},{192,128,0,64},{192,128,64,0}};

GLuint initPermTexture() {
	char *pixels;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	pixels = (char*)malloc(256*256*4);
	for(int i = 0; i < 256; i++) {
		for(int j =0; j < 256; j++) {
			int offset = (i*256+j)*4;
			char value = perm[(j+perm[i]) & 0xFF];
			pixels[offset] = grad3[value & 0x0F][0] * 64 + 64;
			pixels[offset+1] = grad3[value & 0x0F][1] * 64 + 64;
			pixels[offset+2] = grad3[value & 0x0F][2] * 64 + 64;
			pixels[offset+3] = value;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	return textureID;
}

GLuint initSimplexTexture() {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_1D, textureID);
	
	glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, simplex4 );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	
	return textureID;
}

GLuint initGradTexture() {
	char *pixels;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	pixels = (char*)malloc(256*256*4);
	for(int i = 0; i < 256; i++) {
		for(int j =0; j < 256; j++) {
			int offset = (i*256+j)*4;
			char value = perm[(j+perm[i]) & 0xFF];
			pixels[offset] = grad4[value & 0x1F][0] * 64 + 64;
			pixels[offset+1] = grad4[value & 0x1F][1] * 64 + 64;
			pixels[offset+2] = grad4[value & 0x1F][2] * 64 + 64;
			pixels[offset+3] = grad4[value & 0x1F][3] * 64 + 64;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	return textureID;
}

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
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize));
	glEnableVertexAttribArray(2);
	
	glBindVertexArray(0);
	return vao;
}

GLuint initNoiseBuffer(vec3 *points, int pointSize) {
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, pointSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, pointSize, points);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	return vao;
}

GLuint initFramebuffer() {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	vec2 size = {1024.0, 512.0};
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

GLuint initNoise() {
	GLuint vao;
	
	float vertices[] = {
		-1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f
	};
	int numVertices = (sizeof(vertices)/sizeof(vertices[0]));
	int vecSize = numVertices/3;
	vec3 vertArray[vecSize];
	
	int c = 0;
    for(int i = 0; i < numVertices; i+=3) {
    	vertArray[c].x = vertices[i];
    	vertArray[c].y = vertices[i+1];
    	vertArray[c].z = vertices[i+2];
    	c++;
    }
	
	
	
	vao = initNoiseBuffer(vertArray, sizeof(vertices));
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

GLuint initQuadCube() {
	GLuint vao;
	qc = createCube(5);
	vec2 texCoords[qc.vertexNumber];
	for(int i = 0; i < qc.vertexNumber; i++) {
    	texCoords[i].x = 1.0;
    	texCoords[i].y = 0.0;
    }
    vao = initBuffers(qc.points, qc.size, qc.normals, qc.nsize, texCoords, sizeof(texCoords[0])*qc.vertexNumber);
    
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

void drawInstanced(GLuint vao, GLuint vbo, GLuint shader, int vertexNumber, int drawAmount, vec3 *positions, mat4 *rotations, mat4 model, float *scaleArray, float theta) {
	initMVP(shader, model, getViewMatrix());
	vec4 cameraPos = getCameraPosition(model);
	
	vec3 pos[drawAmount];
	for(int i = 0; i < drawAmount; i++){
		float deg = (theta/50.0)+(i * M_PI / 180.0);
		pos[i].x = cos(deg)*32.5;//+(-(((float)rand()/(float)(RAND_MAX)) * width));
		pos[i].y = sin(deg)*32.5;//+(-(((float)rand()/(float)(RAND_MAX)) * width));
		pos[i].z = 10;//-(((float)rand()/(float)(RAND_MAX)) * height);
	}
	
	//vec3 center = {65.0, 0.0, 0.0};
	glfwGetTime();
	
	mat4 modelArr[vertexNumber];
	for(int i = 0; i < vertexNumber; i++) {
		modelArr[i] = multiplymat4(multiplymat4(translate(positions[i].x+65.0/2.0, positions[i].y, positions[i].z), rotations[i]), scale(scaleArray[i]/10.0));
		//modelArr[i] = translate(pos[i].x+65.0/2.0, pos[i].y, pos[i].z);
	}
	
	glBindVertexArray(vao);
	
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
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertexNumber, drawAmount);
	glBindVertexArray(0);
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
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "noiseTexture"), 1);
	glPatchParameteri(GL_PATCH_VERTICES, vertices);
	glDrawArrays(GL_PATCHES, 0, vertices);
	glBindVertexArray(0);
}

void drawNoise(GLuint vao, GLuint shader, int vertices, GLuint permTexture, GLuint simplexTexture, GLuint gradTexture) {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST); 
	glUseProgram(shader);
	glBindVertexArray(vao);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, permTexture);
	glUniform1i(glGetUniformLocation(shader, "permTexture"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, simplexTexture);
	glUniform1i(glGetUniformLocation(shader, "simplexTexture"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gradTexture);
	glUniform1i(glGetUniformLocation(shader, "gradTexture"), 2);
	initMVP(shader, identityMatrix(), getViewMatrix());
	glUniform1f(glGetUniformLocation(shader, "systemTime"), glfwGetTime());
	
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST); 
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
	
	GLuint quadCubeVAO = initQuadCube();
	
	GLuint framebuffer = initFramebuffer();
	GLuint quadVAO = initQuad();
	GLuint sNoiseVAO = initNoise();
	
	GLuint permTexture = initPermTexture();
	GLuint simplexTexture = initSimplexTexture();
	GLuint gradTexture = initGradTexture();
	
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
	
	clock_t start,end;
	float time_spent;
	start=clock();
	//glViewport(0, 0, 512, 512);
	//glViewport(0, 0, getWindowWidth(), getWindowHeight());
	
	float degToRad = M_PI / 180.0;
	float innerRad = 32.5;
	float width = 15.0;
	float height = 1.0;
	int instancedDraws = 720;
	vec3 pos1[instancedDraws];
	for(int i = 0; i < instancedDraws; i++){
		float deg = i * degToRad;
		pos1[i].x = cos(deg)*innerRad+(-(((float)rand()/(float)(RAND_MAX)) * width));
		pos1[i].y = sin(deg)*innerRad+(-(((float)rand()/(float)(RAND_MAX)) * width));
		pos1[i].z = -(((float)rand()/(float)(RAND_MAX)) * height);
	}
	vec3 pos2[instancedDraws];
	for(int i = 0; i < instancedDraws; i++){
		float deg = i * degToRad;
		pos2[i].x = cos(deg)*innerRad+(-(((float)rand()/(float)(RAND_MAX)) * width));
		pos2[i].y = sin(deg)*innerRad+(-(((float)rand()/(float)(RAND_MAX)) * width));
		pos2[i].z = -(((float)rand()/(float)(RAND_MAX)) * height);
	}
	vec3 pos3[instancedDraws];
	for(int i = 0; i < instancedDraws; i++){
		float deg = i * degToRad;
		pos3[i].x = cos(deg)*innerRad+(-(((float)rand()/(float)(RAND_MAX)) * width));
		pos3[i].y = sin(deg)*innerRad+(-(((float)rand()/(float)(RAND_MAX)) * width));
		pos3[i].z = -(((float)rand()/(float)(RAND_MAX)) * height);
	}
	
	vec3 xAxis = {1.0, 0.0, 0.0};
	vec3 yAxis = {0.0, 1.0, 0.0};
	vec3 zAxis = {0.0, 0.0, 1.0};
	
	quaternion xRot[instancedDraws], yRot[instancedDraws];
	for(int i = 0; i < instancedDraws; i++){
		xRot[i] = angleAxis((-(((float)rand()/(float)(RAND_MAX)) * M_PI)), xAxis, zAxis);
		yRot[i] = angleAxis((-(((float)rand()/(float)(RAND_MAX)) * M_PI)), yAxis, zAxis);
	}
	
	mat4 rotations[instancedDraws];
	for(int i = 0; i < instancedDraws; i++) {
		rotations[i] = quaternionToRotation(quatMultiply(xRot[i], yRot[i]));//multiplymat4(rotateX(rotXarray[i]), multiplymat4(rotateY(rotYarray[i]), rotateZ(rotZarray[i])));
	}
	
	float scaleArray[instancedDraws];
	for(int i = 0; i < instancedDraws; i++){
		scaleArray[i] = (-(((float)rand()/(float)(RAND_MAX))));
	}
	
	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);
	
	end=clock();
	time_spent=(((float)end - (float)start) / 1000000.0F );
	printf("\nSystem time is at %f seconds\n", time_spent);
	
	mat4 model, atmo;
	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawNoise(sNoiseVAO, noiseRenderShader, 6, permTexture, simplexTexture, gradTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
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
		
		//drawNoise(sNoiseVAO, noiseRenderShader, 6, permTexture, simplexTexture, gradTexture);
		model = multiplymat4(translate(-10.0, 0.0, 0.0), multiplymat4(rotateX(theta), rotateY(theta)));
		draw(cubeVAO, ringShader, 36, earthTex, model, translation);
		
		model = multiplymat4(translate(5.0, 5.0, 5.0), scale(10.0));
		draw(quadCubeVAO, ringShader, qc.vertexNumber, ringTex, model, translation);
		
		//model = multiplymat4(multiplymat4(translatevec3(translation), rotateX(65.0)), scale(fScale*1.5));
		model = multiplymat4(translatevec3(translation), scale(fScale*1.5));
		draw(ringVAO, ringShader, planetRing.vertexNumber, ringTex, model, translation);
		model = multiplymat4(translatevec3(translation), scale(fScale));
		drawTess(sphereVAO, tessShader, planet.vertexNumber, textureColorBuffer, model, translation);
		atmo = multiplymat4(translatevec3(translation), scale(fScale*fScaleFactor));
		drawAtmoshere(sphereVAO, atmosphereShader, skyShader, planet.vertexNumber, atmo, translation, fScale, fScaleFactor);
		
		model = multiplymat4(translate(-75.0, 25.0, 0.0), scale(10.0));
		draw(quadVAO, fboShader, 6, textureColorBuffer, model, translation);
		
		//vec4 cc = getCameraPosition(translate(-10.0, 0.0, 0.0));
		
		//model = multiplymat4(multiplymat4(translate(cc.x, cc.y, cc.z), scale(1.0)), getViewRotation());
		model = multiplymat4(translate(-75.0, 25.0, 0.0), scale(5.0));
		draw(objectVAO, ringShader, object.vertexNumber, earthTex, model, translation);
		
		glUseProgram(instanceShader);
		drawInstanced(rockVAO, positionsVBO, instanceShader, object.vertexNumber, instancedDraws, pos1, rotations, model, scaleArray, theta);
		drawInstanced(rock2VAO, positionsVBO, instanceShader, object.vertexNumber, instancedDraws, pos2, rotations, model, scaleArray, theta);
		drawInstanced(rock3VAO, positionsVBO, instanceShader, object.vertexNumber, instancedDraws, pos3, rotations, model, scaleArray, theta);
		
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