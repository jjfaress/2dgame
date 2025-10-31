#include "Player.h"

Character::Character() : GameObject()
{
}

Character::Character(glm::vec2 pos, Texture2D sprite) :
	GameObject(pos, sprite, true)
{
}

void Character::draw(SpriteRenderer& renderer)
{
	renderer.drawSprite(this->sprite, this->position, B);
}

void Character::move(glm::vec2 delta)
{
	this->position += glm::normalize(delta)*this->speed;
}