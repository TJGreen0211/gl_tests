#include "Main.h"
#include <stdio.h>
#include <unistd.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
GLuint loadShader(char *vertex_path, char *fragement_path);

int mousePosX, mousePosY;
int keys;
int actionPress;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

const float WIDTH = 1400, HEIGHT = 800;
float ASPECT = WIDTH/HEIGHT;
float theta;

sphere planet;

float zNear = 0.5, zFar = 100000.0;
float thetaY = 0.0;
float zoom = 1;

float rad = 50.0;
float rotX;
float rotY;
float rotZ;

GLuint planetShader, starShader;
GLuint vPosition, vNormal;
GLuint planetVAO, planetVBO, atmosphereVAO, atmosphereVBO;
GLuint ModelView, projection, model, view;
mat4 mv, p, m, v;

float fScale = 10.0;
vec3 translation;
mat4 IM = {
	{{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}}
};

void createShader(GLuint *shader, char *vert, char *frag)
{
	GLuint vertShader = LoadShader(vert, GL_VERTEX_SHADER);
    GLuint fragShader = LoadShader(frag, GL_FRAGMENT_SHADER);
    *shader = glCreateProgram();
    glAttachShader(*shader, vertShader);
    glAttachShader(*shader, fragShader);
    glLinkProgram(*shader);
}

float getScreenWidth()
{
	return HEIGHT;
}

float getScreenHeight()
{
	return WIDTH;
}

void init()
{
	planet = tetrahedron(5);
	
	createShader(&starShader, "shaders/star.vert",
		"shaders/star.frag");

	glGenVertexArrays(1, &atmosphereVAO);
	glBindVertexArray(atmosphereVAO);
	glGenBuffers(1, &atmosphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, atmosphereVBO);
	glBufferData(GL_ARRAY_BUFFER, planet.size + planet.nsize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, planet.size, planet.points);
	glBufferSubData(GL_ARRAY_BUFFER, planet.size, planet.nsize, planet.normals);
	
	vPosition = glGetAttribLocation(starShader, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));	  
    vNormal = glGetAttribLocation(starShader, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(planet.size));
	glBindVertexArray(0);
	
	
    glEnable(GL_DEPTH_TEST);
}

void createPerspectiveMatrix()
{
	p = perspective(45.0, ASPECT, zNear, zFar);
}

void initMVP(int shader, mat4 m, mat4 v)
{
	glUniformMatrix4fv(glGetUniformLocation( shader, "projection" ), 1, GL_FALSE, &p.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation( shader, "model" ), 1, GL_FALSE, &m.m[0][0] );
	glUniformMatrix4fv(glGetUniformLocation( shader, "view" ), 1, GL_FALSE, &v.m[0][0] );
}

void bindTexture(GLuint activeTex, GLuint tex) {
    glActiveTexture(activeTex);
    glBindTexture(GL_TEXTURE_2D, tex);
}

void drawAtmosphere()
{
	glUseProgram(starShader);
	
	v = getViewMatrix();
	
	float scaleFactor = 1.025;
	
	m = multiplymat4(translatevec3(translation), scale(fScale*scaleFactor));
	float fOuter = (fScale*scaleFactor);
	float fInner = (fScale);
	
	glUniform1f(glGetUniformLocation(starShader, "fInnerRadius"), fInner);
	glUniform1f(glGetUniformLocation(starShader, "fOuterRadius"), fOuter);
	glUniform3f(glGetUniformLocation(starShader, "translation"), translation.x, translation.y, translation.z);
	glUniform1f(glGetUniformLocation(starShader, "time"), glfwGetTime());

	initMVP(starShader, m, v);
	
	glBindVertexArray (atmosphereVAO);
	glDrawArrays( GL_TRIANGLES, 0, planet.vertexNumber);
	glBindVertexArray(0);
    
}

void doMovement()
{
	if(keys == GLFW_KEY_W && actionPress == GLFW_PRESS)
        processKeyboard(FORWARD, deltaTime);
    if(keys == GLFW_KEY_S && actionPress == GLFW_PRESS)
        processKeyboard(BACKWARD, deltaTime);
    if(keys == GLFW_KEY_A && actionPress == GLFW_PRESS)
        processKeyboard(LEFT, deltaTime);
    if(keys == GLFW_KEY_D && actionPress == GLFW_PRESS)
        processKeyboard(RIGHT, deltaTime);
}


GLFWwindow *setupGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Star", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	return window;
}

int main(int argc, char *argv[])
{
	chdir("/Users/tjgreen/Documents/OpenGL/gl_tests/star");
	
	GLFWwindow *window = setupGLFW();

	init();
	createPerspectiveMatrix();
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glCullFace(GL_BACK);
	while(!glfwWindowShouldClose(window))
	{	
		GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        
		glfwPollEvents();
		doMovement();
		glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, WIDTH, HEIGHT);
		
		/*translation.x = rad * cos(theta);
		translation.y = 0.0;
		translation.z = rad * sin(theta);*/
		translation.x = 0.0;
		translation.y = 0.0;
		translation.z = -10.0;

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		drawAtmosphere();
		glDisable(GL_BLEND);
		
		glfwSwapBuffers(window);
		
		theta += 0.001;
	}
	
	glDeleteVertexArrays(1, &atmosphereVAO);
    glDeleteBuffers(1, &atmosphereVBO);
	glDeleteVertexArrays(1, &planetVAO);
    glDeleteBuffers(1, &planetVBO);
	
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
    	doMovement();
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS){
    	keys = GLFW_KEY_S;
    	doMovement();
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS){
    	keys = GLFW_KEY_A;
    	doMovement();
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS){
    	keys = GLFW_KEY_D;
    	doMovement();
    }
    if(action == GLFW_RELEASE)
    	keys = 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		processMouseMovement(xpos, ypos);
	}
	mousePosX = xpos;
	mousePosY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom = processMouseScroll(yoffset, zoom);
}
