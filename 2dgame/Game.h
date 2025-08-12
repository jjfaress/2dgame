#pragma once
#ifndef GAME_H
#define GAME_H

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game {
public:
	GameState State;
	bool Keys[1024];
	unsigned int WIDTH, HEIGHT;
	Game(unsigned int width, unsigned int height);
	~Game();
	void init();
	void update(float dt);
	void render();
};
#endif
