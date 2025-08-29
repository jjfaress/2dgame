#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <unordered_set>
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>


namespace YAML {
	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

struct Vec4Hash {
	std::size_t operator()(const glm::vec4& v) const noexcept
	{
		std::size_t h1 = std::hash<float>{}(v.x);
		std::size_t h2 = std::hash<float>{}(v.y);
		std::size_t h3 = std::hash<float>{}(v.z);
		std::size_t h4 = std::hash<float>{}(v.w);
		return ((h1 ^ (h2 << 1)) ^ (h3 << 1)) ^ (h4 << 1);
	}
};

struct Vec4Eq {
	bool operator()(const glm::vec4& a, const glm::vec4& b) const noexcept
	{
		return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
	}
};

template <typename T, typename V>
using umap = std::unordered_map<T, V>;
using direction = int;
using source = int;
using neighbor = int;

class ConfigLoader
{
public:
	std::vector<int> tileTypes;
	umap<source, umap<direction, std::unordered_set<neighbor>>> validNeighbors;
	//umap<source, umap<direction, umap<neighbor, float>>> weights;
	umap<int, float> overallWeights;

	umap<int, std::string> textures;

	ConfigLoader(const ConfigLoader&) = delete;
	ConfigLoader& operator=(const ConfigLoader&) = delete;

	static ConfigLoader& getInstance(const char* path = nullptr);

private:
	//umap<source, umap<direction, umap<neighbor, int>>> tileFrequency;
	umap<source, int> freq;
	std::unordered_map<glm::vec4, int, Vec4Hash> aliases;

	static ConfigLoader* instance;
	ConfigLoader(const char* path);
	void loadTileData(const YAML::Node& node);
	void loadBitmap(const char* file);
};
#endif