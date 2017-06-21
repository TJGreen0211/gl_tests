#include "Main.h"
#include <stdio.h>
#include <unistd.h>

int mousePosX, mousePosY;
GLuint shadowShader, framebufferShader;
GLuint textureColorBuffer;
int keys;
int actionPress;

GLuint loadTexture(char const * path)
{
    //Generate texture ID and load texture data 
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
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

GLuint generateTextureAttachment(int depth, int stencil) {
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
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, getWindowWidth(), getWindowHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

void initShaders() {
	createShader(&shadowShader, "shaders/shadow.vert",
		"shaders/shadow.frag");
		
	createShader(&framebufferShader, "shaders/framebuffer.vert",
		"shaders/framebuffer.frag");
}

GLuint initBuffers(GLuint shader, vec3 *vertices, vec3 *normals, vec2 *texCoords, int vertSize, int normSize, int texSize) {
	GLuint vbo, vao;
	GLuint vPosition, vTexCoords, vNormal;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertSize+normSize+texSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normSize, normals);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize+normSize, texSize, texCoords);
	
	vPosition = glGetAttribLocation(shader, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	vNormal = glGetAttribLocation(shader, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize));
	vTexCoords = glGetAttribLocation(shader, "vTexCoords");
	glEnableVertexAttribArray(vTexCoords);
	glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize));
	
	glBindVertexArray(0);
	return vao;
}

vec3 *generateSmoothNormals(vec3 vna[], vec3 *vertices, vec3 *normals, int size) {
	vec3 vn;
	for(int i = 0; i < size; i++) {
		vec3 tempvn = {0.0, 0.0, 0.0};
		vn = vertices[i];
		for(int j = 0; j < size; j++) {
			if(vn.x == vertices[j].x && vn.y == vertices[j].y && vn.z == vertices[j].z) {
				tempvn = plusequalvec3(tempvn, normals[j]);
			}
		}
		vna[i] = normalizevec3(tempvn);
	}
	return vna;
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

GLuint initFramebuffer() {
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	textureColorBuffer = generateTextureAttachment(0, 0);
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

GLuint initFloor(GLuint shader) {
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
    vao = initBuffers(shader, vertArray, vna, texArray, sizeof(vertices), sizeof(vertices), sizeof(texCoords));    
    return vao;
}

void initMVP(int shader, mat4 m, mat4 v) {
	float zNear = 0.5, zFar = 100000.0;
	mat4 p = perspective(45.0, getWindowWidth()/getWindowHeight(), zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &p.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &m.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &v.m[0][0]);
}

GLuint initCube(GLuint shader) {
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
    vao = initBuffers(shadowShader, vertArray, vna, texArray, sizeof(vertices), sizeof(vertices), sizeof(texCoords));    
    return vao;
}

mat4 floorModelspace(float theta) {
	mat4 m = multiplymat4(scale(25.0), rotateX(90.0));//multiplymat4(multiplymat4(scale(100.0), translate(-5.0, 0.0, 0.0)), rotateX(90));
	return m;
}

mat4 cubeModelspace(float theta) {
	mat4 m = multiplymat4(translate(-10.0, 0.0, 0.0), multiplymat4(rotateX(theta), rotateY(theta)));
	return m;
}

void draw(GLuint VAO, GLuint shader, GLuint vertices, GLuint texture, mat4 m) {
	glUseProgram(shader);
	
	//mat4 m = scale(2.0);
	initMVP(shader, m, getViewMatrix());
	
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(texture, "texture1"), 0);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
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

int main(int argc, char *argv[]) 
{
	float theta = 0.0;
	GLfloat deltaTime = 0.0;
	GLfloat lastFrame = 0.0;
	//printf("%s\n\n", argv[0]);
	chdir("/Users/tjgreen/Documents/OpenGL/gl_tests/framebuffers");
	
	GLFWwindow *window = setupGLFW();
	initShaders();
	GLuint cubeTex = loadTexture("shaders/tex2.png");
	GLuint floorTex = loadTexture("shaders/tex1.jpg");
	GLuint framebuffer = initFramebuffer();
	GLuint floorVAO = initFloor(shadowShader);
	GLuint screenVAO = initFloor(framebufferShader);
	GLuint cubeVAO = initCube(shadowShader);
	
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);
	glCullFace(GL_BACK);
	
	mat4 model;
	while(!glfwWindowShouldClose(window))
	{
		theta += 0.5;
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		doMovement(deltaTime);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glViewport(0, 0, getWindowWidth(), getWindowHeight());
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		model = floorModelspace(theta);
		draw(floorVAO, shadowShader, 6, floorTex, model);
		model = cubeModelspace(theta);
		draw(cubeVAO, shadowShader, 36, cubeTex, model);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		
		model = scale(25.0);
		draw(screenVAO, framebufferShader, 6, textureColorBuffer, model);
		model = floorModelspace(theta);
		draw(floorVAO, shadowShader, 6, floorTex, model);
		model = cubeModelspace(theta);
		draw(cubeVAO, shadowShader, 36, cubeTex, model);
		
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteVertexArrays(1, &cubeVAO);
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
		processMouseMovement(xpos, ypos);
	}
	mousePosX = xpos;
	mousePosY = ypos;
}