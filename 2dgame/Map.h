#pragma once
#ifndef MAP_H
#define MAP_H
#include <vector>
#include "ConfigLoader.h"
#include <random>
#include "Texture.h"
#include <glm/glm.hpp>
#include "SpriteRenderer.h"

typedef unsigned int uint;

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
	void collapse(int x, int y, std::mt19937 rng, int& collapseCount);
	void collapse(int x, int y, uint& seed, int& collapseCount);
	void propagate(int x, int y, int& collapseCount);
	void postProcess();
	void findLowestEntropy();
};
#endif
