#pragma once
#include "SpriteRenderer.h"
#include <vector>
#include "GameObject.h"

template <typename T>
using Grid = std::vector<std::vector<T>>;

template <typename T>
class Map {
public:
	int WIDTH, HEIGHT;
	Map(int width, int height, SpriteRenderer& renderer) :
		WIDTH(width),
		HEIGHT(height),
		renderer(&renderer)
	{
	}
	virtual ~Map() = default;
	virtual void init() = 0;
	virtual void draw() = 0;

protected:
	SpriteRenderer* renderer;
	Grid<T> grid;
};
