#include "ConfigLoader.h"
#include <iostream>

bool ConfigLoader::loadFromFile(const std::string& fileName)
{
	try
	{
		YAML::Node config = YAML::LoadFile(fileName);
		if (config["tile_types"])
		{
			loadTileTypes(config["tile_types"]);
		}
		if (config["valid_neighbors"])
		{
			loadValidNeighbors(config["valid_neighbors"]);
		}
		else
		{
			std::cout << "no neighbors";
		}
		return true;
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "YAML error" << e.what() << std::endl;
		return false;
	}
}

void ConfigLoader::loadTileTypes(const YAML::Node& node)
{
}

void ConfigLoader::loadValidNeighbors(const YAML::Node& node)
{
	for (const auto& tileType : node)
	{
		std::vector<int> neighbors;
		for (const auto& neighbor : tileType)
		{
			neighbors.push_back(neighbor.as<int>());
		}
		this->validNeighbors[tileType.as<int>()] = neighbors;
	}
}