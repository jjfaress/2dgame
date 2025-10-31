#pragma once
#include "TiledMap.h"
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Player.h"

using uint = unsigned int;

enum GameState {
	HUB,
	DUNGEON,
	MENU
};

class Game {
public:
	GameState State= HUB;
	bool keys[512] = {};
	bool buttons[16] = {};
	uint WIDTH, HEIGHT;
	TiledMap::MapData level;

	Game(uint screen_width, uint screen_height);
	~Game();

	void init();
	void update(float dt);
	void tick();
	void processInput(float dt);

private:
	Character player;
	Camera camera;
	SpriteRenderer renderer;
};
