#pragma once
#ifndef PERLIN_H
#define PERLIN_H
#include "Map.h"

class PerlinMap {
public:
	int WIDTH, HEIGHT;
	PerlinMap(int width, int height);
	void generate();


private:
	struct Tile {
		double fertility;
	};
};
#endif