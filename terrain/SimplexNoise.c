#include "SimplexNoise.h"
#include <stdio.h>
#include <math.h>

static int perm[256]= {151,160,137,91,90,15,
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

static GLuint permTexture;
static GLuint simplexTexture;
static GLuint gradTexture;
static GLuint noiseRenderShader;
static GLuint sNoiseVAO;

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

GLuint initNoiseShader() {
	GLuint shader;
	GLuint vertShader = LoadShader("shaders/noise.vert", GL_VERTEX_SHADER);
	GLuint fragShader = LoadShader("shaders/noise.frag", GL_FRAGMENT_SHADER);
	shader = glCreateProgram();
	glAttachShader(shader, vertShader);
	glAttachShader(shader, fragShader);
	glLinkProgram(shader);

	return shader;
}

GLuint initSimplexTexture() {
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
	GLuint textureID;
  	glGenTextures(1, &textureID);
  	glBindTexture(GL_TEXTURE_1D, textureID);

  	glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, simplex4 );
  	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  	return textureID;
}

GLuint initPermTexture() {
	char *pixels;
	int grad3[16][3] = {{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
	                   {1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
	                   {1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0}, // 12 cube edges
	                   {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}}; // 4 more to make 16
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

GLuint initGradTexture() {
	char *pixels;
	int grad4[32][4]= {{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1}, // 32 tesseract edges
	                   {0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
	                   {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
	                   {-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
	                   {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
	                   {-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
	                   {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
	                   {-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}};
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

void initializeNoise() {
	noiseRenderShader = initNoiseShader();
	permTexture = initPermTexture();
	simplexTexture = initSimplexTexture();
	gradTexture = initGradTexture();
	sNoiseVAO = initNoise();
}

void drawNoise(GLuint vao, GLuint shader, int vertices, GLuint permTexture, GLuint simplexTexture, GLuint gradTexture, int animate) {
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
	glUniform1f(glGetUniformLocation(shader, "systemTime"), glfwGetTime());
	glUniform1i(glGetUniformLocation(shader, "animated"), animate);

	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}


void generateNoiseTexture(GLuint framebuffer, int animated) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawNoise(sNoiseVAO, noiseRenderShader, 6, permTexture, simplexTexture, gradTexture, animated);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
