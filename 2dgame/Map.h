#pragma once
#ifndef MAP_H
#define MAP_H
#include "SpriteRenderer.h"
#include <vector>
#include "ConfigLoader.h"

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
	unsigned int seed;

	Map(int width, int height, unsigned int seed);
	ConfigLoader& config;
	void collapse(int x, int y);
	void propagate(int x, int y);
};
#endif
