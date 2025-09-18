#pragma once
#include "SpriteRenderer.h"
#include <vector>
#include "ConfigLoader.h"
#include "GameObject.h"

struct Tile {
	glm::vec2 position;
	const char* texture = nullptr;
	Tile(glm::vec2 pos) : position(pos) {}
	virtual ~Tile() = default;
};

template <typename T>
class Map {
public:
	int WIDTH, HEIGHT;
	Map(int width, int height, ConfigLoader& config) :
		WIDTH(width),
		HEIGHT(height),
		config(config)
	{
	}
	virtual ~Map() = default;
	virtual void init() = 0;
	virtual void draw(SpriteRenderer& renderer) = 0;

private:

protected:

	virtual void postProcess() = 0;
	ConfigLoader& config;
	Grid<T> grid;
};
