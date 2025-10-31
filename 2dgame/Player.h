#pragma once
#include "GameObject.h"

enum PlayerState {
	IDLE, WALKING, FALLING, DEATH, ATTACK, DODGE
};

class Character : public GameObject {
public:
	float speed;
	PlayerState state;

	Character();
	Character(glm::vec2 pos, Texture2D sprite);

	void init(glm::vec2 pos, Texture2D sprite);
	void draw(SpriteRenderer& renderer) override;
	void move(glm::vec2 delta);

};
