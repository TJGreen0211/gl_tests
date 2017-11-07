#include "Window.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

GLFWwindow *setupGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	GLFWwindow* window = glfwCreateWindow(getWindowWidth(), getWindowHeight(), "Shadows", NULL, NULL);
	
	glfwMakeContextCurrent(window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_ENABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	return window;
}

float getWindowHeight() {
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return mode->height;
}

float getWindowWidth() {
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return mode->width;
}