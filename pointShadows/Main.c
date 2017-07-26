#include "Main.h"
#include <stdio.h>
#include <unistd.h>

float nearPlane = 1.0f, farPlane = 75.0f;
int mousePosX, mousePosY;
GLuint shadowShader, framebufferShader, depthShader;
GLuint depthCubemap;
int keys;
int actionPress;
float zNear = 0.5, zFar = 100000.0;

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
	else if(depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, getWindowWidth(), getWindowHeight(), 0, attachment_type, GL_FLOAT, NULL);
		
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, getWindowWidth(), getWindowHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_BORDER
    glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

void initShaders() {
	createShader(&depthShader, "shaders/depth.vert",
		"shaders/depth.frag");

	createShader(&shadowShader, "shaders/shadow.vert",
		"shaders/shadow.frag");
		
	createShader(&framebufferShader, "shaders/framebuffer.vert",
		"shaders/framebuffer.frag");
}

GLuint initBuffers(GLuint shader, vec3 *vertices, vec3 *normals, vec2 *texCoords, int vertSize, int normSize, int texSize) {
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
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(vertSize));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(vertSize+normSize));
	
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

GLuint initDepthbuffer() {
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
    vao = initBuffers(shader, vertArray, vna, texArray, sizeof(vertices), sizeof(vertices), sizeof(texCoords));    
    return vao;
}

void initMVP(int shader, mat4 m, mat4 v) {
	mat4 p = perspective(45.0, getWindowWidth()/getWindowHeight(), zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &p.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &m.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &v.m[0][0]);
}

vec4 getCameraPosition(mat4 model) {
	mat4 mvTranspose = transposemat4(multiplymat4(model, getViewPosition()));
	vec4 inverseCamera = {-mvTranspose.m[3][0], -mvTranspose.m[3][1], -mvTranspose.m[3][2], -mvTranspose.m[3][3]};
	vec4 camPosition = multiplymat4vec4(mvTranspose, inverseCamera);
	return camPosition;
}

mat4 floorModelspace(float theta) {
	mat4 m = multiplymat4(scale(25.0), rotateX(90.0));//multiplymat4(multiplymat4(scale(100.0), translate(-5.0, 0.0, 0.0)), rotateX(90));
	return m;
}

mat4 cubeModelspace(float theta, float offsetX, float offsetZ) {
	mat4 m = multiplymat4(translate(offsetX, 0.0, offsetZ), multiplymat4(rotateX(theta), rotateY(theta)));
	return m;
}

void draw(GLuint VAO, GLuint shader, GLuint vertices, GLuint texture, mat4 m, mat4 *l) {
	glUseProgram(shader);
	initMVP(shader, m, getViewMatrix());
	if(shader == framebufferShader) {
		glUniform1f ( glGetUniformLocation(shader, "nearPlane"), nearPlane );
		glUniform1f ( glGetUniformLocation(shader, "farPlane"), farPlane );
	}
	vec4 cameraPos = getCameraPosition(m);
	
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthCubemap);
	glUniform1i(glGetUniformLocation(shadowShader, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shadowShader, "depthMap"), 1);
	glUniform3f(glGetUniformLocation(shader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	
	char *matToString[6] = {"lightSpace[0]", "lightSpace[1]", "lightSpace[2]", "lightSpace[3]", "lightSpace[4]", "lightSpace[5]"};
	for(int i = 0; i < 6; i++) {
		glUniformMatrix4fv(glGetUniformLocation(shader, matToString[i]), 1, GL_FALSE, &l[i].m[0][0]);
	}
	
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
}

mat4 getLookAtMatrix(vec4 lightPosition, vec3 lookAt) {
	vec3 d = {lightPosition.x - lookAt.x, lightPosition.y - lookAt.y, lightPosition.z - lookAt.z};
	d = normalizevec3(d);
	float rad = 180.0 / M_PI;
	float lightYaw = asin(-d.y) * rad;
	float lightPitch = atan2(d.x, d.z) * rad;
	mat4 rxry = multiplymat4(rotateX(lightYaw), rotateY(lightPitch));
	return multiplymat4(rxry, translatevec4(lightPosition));
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
	chdir("/Users/tjgreen/Documents/OpenGL/gl_tests/pointShadows");	
	
	GLFWwindow *window = setupGLFW();
	initShaders();
	GLuint cubeTex = loadTexture("shaders/tex2.png");
	GLuint floorTex = loadTexture("shaders/tex1.jpg");
	GLuint depthbuffer = initDepthbuffer();
	GLuint floorVAO = initFloor(shadowShader);
	GLuint screenVAO = initFloor(framebufferShader);
	GLuint cubeVAO = initCube(shadowShader);
	
	vec3 lookAt[6] = {{1.0, 0.0, 0.0}, {-1.0, 1.0, 0.0},{0.0, 1.0, 0.0},
					{0.0, -1.0, 0.0},{0.0, 0.0, 1.0},{0.0, 0.0, -1.0}};
	vec4 lightPosition = {0.0, 0.0, 0.0, 1.0};
	
	mat4 shadowProjection = perspective(90.0, 1024.0/1024.0, zNear, zFar);
	
	mat4 lightSpaceMatrix[6];
	for(int i = 0; i < 6; i++) {
		lightSpaceMatrix[i] = multiplymat4(shadowProjection, getLookAtMatrix(lightPosition, lookAt[i]));
	}
		
	int numCubes = 5;
	float posXArray[numCubes];
	for(int i = 0; i < numCubes; i++){
		posXArray[i] = -(((float)rand()/(float)(RAND_MAX)) * 40.0);
	}
	float posZArray[numCubes];
	for(int i = 0; i < numCubes; i++){
		posZArray[i] = -(((float)rand()/(float)(RAND_MAX)) * 40.0);
	}
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);
	glCullFace(GL_BACK);
	
	mat4 model;
	//mat4 lightProjection = ortho(-100.0, 100.0, -100.0, 100.0, zNear, zFar);
	while(!glfwWindowShouldClose(window))
	{
		theta += 0.5;
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		//glCullFace(GL_FRONT);
		doMovement(deltaTime);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glViewport(0, 0, 1024, 1024);
		
		glBindFramebuffer(GL_FRAMEBUFFER, depthbuffer);
			glClear(GL_DEPTH_BUFFER_BIT);
			model = floorModelspace(theta);
			draw(floorVAO, depthShader, 6, floorTex, model, lightSpaceMatrix);
			for(int i = 0; i < numCubes; i++) {
				model = cubeModelspace(theta, posXArray[i], posZArray[i]);
				draw(cubeVAO, depthShader, 36, cubeTex, model, lightSpaceMatrix);
			}
			model = multiplymat4(translate(0.0, -15.0, 0.0), scale(3.0));
			draw(cubeVAO, depthShader, 36, cubeTex, model, lightSpaceMatrix);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glCullFace(GL_BACK);
		
		glViewport(0, 0, getWindowWidth(), getWindowHeight());
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		
		model = scale(25.0);
		draw(screenVAO, framebufferShader, 6, depthCubemap, model, lightSpaceMatrix);
		model = floorModelspace(theta);
		draw(floorVAO, shadowShader, 6, floorTex, model, lightSpaceMatrix);
		for(int i = 0; i < numCubes; i++) {
			model = cubeModelspace(theta, posXArray[i], posZArray[i]);
			draw(cubeVAO, shadowShader, 36, cubeTex, model, lightSpaceMatrix);
		}
		model = multiplymat4(translate(0.0, -15.0, 0.0), scale(3.0));
		draw(cubeVAO, shadowShader, 36, cubeTex, model, lightSpaceMatrix);
		
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