#pragma once
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
	WFCMap* level;

	Game(uint screen_width, uint screen_height);
	~Game();

	void init();
	void update(float dt);
	void render();
	void processInput();

};
