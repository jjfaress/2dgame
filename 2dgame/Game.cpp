#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* renderer;


Game::Game(uint screen_width, uint screen_height) : 
	WIDTH(screen_width), 
	HEIGHT(screen_height),
	level(config.mapHeight, config.mapHeight, static_cast<uint>(std::time(nullptr)))
{
}

Game::~Game()
{
	delete renderer;
}

void Game::init()
{
	level.init();
	ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->WIDTH), static_cast<float>(this->HEIGHT), 0.0f);
	ResourceManager::getShader("sprite").use().setInt("image", 0);
	ResourceManager::getShader("sprite").setMat4("projection", projection);
	Shader spriteShader = ResourceManager::getShader("sprite");
	renderer = new SpriteRenderer(spriteShader);
	level.generate();
}

void Game::render()
{
	level.draw(*renderer);
	
}

void Game::processInput()
{

}