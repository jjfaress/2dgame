#include "Player.h"

Player::Player() : GameObject()
{
}

Player::Player(glm::vec2 pos, Texture2D sprite) :
	GameObject(pos, sprite)
{
}

void Player::draw(SpriteRenderer& renderer)
{
	renderer.drawSprite(this->sprite, this->position);
}
