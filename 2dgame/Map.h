#pragma once
#ifndef MAP_H
#define MAP_H
#include <vector>
#include "ConfigLoader.h"
#include <random>
#include "Texture.h"
#include <glm/glm.hpp>
#include "SpriteRenderer.h"
#include <queue>

typedef unsigned int uint;

struct compare {
	bool operator()(const std::pair<int, glm::vec2>& a,
		const std::pair<int, glm::vec2>& b) const
	{
		return a.first > b.first;
	}
};

template<typename T, typename Container = std::vector<T>, typename Comp = std::greater<T>>
class EntropyQueue : public std::priority_queue<T, Container, Comp>
{
public:
	using Base = std::priority_queue<T, Container, Comp>;
	using Base::Base;

	bool remove(const T& value)
	{
		auto it = std::find(this->c.begin(), this->c.end(), value);

		if (it == this->c.end()) return false;
		if (it == this->c.begin()) this->pop();
		else
		{
			this->c.erase(it);
			std::make_heap(this->c.begin(), this->c.end(), this->comp);
		}
		return true;
	}
};


struct Tile {
	glm::vec2 position;
	std::vector<int> possibilities;
	bool collapsed = false;
	int type;
	const char* texture = nullptr;
	Tile(glm::vec2 position, std::vector<int>& poss) : position(position), possibilities(poss) {}
};

typedef std::vector<std::vector<Tile>> Grid;

class Map {
public:
	int WIDTH, HEIGHT;
	Grid grid;
	std::vector<Tile> finalTiles;
	bool isReady;
	Map(int width, int height, uint seed);
	void generate();
	void init();
	void draw(SpriteRenderer& renderer);

private:

	uint seed;
	ConfigLoader& config;
	EntropyQueue<std::pair<int, glm::vec2>, std::vector<std::pair<int, glm::vec2>>, compare> eq;
	void collapse(int x, int y, std::mt19937& rng, int& collapseCount);
	void collapse(int x, int y, uint& seed, int& collapseCount);
	void propagate(int x, int y, int& collapseCount);
	void postProcess();
	glm::vec2 getLowestEntropy();
};
#endif
