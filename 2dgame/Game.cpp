#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"

//SpriteRenderer* renderer;

Game::Game(uint screen_width, uint screen_height) :
	WIDTH(screen_width),
	HEIGHT(screen_height)
{
	ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->WIDTH), static_cast<float>(this->HEIGHT), 0.0f);
	ResourceManager::getShader("sprite").use().setInt("image", 0);
	ResourceManager::getShader("sprite").setMat4("projection", projection);
	Shader spriteShader = ResourceManager::getShader("sprite");

	level = new WFCMap(
		config.mapWidth,
		config.mapHeight,
		config.seed ? config.seed : static_cast<unsigned int>(std::time(nullptr)), 
		*new SpriteRenderer(spriteShader));
}

Game::~Game()
{
	//delete renderer;
}

void Game::init()
{
	level->init();
}

void Game::render()
{
	level->stepGenerate();
}

void Game::processInput()
{

}