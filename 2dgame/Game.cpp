#include "Game.h"
#include "ResourceManager.h"
#include <utility>


Game::Game(uint screen_width, uint screen_height) :
	WIDTH(screen_width),
	HEIGHT(screen_height)
{
}

Game::~Game()
{
	this->renderer.destroy();
}

void Game::init()
{
	this->camera.init(static_cast<float>(this->WIDTH), static_cast<float>(this->HEIGHT));

	Shader spriteShader =
		ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
	spriteShader.use().setInt("image", 0);
	this->renderer.init();
	this->renderer.setShader(spriteShader);

	std::string spriteDir("assets/sprites/");
	std::string mapPath(spriteDir + "map.json");
	this->level = TiledMap::loadMap(mapPath, spriteDir, this->renderer);

	this->player.sprite =
		ResourceManager::loadTexture((spriteDir + "cat.png").c_str(), "cat");
	this->player.position = this->level.POIs["playerSpawn"];

}

void Game::tick()
{
	switch (this->State)
	{
	case HUB:
		glm::vec2 cameraPos =
			this->player.position - glm::vec2(this->WIDTH / 2.0f, this->HEIGHT / 2.0f);
		if (cameraPos != this->camera.lastPosition)
		{
			this->camera.setPosition(cameraPos);
			this->renderer.shader.setMat4(
				"projection", this->camera.getProjectionMatrix());
			this->camera.lastPosition = cameraPos;
		}

		this->renderer.drawSprite(this->level.texture, glm::vec2(0));
		this->player.draw(this->renderer);


		break;
	}
}

void Game::processInput(float dt)
{
	Character& player = this->player;
	player.speed = 2.0;
	if (this->keys[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	}
	if (this->keys[GLFW_KEY_LEFT_SHIFT])
	{
		player.speed = 4.0;
	}

	if (this->keys[GLFW_KEY_W])
	{
		player.move({ 0.0, -1.0 });
	}
	if (this->keys[GLFW_KEY_S])
	{
		player.move({ 0.0, 1.0 });
	}
	if (this->keys[GLFW_KEY_A])
	{
		player.move({ -1.0, 0.0 });
	}
	if (this->keys[GLFW_KEY_D])
	{
		player.move({ 1.0, 0.0 });
	}
}