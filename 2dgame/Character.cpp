#include "Character.h"

void Character::init(std::string spriteSheet, std::string& spriteDir)
{
	auto ad = ResourceManager::parseSpriteSheet(spriteSheet, spriteDir);
	this->sprite = ad.texture;
	this->frames = ad.frames;
	for (auto& pair : ad.tags)
	{
		if (pair.first == "idle")
		{
			this->animations[IDLE] = getFrames(pair.second.first, pair.second.second);
			setState(IDLE);
			this->frameCount = this->animations[IDLE].size();
			this->frameDuration = this->frames[this->animations[IDLE][this->localFrameIdx]].duration;
		}
		else if (pair.first == "walking")
		{
			this->animations[WALKING] = getFrames(pair.second.first, pair.second.second);
		}
		else if (pair.first == "running")
		{
			this->animations[RUNNING] = getFrames(pair.second.first, pair.second.second);
		}
	}

}

void Character::init(glm::vec2 pos, Texture2D sprite)
{
	this->position = pos;
	this->sprite = sprite;
}

void Character::update(float dt)
{
	frameTimer += dt;
	if (frameTimer >= frameDuration)
	{
		frameTimer = 0;
		localFrameIdx = (localFrameIdx + 1) % frameCount;
		int globalFrameIdx = this->animations[state][localFrameIdx];
		frameDuration = this->frames[globalFrameIdx].duration;
	}
}

void Character::draw(SpriteRenderer& renderer)
{
	std::vector<unsigned int>& localFrames = this->animations[this->state];
	unsigned int currentFrameIdx = localFrames[this->localFrameIdx];
	auto& currentFrame = this->frames[currentFrameIdx];

	renderer.drawSprite(this->sprite, this->position, currentFrame, B);
}

void Character::move(float dt)
{
	if (this->state == RUNNING)
	{
		this->velocity *= 1.5;
	}
	this->position += this->velocity * (dt * 500);
}

std::vector<unsigned int> Character::getFrames(int from, int to)
{
	std::vector<unsigned int> frames;
	for (int i{ from }; i <= to; i++)
	{
		frames.push_back(i);
	}
	return frames;
}

void Character::setState(CharacterState state)
{
	if (state == this->state) return;
	this->state = state;
	this->frameCount = this->animations[state].size();
	this->localFrameIdx = 0;
	this->frameDuration = this->frames[this->animations[state][0]].duration;
}
