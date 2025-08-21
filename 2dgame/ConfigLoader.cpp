#include "ConfigLoader.h"
#include <iostream>

ConfigLoader* ConfigLoader::instance = nullptr;

ConfigLoader::ConfigLoader(const char* path)
{
	try
	{
		YAML::Node config = YAML::LoadFile(path);
		if (config["aliases"])
		{
			loadAliases(config["aliases"]);
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

ConfigLoader& ConfigLoader::getInstance(const char* path = nullptr)
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

void ConfigLoader::loadAliases(const YAML::Node& node)
{
	for (const auto pair : node)
	{
		std::string name = pair.first.as<std::string>();
		int id = pair.second.as<int>();
		this->aliases[name] = id;
		this->tileTypes.push_back(id);
	}
}

void ConfigLoader::loadValidNeighbors(const YAML::Node& node)
{
	for (const auto pair : node)
	{

		std::string sourceName = pair.first.as<std::string>();
		int sourceId = aliases[sourceName];
		std::vector<std::string> neighborsNames = pair.second.as<std::vector<std::string>>();
		//std::vector<int> neighbors;
		std::unordered_set<int> neighbors;
		for (auto name : neighborsNames)
		{
			//neighbors.push_back(aliases[name]);
			neighbors.insert(aliases[name]);
		}
		this->validNeighbors[sourceId] = neighbors;
	}
}