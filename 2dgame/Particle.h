#pragma once
#include "Shader.h"
#include "Texture.h"
#include <vector>

struct Particle {
	glm::vec2 position, velocity;
	glm::vec4 color;
	Particle();
};

class ParticleEmitter {
public:
	std::vector<Particle> particles;

	ParticleEmitter(Shader shader, Texture2D texture, unsigned int amount);
	void draw();
	void update(float dt, glm::vec2 offset);

private:
	unsigned int VAO, amount;
	Shader shader;
	Texture2D texture;

	void init();

};