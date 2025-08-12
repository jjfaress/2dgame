#include "Shader.h"
#include<glad/glad.h>

Shader& Shader::use()
{
	glUseProgram(this->ID);
	return *this;
}

void Shader::compile(const char* vertexSource, const char* fragmentSource)
{
	unsigned int vertShader, fragShader;

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertexSource, NULL);
	glCompileShader(vertShader);
	checkCompileErrors(vertShader, "VERTEX");

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentSource, NULL);
	glCompileShader(fragShader);
	checkCompileErrors(fragShader, "FRAGMENT");

	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertShader);
	glAttachShader(this->ID, fragShader);
	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

}

void Shader::checkCompileErrors(unsigned int object, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER compilation error on " << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->ID, 1024, NULL, infoLog);
			std::cerr << "ERROR::PROGRAM linking error on " << type << "\n" << infoLog << std::endl;
		}
	}
}