#pragma once
#include "Shader.h"
#include "Texture.h"
#include <map>

class ResourceManager {
public:
	static Shader loadShader(const char* vertexSource, const char* fragmentSource, std::string name);
	static Shader getShader(std::string name);
	static Texture2D loadTexture(const char* file, bool alpha, std::string name);
	static Texture2D getTexture(std::string name);
	static void clear();

private:
	ResourceManager() {}
	static std::map<std::string, Texture2D>& getTexturesMap();
	static std::map<std::string, Shader>& getShadersMap();
	static Shader loadShaderFromFile(const char* verTexSource, const char* fragmentSource);
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};