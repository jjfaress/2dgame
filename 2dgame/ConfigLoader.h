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
	std::unordered_map<std::string, int> aliases;

	ConfigLoader(const ConfigLoader&) = delete;
	ConfigLoader& operator=(const ConfigLoader&) = delete;

	static ConfigLoader& getInstance(const std::string& path);

private:
	static ConfigLoader* instance;
	ConfigLoader(const std::string& path);
	void loadValidNeighbors(const YAML::Node& node);
	void loadTileTypes(const YAML::Node& node);
	void loadAliases(const YAML::Node& node);
};
#endif