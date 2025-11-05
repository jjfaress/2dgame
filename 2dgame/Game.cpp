#include "Game.h"
#include "ResourceManager.h"
#include <utility>
#include <array>

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
	this->player.init("cat.json", spriteDir);
	this->level = TiledMap::loadMap(mapPath, spriteDir, this->renderer);
	this->player.position = this->level.POIs["playerSpawn"];

	this->collisions = Collision::buildSpacialGrid(this->level.worldCollisions, 32);

}

void Game::update(float dt)
{
	this->player.update(dt);
}

void Game::render()
{
	switch (this->State)
	{
	case HUB:
		updateCamera();
		this->renderer.drawSprite(this->level.texture, glm::vec2(0));
		this->player.draw(this->renderer);

		break;
	}
}

void Game::processInput(float dt)
{
	Character& player = this->player;
	if (this->keys[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	}

	glm::vec2 inputDir(0);

	if (this->keys[GLFW_KEY_W]) inputDir.y -= 1;
	if (this->keys[GLFW_KEY_A]) inputDir.x -= 1;
	if (this->keys[GLFW_KEY_S]) inputDir.y += 1;
	if (this->keys[GLFW_KEY_D]) inputDir.x += 1;

	if (glm::length(inputDir) > 0)
	{
		player.velocity = glm::normalize(inputDir);

		if (this->keys[GLFW_KEY_LEFT_SHIFT])
		{
			player.setState(RUNNING);
		}
		else
		{
			player.setState(WALKING);
		}
		player.move(dt);
	}
	else
	{
		player.velocity = glm::vec2(0);
		player.setState(IDLE);
	}
}

void Game::updateCamera()
{
	glm::vec2 cameraPos =
		this->player.position - glm::vec2(this->WIDTH / 2.0f, this->HEIGHT / 2.0f);
	if (cameraPos != this->camera.lastPosition)
	{
		this->camera.setPosition(cameraPos);
		this->renderer.shader.setMat4("projection", this->camera.getProjectionMatrix());
		this->camera.lastPosition = cameraPos;
	}
}
