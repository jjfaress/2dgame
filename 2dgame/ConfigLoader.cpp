#include "ConfigLoader.h"
#include <iostream>

ConfigLoader* ConfigLoader::instance = nullptr;

ConfigLoader::ConfigLoader(const std::string& path)
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

ConfigLoader& ConfigLoader::getInstance(const std::string& path)
{
	if (instance == nullptr)
	{
		if (path.empty())
		{
			throw std::runtime_error("File path missing");
		}
		instance = new ConfigLoader(path);
	}
	return *instance;
}

void ConfigLoader::loadAliases(const YAML::Node& node)
{
	for (const auto& item : node)
	{
		std::string name = item.begin()->first.as<std::string>();
		int id = item.begin()->second.as<int>();
		this->aliases[name] = id;
		this->tiletypes.push_back(id);
	}
}

void ConfigLoader::loadValidNeighbors(const YAML::Node& node)
{
	for (const auto& pair : node)
	{
		std::string sourceName = pair.first.as<std::string>();
		int sourceId = aliases[sourceName];
		std::vector<std::string> neighborsNames = pair.second.as<std::vector<std::string>>();
		std::vector<int> neighbors;
		for (auto name : neighborsNames)
		{
			neighbors.push_back(aliases[name]);
		}
		this->validNeighbors[sourceId] = neighbors;
	}
}