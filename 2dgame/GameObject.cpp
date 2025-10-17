#include "GameObject.h"

GameObject::GameObject() :
	position(0.0f, 0.0f),
	scale(1.0f, 1.0f),
	velocity(0.0f),
	sprite()
{}

GameObject::GameObject(glm::vec2 position, Texture2D sprite, glm::vec2 scale,glm::vec2 velocity) :
	position(position),
	scale(scale),
	velocity(velocity),
	sprite(sprite)
{}