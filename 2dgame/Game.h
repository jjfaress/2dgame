#pragma once
#ifndef GAME_H
#define GAME_H
#include "WFCMap.h"

using uint = unsigned int;

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game {
public:
	GameState State;
	bool Keys[1024];
	uint WIDTH, HEIGHT;

	Game(uint width, uint height);
	~Game();

	void init();
	void update(float dt);
	void render();
	void processInput();

};
#endif
