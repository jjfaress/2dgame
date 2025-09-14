#pragma once
#ifndef MAP_H
#define MAP_H
#include "SpriteRenderer.h"
#include <vector>
#include "ConfigLoader.h"

template<typename T>
using Grid = std::vector<std::vector<T>>;

template<typename T>
class Map {
public:
	int WIDTH, HEIGHT;
	Grid<T> grid;
	Map(int width, int height) : WIDTH(width), HEIGHT(height) {}
	virtual ~Map() = default;
	virtual void init() = 0;
	virtual void draw(SpriteRenderer& renderer) = 0;

private:

protected:
	virtual void postProcess() = 0;
};

#endif
