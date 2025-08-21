#pragma once
#ifndef MAP_H
#define MAP_H
#include "SpriteRenderer.h"
#include <vector>
#include "ConfigLoader.h"
#include <random>

typedef unsigned int uint;

struct Tile {
	glm::vec2 position;
	std::vector<int> possibilities;
	bool collapsed = false;
	int type;

	Tile(glm::vec2 position, std::vector<int>& poss) : position(position), possibilities(poss) {}
};

class Map {
public:
	bool isReady;
	int WIDTH, HEIGHT;
	std::vector<std::vector<Tile>> grid;
	Map(int width, int height, uint seed);
	void generate();

private:
	uint seed;
	ConfigLoader& config;
	void collapse(int x, int y, std::mt19937 rng, int &collapseCount);
	void collapse(int x, int y, uint seed, int& collapseCount);
	void propagate(int x, int y, int &collapseCount);
	void findLowestEntropy();
};
#endif
