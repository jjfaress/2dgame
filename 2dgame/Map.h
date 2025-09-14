#pragma once
#ifndef MAP_H
#define MAP_H
#include "SpriteRenderer.h"
#include <vector>
#include "ConfigLoader.h"
#include "GameObject.h"

template<typename T>
using Grid = std::vector<std::vector<T>>;

struct Tile {
	glm::vec2 position;
	Tile() : position(glm::vec2(0.0)) {}
	Tile(glm::vec2 pos) : position(pos) {}
	virtual ~Tile() = default;
};

template<typename T = Tile>
class Map {
public:
	int WIDTH, HEIGHT;
	Map(int width, int height) : WIDTH(width), HEIGHT(height) {}
	virtual ~Map() noexcept = default;
	virtual void init() = 0;
	virtual void draw(SpriteRenderer& renderer) = 0;

private:

protected:
	virtual void postProcess() = 0;
	Grid<T> grid;
};

#endif
