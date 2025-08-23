#include "ConfigLoader.h"
#include <iostream>

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
		if (config["valid_neighbors"])
		{
			loadValidNeighbors(config["valid_neighbors"]);
		}
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "Config not loaded :: YAML error: " << e.what() << std::endl;
	}
}

ConfigLoader& ConfigLoader::getInstance(const char* path)
{

	if (instance == nullptr)
	{
		if (path == nullptr)
		{
			throw std::runtime_error("File path missing");
		}
		instance = new ConfigLoader(path);
	}
	return *instance;
}

void ConfigLoader::loadTileData(const YAML::Node& node)
{
	for (const auto& pair : node)
	{
		std::string name = pair.second["name"].as<std::string>();
		int id = pair.first.as<int>();
		std::string texture = pair.second["texture"].as<std::string>();
		this->aliases[name] = id;
		this->textures[id] = texture;
		this->tileTypes.push_back(id);
	}
}

void ConfigLoader::loadValidNeighbors(const YAML::Node& node)
{
	for (const auto& pair : node)
	{
		std::string sourceName = pair.first.as<std::string>();
		int sourceId = aliases[sourceName];
		std::vector<std::string> neighborsNames = pair.second.as<std::vector<std::string>>();
		std::unordered_set<int> neighbors;
		for (const auto& name : neighborsNames)
		{
			neighbors.insert(aliases[name]);
		}
		this->validNeighbors[sourceId] = neighbors;
	}
}
