#include "Game.h"
#include "ResourceManager.h"

Game::Game(uint screen_width, uint screen_height) :
	WIDTH(screen_width),
	HEIGHT(screen_height)
{
	this->camera = new Camera(static_cast<float>(this->WIDTH), static_cast<float>(this->HEIGHT));
	ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
	ResourceManager::getShader("sprite").use().setInt("image", 0);
	Shader spriteShader = ResourceManager::getShader("sprite");
	this->renderer = new SpriteRenderer(spriteShader);
	std::string spriteDir("assets/sprites/");
	std::string mapPath = spriteDir + std::string("map.json");
	this->level = new TiledMap(mapPath, spriteDir, *this->renderer);
}

Game::~Game()
{
	delete this->level;
	delete this->renderer;
	delete this->camera;
}

void Game::init()
{


}

void Game::render()
{
	switch (this->State)
	{
	case HUB:
		Shader spriteShader = ResourceManager::getShader("sprite");
		spriteShader.setMat4("projection", this->camera->getProjectionMatrix());
		this->level->draw();

		break;
	}
}

void Game::processInput(float dt)
{
	float cameraSpeed = 2.0f;
	if (this->keys[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	}
	if (this->keys[GLFW_KEY_LEFT_SHIFT])
	{
		cameraSpeed = 4.0f;
	}
	if (this->keys[GLFW_KEY_W])
	{
		this->camera->move({ 0.0, -cameraSpeed });
	}
	if (this->keys[GLFW_KEY_S])
	{
		this->camera->move({ 0.0, cameraSpeed });
	}
	if (this->keys[GLFW_KEY_A])
	{
		this->camera->move({ -cameraSpeed, 0.0 });
	}
	if (this->keys[GLFW_KEY_D])
	{
		this->camera->move({ cameraSpeed, 0.0 });
	}
}