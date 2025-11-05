#include "ResourceManager.h"
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Shader ResourceManager::getShader(std::string name)
{
	return getShadersMap()[name];
}

std::map<std::string, Shader>& ResourceManager::getShadersMap()
{
	static std::map<std::string, Shader> shaders;
	return shaders;
}


Shader ResourceManager::loadShader(const char* vertexSource, const char* fragmentSource, std::string name)
{
	if (!getShadersMap().contains(name))
	{
		getShadersMap()[name] = loadShaderFromFile(vertexSource, fragmentSource);
	}
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
	return getTexturesMap()[name];
}

AnimationData ResourceManager::parseSpriteSheet(std::string& fileName, std::string& spriteDir)
{
	AnimationData ad;
	std::string stringPath(spriteDir + fileName);

	std::ifstream f(stringPath);
	if (!f.is_open())
	{
		std::cout << "ERROR: Could not open file: " << stringPath << std::endl;
		return AnimationData();  // Return empty
	}

	json data = json::parse(f);

	std::string image = data["meta"]["image"];
	ad.texture = loadTexture((spriteDir + image).c_str(), image);

	unsigned int sheetW = data["meta"]["size"]["w"];
	unsigned int sheetH = data["meta"]["size"]["h"];

	for (auto& frame : data["frames"])
	{
		FrameData fd;
		auto& temp = frame["frame"];
		float x = temp["x"];
		float y = temp["y"];
		float w = temp["w"];
		float h = temp["h"];

		fd.uvTopLeft = { x / sheetW, y / sheetH };
		fd.uvBotRight = { (x + w) / sheetW, (y + h) / sheetH };
		fd.size = { w, h };
		fd.duration = frame["duration"] * 0.001;

		ad.frames.push_back(fd);
	}

	for (auto& tag : data["meta"]["frameTags"])
	{
		ad.tags[tag["name"]] = { tag["from"], tag["to"] };
	}
	return ad;
}

std::map<std::string, Texture2D>& ResourceManager::getTexturesMap()
{
	static std::map<std::string, Texture2D> textures;
	return textures;
}

Texture2D ResourceManager::loadTexture(const char* file, std::string name, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);
	if (!getTexturesMap().contains(name))
	{
		getTexturesMap()[name] = loadTextureFromFile(file);
	}
	return getTexturesMap()[name];
}

Texture2D ResourceManager::loadTextureFromFile(const char* file)
{
	Texture2D texture;

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