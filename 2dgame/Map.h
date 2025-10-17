#pragma once
#include "SpriteRenderer.h"
#include <vector>
#include "GameObject.h"

template <typename T>
using Grid = std::vector<std::vector<T>>;

class Map {
public:
	int32_t WIDTH, HEIGHT;
	Map(int32_t width, int32_t height, SpriteRenderer& renderer) :
		WIDTH(width),
		HEIGHT(height),
		renderer(&renderer)
	{}

	Map(SpriteRenderer& renderer) : renderer(&renderer) {}

	virtual ~Map() = default;
	virtual void init() = 0;
	virtual void draw() = 0;

protected:
	SpriteRenderer* renderer;

};
