#pragma once
#include <glm/glm.hpp>
#include "Texture.h"
#include "SpriteRenderer.h"

class GameObject {
public:
	glm::vec2 position{}, scale{1}, velocity{};
	Texture2D sprite;
	bool collision;
	virtual void draw(SpriteRenderer& renderer) = 0;
};
