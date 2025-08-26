#include "Game.h"
#include "ResourceManager.h"
#include "ConfigLoader.h"
#include "SpriteRenderer.h"

ConfigLoader& config = ConfigLoader::getInstance("config.yaml");

SpriteRenderer* renderer;

Map level(100, 100, static_cast<uint>(std::time(nullptr)));

Game::Game(uint width, uint height) : WIDTH(width), HEIGHT(height)
{
}

Game::~Game()
{
	delete renderer;
}

void Game::init()
{
	level.init();
	level.generate();
	ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->WIDTH), static_cast<float>(this->HEIGHT), 0.0f);
	ResourceManager::getShader("sprite").use().setInt("image", 0);
	ResourceManager::getShader("sprite").setMat4("projection", projection);
	Shader spriteShader = ResourceManager::getShader("sprite");
	renderer = new SpriteRenderer(spriteShader);
}

void Game::render()
{
	level.draw(*renderer);
}

void Game::processInput()
{

}