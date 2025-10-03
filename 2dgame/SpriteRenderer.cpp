#include "SpriteRenderer.h"
#include <glad/glad.h>

SpriteRenderer::SpriteRenderer(Shader& shader)
{
	this->shader = shader;
	initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::drawSprite(Texture2D texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
	this->shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, { position, 0.0f });
	model = glm::scale(model, { (size * glm::vec2(texture.WIDTH, texture.HEIGHT)), 1.0f });
	model = glm::rotate(model, rotate, { 0.0f, 0.0f, 1.0f });
	this->shader.setVec3("spriteColor", color);
	this->shader.setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
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

	unsigned int vertexBuffer;
	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &vertexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}