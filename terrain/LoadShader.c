#include "LoadShader.h"

char *readShader(char *filename)
{
	FILE *fp;
	char *content = NULL;
	
	int count = 0;
	if(filename != NULL)
	{
		fp = fopen(filename, "rt");
		if(fp != NULL){
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) {
            	content = (char *)malloc(sizeof(char) * (count+1));
            	count = fread(content,sizeof(char),count,fp);
            	content[count] = '\0';
        	}
        }
        fclose(fp);
	}
	return content;	
}

GLuint LoadShader(char *shaderPath, int shaderType)
{	
	GLuint shader = glCreateShader(shaderType);
	
	char *shaderStr = readShader(shaderPath);
	
	printf("Compiling %d, %s shader\n", shaderType, shaderPath);
	GLint length = strlen(shaderStr);
	glShaderSource(shader, 1, (const char **)&shaderStr, &length);
	glCompileShader(shader);
	
	//Check shader
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		char log[1024];
		glGetShaderInfoLog(shader, sizeof(log), 0, log);
		printf("Shader compile info: \n %s \n", log);
		glDeleteShader(shader);
		return 1;
	}
	return shader;
	/*
	printf("Linking program\n");
    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    
    GLint programSuccess;
	glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
	if(programSuccess == GL_FALSE) {
		char log[1024];
		glGetProgramInfoLog(program, sizeof(log), 0, log);
		printf("shader link info: \n %s \n", log);
		glDeleteProgram(program);
		return 1;
	}
	
	//printf("%s", vertShaderStr);
	//printf("%s", fragShaderStr);
	return shader;*/
}