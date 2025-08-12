#pragma once
#ifndef SHADER_H
#define SHADER_H
#include <iostream>

class Shader {
public:
	unsigned int ID;
	Shader();
	Shader& use();
	void compile(const char* vertexSource, const char* fragmentSource);
private:
	void checkCompileErrors(unsigned int object, std::string type);
};
#endif