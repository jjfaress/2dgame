#pragma once
#ifndef LEVEL_H
#define LEVEL_H
#include "GameObject.h"
#include <unordered_map>
#include <vector>
#include "ConfigLoader.h"

struct Tile {
	glm::vec2 position;
	std::vector<int> possibilities;
	bool collapsed = false;
	int type;

	Tile(glm::vec2 position, std::vector<int> possibilities) : position(position), possibilities(possibilities) {}
};

class Level {
public:
	bool isReady;
	int WIDTH, HEIGHT;
	std::vector<std::vector<Tile>> tiles;
	unsigned int seed;
	ConfigLoader& config;

	Level(int width, int height, unsigned int seed);

	void collapse(int x, int y);
	void propagate(int x, int y);
	void draw(SpriteRenderer& renderer);
};
#endif
