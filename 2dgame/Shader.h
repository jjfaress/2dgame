#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {
public:
	unsigned int ID;
	Shader() {}
	Shader& use();
	void compile(const char* vertexSource, const char* fragmentSource);
	void setInt(const char* name, int val, bool useShader = false);
	void setVec3(const char* name, const glm::vec3 val, bool useShader = false);
	void setMat4(const char* name, const glm::mat4& mat, bool useShader = false);
private:
	void checkCompileErrors(unsigned int object, std::string type) const;
};