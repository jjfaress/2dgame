#pragma once
#include "GameObject.h"
#include "ResourceManager.h"
#include "Collision.h"

enum CharacterState {
	IDLE,
	WALKING,
	RUNNING,
	FALLING,
	JUMPING,
	DEATH,
	ATTACK,
	DODGE
};

using namespace Collision;

class Character : public GameObject
{
private:
	int localFrameIdx = 0;
	int frameCount = 0;
	float frameTimer = 0;
	float frameDuration = 0;
	bool facingRight = false;
	std::unordered_map<
		CharacterState, std::vector<unsigned int>> animations;
	std::vector<FrameData> frames;
	std::unordered_map<
		CharacterState, std::vector<CollisionObject>> collidersMap;

public:
	glm::vec2 prevPos{};
	CharacterState state{ IDLE };
	//std::vector<CollisionObject> colliders;
	CollisionObject collider;

	void init(std::string spriteSheet, std::string& spriteDir, glm::vec2 pos);
	void init(glm::vec2 pos, Texture2D sprite);
	void update(float dt);
	void tick();
	void draw(SpriteRenderer& renderer) override;
	void draw(SpriteRenderer& renderer, float alpha);
	void move();
	std::vector<unsigned int> getFrames(int from, int to);
	void setState(CharacterState state);
	void setSpeed(glm::vec2 vel);
	std::vector<CollisionObject> getCurrentColliders();
};
