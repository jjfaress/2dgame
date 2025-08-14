#pragma once
#ifndef LEVEL_H
#define LEVEL_H
#include "GameObject.h"
#include <vector>
#include <unordered_set>

class Level {
public:
	bool isReady;
	int WIDTH, HEIGHT;

	Level(int width, int height);

	//private:
	void init();
	void collapse(int x, int y);
	void propagate(int x, int y);
	void draw(SpriteRenderer& renderer);

};
#endif
