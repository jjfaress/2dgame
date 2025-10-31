#pragma once
#include <glm/glm.hpp>
#include "Texture.h"
#include "SpriteRenderer.h"

class GameObject {
public:
	glm::vec2 position, scale, velocity;
	Texture2D sprite;
	bool collision;
	GameObject();
	GameObject(glm::vec2 pos,
		Texture2D sprite,
		bool collision,
		glm::vec2 scale = glm::vec2(1.0),
		glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
	virtual void draw(SpriteRenderer& renderer) = 0;
};
