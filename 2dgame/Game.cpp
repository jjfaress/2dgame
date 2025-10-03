#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"

Game::Game(uint screen_width, uint screen_height) :
	WIDTH(screen_width),
	HEIGHT(screen_height)
{
	this->camera = new Camera(static_cast<float>(this->WIDTH), static_cast<float>(this->HEIGHT));
	ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
	ResourceManager::loadShader("particle.vert", "particle.frag", "particle");
	ResourceManager::getShader("sprite").use().setInt("image", 0);
	ResourceManager::getShader("sprite").setMat4("projection", this->camera->getProjectionMatrix());
	Shader spriteShader = ResourceManager::getShader("sprite");
	ResourceManager::loadTexture("assets/sprites/grass.png", true, "grass");
	ResourceManager::loadTexture("assets/sprites/dirt32.png", true, "dirt");
	this->renderer = new SpriteRenderer(spriteShader);

	this->level = new WFCMap(
		config.mapWidth,
		config.mapHeight, 
		static_cast<unsigned int>(std::time(nullptr)),
		*this->renderer);

	level->init();
}

Game::~Game()
{
	delete this->renderer;
	delete this->camera;
	delete this->level;
}

void Game::init()
{
}

void Game::render()
{
	switch (this->State)
	{
	case HUB:
		//ResourceManager::getShader("sprite").use();
		ResourceManager::getShader("sprite").setMat4("projection",
			this->camera->getProjectionMatrix());

		level->stepGenerate();

		//Texture2D dirt = ResourceManager::getTexture("dirt");
		//Texture2D grass = ResourceManager::getTexture("grass");

		//this->renderer->drawSprite(dirt, glm::vec2(0));
		//this->renderer->drawSprite(grass, glm::vec2(16, 16));
		
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