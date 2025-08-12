#include "ResourceManager.h"
#include <fstream>
#include <sstream>
#include "stb_image.h"

std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Shader> ResourceManager::Shaders;

Shader ResourceManager::getShader(std::string name)
{
	return Shaders[name];
}

Shader ResourceManager::loadShader(const char* vertexSource, const char* fragmentSource, std::string name)
{
	Shaders[name] = loadShaderFromFile(vertexSource, fragmentSource);
	return Shaders[name];
}

Shader ResourceManager::loadShaderFromFile(const char* vertexSource, const char* fragmentSource)
{
	std::string vertTemp, fragTemp;
	try
	{
		std::ifstream vertexShaderFile(vertexSource);
		std::ifstream fragmentShaderFile(fragmentSource);
		std::stringstream vertStream, fragStream;
		vertStream << vertexShaderFile.rdbuf();
		fragStream << fragmentShaderFile.rdbuf();
		vertexShaderFile.close();
		fragmentShaderFile.close();
		vertTemp = vertStream.str();
		fragTemp = fragStream.str();
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER Failed to read shader source" << std::endl;
	}
	const char* vertexCode = vertTemp.c_str();
	const char* fragmentCode = fragTemp.c_str();
	Shader shader;
	shader.compile(vertexCode, fragmentCode);
	return shader;
}

Texture2D ResourceManager::getTexture(std::string name)
{
	return Textures[name];
}

Texture2D ResourceManager::loadTexture(const char* file, bool alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
	Texture2D texture;
	if (alpha)
	{
		texture.internalFormat = GL_RGBA;
		texture.imageFormat = GL_RGBA;
	}

	int width, height, channels;
	unsigned char* data = stbi_load(file, &width, &height, &channels, 0);
	if (!data)
	{
		std::cout << "ERROR::TEXTURE failed to load texture \n" << stbi_failure_reason << endl;
	}
	else
	{
		texture.generate(width, height, data);
		stbi_image_free(data);
		return texture;
	}
}

void ResourceManager::clear()
{
	for (auto item : Shaders)
	{
		glDeleteProgram(item.second.ID);
	}
	for (auto item : Textures)
	{
		glDeleteTextures(1, &item.second.ID);
	}
}