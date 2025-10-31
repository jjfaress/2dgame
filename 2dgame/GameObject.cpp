#include "GameObject.h"

GameObject::GameObject() :
	position(0.0f, 0.0f),
	scale(1.0f, 1.0f),
	velocity(0.0f),
	sprite(),
	collision(false)
{}

GameObject::GameObject(glm::vec2 position, Texture2D sprite,
	bool collision, glm::vec2 scale, glm::vec2 velocity) :
	position(position),
	scale(scale),
	velocity(velocity),
	sprite(sprite),
	collision(collision)
{}