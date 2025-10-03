#include "Particle.h"
#include <glad/glad.h>

ParticleEmitter::ParticleEmitter(Shader shader, Texture2D texture, unsigned int amount) :
	shader(shader),
	texture(texture),
	amount(amount)
{
}

void ParticleEmitter::draw()
{
	this->shader.use();
	this->texture.bind();
	for (Particle& particle : this->particles)
	{
		this->shader.setVec2f("offset", particle.position);
		this->shader.setVec4f("color", particle.color);
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

void ParticleEmitter::update(float dt, glm::vec2 offset)
{
	//for (Particle& particle : this->particles)
	//{
	//	particle.position 
	//}
}

void ParticleEmitter::init()
{
	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	unsigned int VBO;
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribIPointer(0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	for (int i = 0; i < this->amount; i++)
	{
		this->particles.push_back(Particle());
	}
}

Particle::Particle() : position(0.0f), velocity(0.0f), color(1.0f) {}