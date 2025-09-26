#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <unordered_set>
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

template<typename T>
using Grid = std::vector<std::vector<T>>;

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

struct Pattern {
	Grid<int> tiles;
	int id;
	int frequency = 0;
	Pattern() {}
	Pattern(int id) : id(id) {}

	bool operator==(const Pattern& other) const
	{
		return tiles == other.tiles;
	}
};

struct PatternHash {
	size_t operator()(const Pattern& pattern) const
	{
		size_t hash = 0;
		for (size_t i = 0; i < pattern.tiles.size(); i++)
		{
			for (size_t j = 0; j < pattern.tiles[0].size(); j++)
			{
				size_t tileHash = std::hash<int>{}(pattern.tiles[i][j]);
				size_t positionHash = std::hash<size_t>{}(i * pattern.tiles[i].size() * j);

				hash ^= (tileHash ^ positionHash) + 0x9e3779b9 + (hash << 6) + (hash << 2);
			}
		}
		return hash;
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
	unsigned int seed;
	int n = 3;
	int mapWidth, mapHeight;
	std::vector<int> tileTypes;
	std::vector<int> patternTypes;
	std::unordered_map<glm::vec4, int, Vec4Hash> tileIds;
	std::unordered_map<Pattern, int, PatternHash> patternIds;
	std::unordered_map<int, Pattern> patterns;
	umap<source, umap<direction, std::unordered_set<neighbor>>> validNeighbors;
	std::unordered_map<int, std::string> textures;

	ConfigLoader(const ConfigLoader&) = delete;
	ConfigLoader& operator=(const ConfigLoader&) = delete;

	static ConfigLoader& getInstance(const char* path);

private:
	ConfigLoader(const char* path);
	Pattern extractPattern(const Grid<int>& bitmap, int x, int y);
	Pattern rotate(const Pattern& pattern, int rot);
	Pattern flipHor(const Pattern& pattern);
	Pattern flipVer(const Pattern& pattern);
	std::vector<Pattern> generateSymmetries(const Pattern& base);
	bool patternIsUnique(Pattern& pattern);
	void loadTileData(const YAML::Node& node);
	void loadBitmap(const char* file);

};