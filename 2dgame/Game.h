#pragma once
#include "TiledMap.h"
//#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Character.h"

using uint = unsigned int;

enum GameState {
	HUB,
	DUNGEON,
	MENU
};

class Game {
public:
	GameState State = HUB;
	bool keys[512] = {};
	bool buttons[16] = {};
	uint WIDTH, HEIGHT;
	std::vector<CollisionObject> colliders;
	Texture2D map;

	Game(uint screen_width, uint screen_height);
	~Game();

	void init();
	void update(float dt);
	void tick();
	void render(float alpha);
	void processInput(float dt);
	void updateCamera(float alpha);
	void checkCollision(std::vector<CollisionObject>& objects);

private:
	Character player;
	Camera camera;
	SpriteRenderer renderer;
};
