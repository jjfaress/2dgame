#pragma once
#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H
#include <unordered_map>
#include <yaml-cpp/yaml.h>

class ConfigLoader
{
public:
	std::vector<int> tiletypes;
	std::unordered_map<int, std::vector<int>> validNeighbors;
	bool loadFromFile(const std::string& fileName);
private:
	void loadTileTypes(const YAML::Node& node);
	void loadValidNeighbors(const YAML::Node& node);
};
#endif