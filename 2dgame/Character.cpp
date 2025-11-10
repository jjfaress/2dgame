#include "Character.h"

void Character::init(std::string spriteSheet, std::string& spriteDir, glm::vec2 pos)
{
	auto ad = ResourceManager::parseSpriteSheet(spriteSheet, spriteDir);
	this->sprite = ad.texture;
	this->frames = ad.frames;
	this->prevPos = this->position = pos;
	for (auto& pair : ad.tags)
	{
		if (pair.first == "idle")
		{
			this->animations[IDLE] = getFrames(pair.second.first, pair.second.second);
			setState(IDLE);
			this->frameCount = this->animations[IDLE].size();
			this->frameDuration =
				this->frames[this->animations[IDLE][this->localFrameIdx]].duration;
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
	if (this->velocity.x > 0) this->facingRight = true;
	if (this->velocity.x < 0) this->facingRight = false;

	this->frameTimer += dt;
	if (this->frameTimer >= this->frameDuration)
	{
		this->frameTimer -= this->frameDuration;
		this->localFrameIdx = (this->localFrameIdx + 1) % this->frameCount;
		int globalFrameIdx = this->animations[this->state][this->localFrameIdx];
		this->frameDuration = this->frames[globalFrameIdx].duration;
	}
}

void Character::draw(SpriteRenderer& renderer)
{
	std::vector<unsigned int>& localFrames = this->animations[this->state];
	unsigned int currentFrameIdx = localFrames[this->localFrameIdx];
	auto& currentFrame = this->frames[currentFrameIdx];
	renderer.drawSprite(
		this->sprite,
		this->position,
		currentFrame,
		B,
		(this->facingRight ? glm::vec2(1.0f, 0.0f) : glm::vec2(0.0f))
	);
}

void Character::draw(SpriteRenderer& renderer, float alpha)
{
	glm::vec2 renderPos = glm::mix(this->prevPos, this->position, alpha);
	std::vector<unsigned int>& localFrames = this->animations[this->state];
	unsigned int currentFrameIdx = localFrames[this->localFrameIdx];
	auto& currentFrame = this->frames[currentFrameIdx];
	renderer.drawSprite(
		this->sprite,
		renderPos,
		currentFrame,
		TL,
		(this->facingRight ? glm::vec2(1.0f, 0.0f) : glm::vec2(0.0f))
	);
}

void Character::move()
{
	this->prevPos = this->position;
	if (glm::length(this->velocity) > 0)
	{
		float speedMultiplier = (this->state == RUNNING) ? 1.8f : 1.0f;
		glm::vec2 movement = 
			this->velocity * speedMultiplier * (1.0f / 60.0f) * 300.0f;
		this->position += movement;

		std::visit([&](auto& col) {
			col.position = this->position;
			}, this->collider);
	}
}

std::vector<unsigned int> Character::getFrames(int from, int to)
{
	std::vector<unsigned int> frames;
	for (int i = from; i <= to; i++)
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

void Character::setSpeed(glm::vec2 vel)
{
	this->velocity = vel;
}