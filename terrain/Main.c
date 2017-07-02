#include "Main.h"
#include <stdio.h>
#include <unistd.h>

float zNear = 0.5, zFar = 100000.0;
int mousePosX, mousePosY, actionPress, keys;
struct sphere planet;

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

GLuint initLightShader() {
	GLuint shader;
	createShader(&shader, "shaders/light.vert",
		"shaders/light.frag");
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

GLuint initFloor() {
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

vec4 getCameraPosition(mat4 model) {
	mat4 mvTranspose = transposemat4(multiplymat4(model, getViewPosition()));
	vec4 inverseCamera = {-mvTranspose.m[3][0], -mvTranspose.m[3][1], -mvTranspose.m[3][2], -mvTranspose.m[3][3]};
	vec4 camPosition = multiplymat4vec4(mvTranspose, inverseCamera);
	return camPosition;
}

void initMVP(int shader, mat4 m, mat4 v) {
	mat4 p = perspective(45.0, getWindowWidth()/getWindowHeight(), zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &p.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &m.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &v.m[0][0]);
}


void draw(GLuint VAO, GLuint shader, GLuint vertices, GLuint texture, mat4 m) {
	glUseProgram(shader);
	initMVP(shader, m, getViewMatrix());
	vec4 cameraPos = getCameraPosition(m);
	
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
	glUniform3f(glGetUniformLocation(shader, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glBindVertexArray(0);
}

void drawTess(GLuint vao, GLuint shader, GLuint vertices, mat4 m) {
	glUseProgram(shader);
	initMVP(shader, m, getViewMatrix());
	glBindVertexArray(vao);
	glPatchParameteri(GL_PATCH_VERTICES, vertices);
	glDrawArrays(GL_PATCHES, 0, vertices);
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
	chdir("/Users/tjgreen/Documents/OpenGL/gl_tests/terrain");
	GLFWwindow *window = setupGLFW();
	
	GLuint lightShader = initLightShader();
	GLuint tessShader = initTessShader();
	
	GLuint planeTex = loadTexture("shaders/tex2.png");
	GLuint sphereTex = loadTexture("shaders/sky.jpg");
	GLuint sphereVAO = initSphere();
	GLuint planeVAO = initFloor();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	
	mat4 model;
	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	while(!glfwWindowShouldClose(window))
	{
		theta += 0.5;
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		doMovement(deltaTime);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		
		model = multiplymat4(translate(0.0, -15.0, 0.0), scale(3.0));
		draw(sphereVAO, lightShader, planet.vertexNumber, sphereTex, model);
		model = multiplymat4(scale(25.0), rotateX(90.0));
		draw(planeVAO, lightShader, 6, planeTex, model);
		
		model = scale(10.0);
		drawTess(sphereVAO, tessShader, planet.vertexNumber, model);
		
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
		processMouseMovement(xpos, ypos);
	}
	mousePosX = xpos;
	mousePosY = ypos;
}