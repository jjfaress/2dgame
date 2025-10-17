#pragma once
#include "GameObject.h"

class Player : public GameObject {
public:
	Player();
	Player(glm::vec2 pos, Texture2D sprite);

	void draw(SpriteRenderer& renderer) override;
};
