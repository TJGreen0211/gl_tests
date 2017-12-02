#include "Common.h"
#include "LoadShader.h"

//GLfloat deltaTime = 0.0f;
//GLfloat lastFrame = 0.0f;

void createShader(GLuint *shader, char *vert, char *frag)
{
	GLuint vertShader = LoadShader(vert, GL_VERTEX_SHADER);
    GLuint fragShader = LoadShader(frag, GL_FRAGMENT_SHADER);
    *shader = glCreateProgram();
    glAttachShader(*shader, vertShader);
    glAttachShader(*shader, fragShader);
    glLinkProgram(*shader);
}

/*
GLfloat frameCount()
{
	GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    return lastFrame;
}*/
