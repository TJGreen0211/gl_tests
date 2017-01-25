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
mat4 matArray[5];

float zNear = 0.5, zFar = 100000.0;
float thetaY = 0.0;
float zoom = 1;

float rad = 50.0;
float sphereSize = 10.0;
float rotX;
float rotY;
float rotZ;

GLuint lightShader, lightShader;
GLuint vPosition, vNormal, v1, v2, v3, v4;
GLuint atmosphereVAO, atmosphereVBO;
GLuint ModelView, projection, model, view;
mat4 mv, p, m, v;

float fScale = 10.0;
vec3 translation;

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
	
	for(int i = 0; i < 5; i++) {
		float r = rad*(i+1.0);
		translation.x = r * cos(theta);
		translation.y = 0.0;
		translation.z = r * sin(theta);
		mat4 matT = multiplymat4(translatevec3(translation), scale(sphereSize));
		mat4 matR = multiplymat4(rotateY(theta), rotateX(45.0));
		m = multiplymat4(matT,matR);
		matArray[i] = m;
	}
	
	createShader(&lightShader, "shaders/light.vert",
		"shaders/light.frag");
		
	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * 3.0, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//Use existing VAO. - BufferSub data?

	glGenVertexArrays(1, &atmosphereVAO);
	glBindVertexArray(atmosphereVAO);
	glGenBuffers(1, &atmosphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, atmosphereVBO);
	glBufferData(GL_ARRAY_BUFFER, planet.size + planet.nsize + sizeof(matArray), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, planet.size, planet.points);
	glBufferSubData(GL_ARRAY_BUFFER, planet.size, planet.nsize, planet.normals);
	glBufferSubData(GL_ARRAY_BUFFER, planet.size+planet.nsize, sizeof(matArray), matArray);
	
	//printf("\n%lu\n", sizeof(mat4));
	//printf("\n%lu\n", 4*sizeof(vec4));
	
	vPosition = glGetAttribLocation(lightShader, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
    	  
    vNormal = glGetAttribLocation(lightShader, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(planet.size));
	
	long unsigned int nextOffset = 0;
	for(int i = 0; i < 5; i++)
	{
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), BUFFER_OFFSET(nextOffset+planet.size+planet.nsize));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), BUFFER_OFFSET(nextOffset+planet.size+planet.nsize+sizeof(vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), BUFFER_OFFSET(nextOffset+planet.size+planet.nsize+sizeof(vec4)*2));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), BUFFER_OFFSET(nextOffset+planet.size+planet.nsize+sizeof(vec4)*3));
	
		nextOffset += sizeof(mat4);
	
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		
		glBindVertexArray(0);
	}
	
	
	
    glEnable(GL_DEPTH_TEST);
}

void setupLighting(int prog)
{	
    vec4 light_ambient = {0.2, 0.2, 0.2, 1.0};
    vec4 light_diffuse = {1.0, 1.0, 1.0, 1.0};
    vec4 light_specular = {1.0, 1.0, 1.0, 1.0};
    
    vec4 material_ambient = {0.2, 0.2, 1.0, 1.0};
    vec4 material_diffuse = {0.8, 0.8, 0.8, 1.0};
    vec4 material_specular = {0.5, 0.5, 0.5, 1.0};
    
    vec4 light_position = {0.0, 0.0, 1.0, 1.0}; 
    float material_shininess = 50.0f;
    
    vec4 ambient_product = multiplyvec4(light_ambient, material_ambient);
    vec4 diffuse_product = multiplyvec4(light_diffuse, material_diffuse);
    vec4 specular_product = multiplyvec4(light_specular, material_specular);
    
    glUniform4fv( glGetUniformLocation(prog, "ambientProduct"), 1, (float*)(&ambient_product) );
    glUniform4fv( glGetUniformLocation(prog, "diffuseProduct"), 1, (float*)(&diffuse_product) );
    glUniform4fv( glGetUniformLocation(prog, "specularProduct"), 1, (float*)(&specular_product) );
	glUniform4fv( glGetUniformLocation(prog, "lightPos"), 1, (float*)(&light_position) );
	glUniform1f ( glGetUniformLocation(prog, "shininess"), material_shininess );
}

void createPerspectiveMatrix()
{
	p = perspective(45.0, ASPECT, zNear, zFar);
}

void initMVP(int shader, mat4 m, mat4 v)
{
	glUniformMatrix4fv(glGetUniformLocation( shader, "projection" ), 1, GL_FALSE, &p.m[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation( shader, "model" ), 1, GL_FALSE, &m.m[0][0] );
	glUniformMatrix4fv(glGetUniformLocation( shader, "view" ), 1, GL_FALSE, &v.m[0][0] );
}

void bindTexture(GLuint activeTex, GLuint tex) {
    glActiveTexture(activeTex);
    glBindTexture(GL_TEXTURE_2D, tex);
}

void drawSphere()
{
	
	//float s = (float)rand()/(float)RAND_MAX;
	glUseProgram(lightShader);
	setupLighting(lightShader);
	v = getViewMatrix();	
	
	for(int i = 0; i < 5; i++) {
		
		//initMVP(lightShader, m, v);
		
		glUniformMatrix4fv(glGetUniformLocation( lightShader, "projection" ), 1, GL_FALSE, &p.m[0][0]);
		glUniformMatrix4fv(glGetUniformLocation( lightShader, "model" ), 1, GL_FALSE, &matArray[i].m[0][0] );
		glUniformMatrix4fv(glGetUniformLocation( lightShader, "view" ), 1, GL_FALSE, &v.m[0][0] );
	
		glBindVertexArray (atmosphereVAO);
		//bindTexture(GL_TEXTURE0, planetInstanceArray[i].texture);
		//bindTexture(GL_TEXTURE1, planetInstanceArray[i].normal);
		//glUniform1i(glGetUniformLocation(lightShader, "tex"), 0);
		//glUniform1i(glGetUniformLocation(lightShader, "normalTex"), 1);
		glDrawArrays( GL_TRIANGLES, 0, planet.vertexNumber);
		glDrawElementsInstanced(GL_TRIANGLES, planet.vertexNumber, GL_UNSIGNED_INT, 0, 5);
		glBindVertexArray(0);
	}
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
	chdir("/Users/tjgreen/Documents/OpenGL/gl_tests/instancing");
	
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
		//translation.x = 0.0;
		//translation.y = 0.0;
		//translation.z = -10.0;

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		drawSphere();
		glDisable(GL_BLEND);
		
		glfwSwapBuffers(window);
		
		theta += 0.001;
	}
	
	glDeleteVertexArrays(1, &atmosphereVAO);
    glDeleteBuffers(1, &atmosphereVBO);
	
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
