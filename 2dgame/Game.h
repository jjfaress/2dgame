#pragma once
#include "TiledMap.h"
#include <GLFW/glfw3.h>
#include "Camera.h"

using uint = unsigned int;

enum GameState {
	HUB,
	DUNGEON,
	MENU
};


class Game {
public:
	GameState State;
	bool keys[512];
	bool buttons[16];
	uint WIDTH, HEIGHT;
	TiledMap* level;

	Game(uint screen_width, uint screen_height);
	~Game();

	void init();
	void update(float dt);
	void render();
	void processInput(float dt);

private:
	Camera* camera;
	SpriteRenderer* renderer;
};
