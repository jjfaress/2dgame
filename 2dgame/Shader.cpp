#include "Shader.h"
#include<glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Shader& Shader::use()
{
	glUseProgram(this->ID);
	return *this;
}

void Shader::compile(const char* vertexSource, const char* fragmentSource)
{
	unsigned int vertShader, fragShader;
	//std::cout << vertexSource;
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertexSource, nullptr);
	glCompileShader(vertShader);
	checkCompileErrors(vertShader, "VERTEX");

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentSource, nullptr);
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

void Shader::checkCompileErrors(unsigned int object, std::string type) const
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

void Shader::setInt(const char* name, int val, bool useShader)
{
	if (useShader)
	{
		this->use();
	}
	glUniform1i(glGetUniformLocation(this->ID, name), val);
}

void Shader::setVec3(const char* name, const glm::vec3 val, bool useShader)
{
	if (useShader)
	{
		this->use();
	}
	glUniform3f(glGetUniformLocation(this->ID, name), val.x, val.y, val.z);
}

void Shader::setMat4(const char* name, const glm::mat4 &mat, bool useShader)
{
	if (useShader)
	{
		this->use();
	}
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(mat));
}