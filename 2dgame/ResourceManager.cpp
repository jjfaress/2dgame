#include "ResourceManager.h"
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>

//std::map<std::string, Texture2D> ResourceManager::Textures;
//std::map<std::string, Shader> ResourceManager::Shaders;

Shader ResourceManager::getShader(std::string name)
{
	return getShadersMap()[name];
	//return Shaders[name];
}

std::map<std::string, Shader>& ResourceManager::getShadersMap()
{
	static std::map<std::string, Shader> shaders;
	return shaders;
}



Shader ResourceManager::loadShader(const char* vertexSource, const char* fragmentSource, std::string name)
{
	getShadersMap()[name] = loadShaderFromFile(vertexSource, fragmentSource);
	return getShadersMap()[name];
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
	//return Textures[name];
	return getTexturesMap()[name];
}



std::map<std::string, Texture2D>& ResourceManager::getTexturesMap()
{
	static std::map<std::string, Texture2D> textures;
	return textures;
}

Texture2D ResourceManager::loadTexture(const char* file, bool alpha, std::string name)
{
	getTexturesMap()[name] = loadTextureFromFile(file, alpha);
	return getTexturesMap()[name];
}



//Texture2D ResourceManager::loadTexture(const char* file, bool alpha, std::string name)
//{
//	if (Textures.find(name) == Textures.end())
//	{
//		Textures[name] = loadTextureFromFile(file, alpha);
//	}
//	
//	return Textures[name];
//}

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
		std::cout << "ERROR::TEXTURE failed to load texture \n" << stbi_failure_reason << std::endl;
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
	auto& shaders = getShadersMap();
	auto& textures = getTexturesMap();

	for (auto item : shaders)
	{
		glDeleteProgram(item.second.ID);
	}
	for (auto item : textures)
	{
		glDeleteTextures(1, &item.second.ID);
	}
	shaders.clear();
	textures.clear();
}