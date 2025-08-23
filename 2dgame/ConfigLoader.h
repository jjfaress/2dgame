#pragma once
#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <unordered_set>
#include "Texture.h"

class ConfigLoader
{
public:
	std::vector<int> tileTypes;
	std::unordered_map<int, std::unordered_set<int>> validNeighbors;
	std::unordered_map<std::string, int> aliases;
	std::unordered_map<int, std::string> textures;

	ConfigLoader(const ConfigLoader&) = delete;
	ConfigLoader& operator=(const ConfigLoader&) = delete;

	static ConfigLoader& getInstance(const char* path = nullptr);
	
private:
	static ConfigLoader* instance;
	ConfigLoader(const char* path);
	void loadValidNeighbors(const YAML::Node& node);
	void loadTileData(const YAML::Node& node);
};
#endif