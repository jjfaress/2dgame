#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* renderer;

Game::Game(unsigned int width, unsigned int height) :
	State(GAME_ACTIVE),
	WIDTH(width),
	HEIGHT(height)
{
}

Game::~Game()
{
	delete renderer;
}

void Game::init()
{
	//ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
	//glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->WIDTH), static_cast<float>(this->HEIGHT), 0.0f);
	//ResourceManager::getShader("sprite").use().setInt("image", 0);
	//ResourceManager::getShader("sprite").setMat4("projection", projection);

	//Shader spriteShader = ResourceManager::getShader("sprite");
	//renderer = new SpriteRenderer(spriteShader);
	//ResourceManager::loadTexture("assets/sprite.png", true, "face");


}

void Game::render()
{
	//renderer->drawSprite(
	//	ResourceManager::getTexture("face"), 
	//	glm::vec2(100.0f, 200.0f), 
	//	glm::vec2(300.0f, 400.0f), 
	//	45.0f, 
	//	glm::vec3(0.0f, 1.0f, 0.0f));

}

void Game::processInput()
{

}