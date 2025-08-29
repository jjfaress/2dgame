#include "ConfigLoader.h"
#include <iostream>
#include "stb_image.h"

ConfigLoader* ConfigLoader::instance = nullptr;

ConfigLoader::ConfigLoader(const char* path)
{
	try
	{
		YAML::Node config = YAML::LoadFile(path);
		if (config["tile_data"])
		{
			loadTileData(config["tile_data"]);
		}
		if (config["bitmap"])
		{
			loadBitmap(config["bitmap"].as<std::string>().c_str());
		}
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "Config not loaded :: YAML error: " << e.what() << std::endl;
	}
}

ConfigLoader& ConfigLoader::getInstance(const char* path)
{
	static ConfigLoader instance(path);
	return instance;
}

void ConfigLoader::loadTileData(const YAML::Node& node)
{
	for (const auto& pair : node)
	{
		glm::vec4 color = pair.second["bitColor"].as<glm::vec4>();
		int id = pair.first.as<int>();
		std::string texture = pair.second["texture"].as<std::string>();
		this->aliases[color] = id;
		this->textures[id] = texture;
		this->tileTypes.push_back(id);
	}
}

void ConfigLoader::loadBitmap(const char* file)
{
	glm::ivec2 directions[] = {
		glm::ivec2(0, 1), //north
		glm::ivec2(1, 1), //north east
		glm::ivec2(1, 0), //east
		glm::ivec2(1, -1), //south east
		glm::ivec2(0, -1), //south
		glm::ivec2(-1, -1), //south west
		glm::ivec2(-1, 0), //west
		glm::ivec2(-1, 1) //north west
	};

	int width, height, channels;
	unsigned char* data = stbi_load(file, &width, &height, &channels, 0);
	if (!data)
	{
		std::cerr << "failed to load bitmap :: " << stbi_failure_reason() << std::endl;
		return;
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int sourceIdx = (y * width + x) * channels;  // images are stored as 1d arrays in memory
			unsigned char r = data[sourceIdx];
			unsigned char g = data[sourceIdx + 1];
			unsigned char b = data[sourceIdx + 2];
			unsigned char a = data[sourceIdx + 3];
			glm::vec4 sourceColor(r, g, b, a);
			int source = this->aliases[sourceColor];
			for (int dir = 0; dir < 8; dir++)
			{
				glm::ivec2 direction = directions[dir];
				int neighborX = x + direction.x;
				int neighborY = y + direction.y;

				if (neighborY < 0) neighborY = height - 1;
				if (neighborY >= height) neighborY = 0;
				if (neighborX < 0) neighborX = width - 1;
				if (neighborX >= width) neighborX = 0;
				int neighborIdx = (neighborY * width + neighborX) * channels;

				unsigned char r = data[neighborIdx];
				unsigned char g = data[neighborIdx + 1];
				unsigned char b = data[neighborIdx + 2];
				unsigned char a = data[neighborIdx + 3];
				glm::vec4 neighborColor(r, g, b, a);
				int neighbor = this->aliases[neighborColor];
				this->validNeighbors[source][dir].insert(neighbor);
				++this->tileFrequency[source][dir][neighbor];
			}
		}
	}
	for (const auto& sourcePair : this->tileFrequency)
	{
		int source = sourcePair.first;
		for (const auto& dirPair : sourcePair.second)
		{
			int dir = dirPair.first;

			int totalNeighbors = 0;

			for (const auto& neighborPair : dirPair.second)
			{
				totalNeighbors += neighborPair.second;
			}

			for (const auto& neighborPair : dirPair.second)
			{
				int neighbor = neighborPair.first;
				int freq = neighborPair.second;
				float weight = static_cast<float>(freq) / static_cast<float>(totalNeighbors);
				this->weights[source][dir][neighbor] = weight;
			}
		}
	}
}
