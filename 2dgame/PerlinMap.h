#pragma once
#ifndef PERLIN_H
#define PERLIN_H
#include "Map.h"

struct PerlinTile : public Tile {
	glm::vec2 position;
};

class PerlinMap : public Map<PerlinTile> {
public:
	PerlinMap(int width, int height);
	void generate();

private:

};
#endif