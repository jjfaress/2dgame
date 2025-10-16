#pragma once
#include <glm/glm.hpp>
#include "Texture.h"

class Player {
public:
	glm::vec2 position = glm::vec2(0);
	Texture2D sprite;
	Player();
	Player(glm::vec2 pos);
};
