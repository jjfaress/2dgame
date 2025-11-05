#pragma once
#include "GameObject.h"
#include "ResourceManager.h"

enum CharacterState {
	IDLE, 
	WALKING, 
	RUNNING, 
	FALLING, 
	DEATH, 
	ATTACK, 
	DODGE
};

class Character : public GameObject {
private:
	int localFrameIdx = 0;
	int frameCount = 0;
	float frameTimer = 0;
	float frameDuration = 0;

public:
	CharacterState state{ IDLE };
	std::unordered_map<
		CharacterState, std::vector<unsigned int>> animations;
	std::vector<FrameData> frames;

	void init(std::string spriteSheet, std::string& spriteDir);
	void init(glm::vec2 pos, Texture2D sprite);
	void update(float dt);
	void draw(SpriteRenderer& renderer) override;
	void move(float dt);
	std::vector<unsigned int> getFrames(int from, int to);
	void setState(CharacterState state);
};
