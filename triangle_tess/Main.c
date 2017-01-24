#include "Main.h"
#include <stdio.h>
#include <unistd.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
const GLuint WIDTH = 800, HEIGHT = 600;
GLuint program, vertShader, fragShader, teshShader, tcshShader;

sphere s;

int main()
{
	GLfloat vertices[] = {
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
       -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
        0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Top 
    };
    
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Main", NULL, NULL);
	glfwMakeContextCurrent(window);
	
	glfwSetKeyCallback(window, key_callback);
	
	glViewport(0, 0, WIDTH, HEIGHT);
	
	chdir("/Users/tjgreen/Documents/OpenGL/gl_tests/triangle_tess");
	
	vertShader = LoadShader("shaders/shader1.vert", GL_VERTEX_SHADER);
    tcshShader = LoadShader("shaders/shader1.tcsh", GL_TESS_CONTROL_SHADER);
	teshShader = LoadShader("shaders/shader1.tesh", GL_TESS_EVALUATION_SHADER);
    fragShader = LoadShader("shaders/shader1.frag", GL_FRAGMENT_SHADER);
	program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, tcshShader);
    glAttachShader(program, teshShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glUseProgram(program);
	
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);
	
	GLuint vColor = glGetAttribLocation(program, "vColor");
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(vColor);
	
	glBindVertexArray(0);
	
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindVertexArray(vao);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawArrays(GL_PATCHES, 0, 3);
		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
	}
	
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}