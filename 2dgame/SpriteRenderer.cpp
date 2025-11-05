#include "SpriteRenderer.h"
#include <glad/glad.h>

void SpriteRenderer::init()
{
	initRenderData();
}

void SpriteRenderer::destroy()
{
	glDeleteVertexArrays(1, &this->quadVAO);
	glDeleteBuffers(1, &this->VBO);
}

void SpriteRenderer::drawSprite(
	Texture2D texture,
	glm::vec2 position,
	Origin origin,
	glm::vec2 scale,
	float rotate,
	glm::vec3 color)
{
	FrameData fd;
	fd.uvTopLeft = glm::vec2(0.0f, 0.0f);
	fd.uvBotRight = glm::vec2(1.0f, 1.0f);
	fd.size = glm::vec2(texture.WIDTH, texture.HEIGHT);

	drawSprite(texture, position, fd, origin, scale, rotate, color);
}

void SpriteRenderer::drawSprite(
	Texture2D texture,
	glm::vec2 position,
	FrameData fd,
	Origin origin, 
	glm::vec2 scale, 
	float rotate,
	glm::vec3 color)
{
	glm::vec2 spriteSize = scale * fd.size;
	switch (origin)
	{
	case TL:
		break;
	case T:
		position -= glm::vec2(std::floorf(spriteSize.x / 2), 0);
		break;
	case TR:
		position -= glm::vec2(spriteSize.x, 0);
		break;
	case R:
		position -= glm::vec2(spriteSize.y, std::floorf(spriteSize.x / 2));
		break;
	case BR:
		position -= glm::vec2(spriteSize.x, spriteSize.y);
		break;
	case B:
		position -= glm::vec2(std::floorf(spriteSize.x / 2), spriteSize.y);
		break;
	case BL:
		position -= glm::vec2(0, spriteSize.y);
		break;
	case L:
		position -= glm::vec2(0, std::floorf(spriteSize.y / 2));
		break;
	case C:
		position -= glm::vec2(std::floorf(spriteSize.x / 2), std::floorf(spriteSize.y / 2));
		break;
	}

	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, fd.uvTopLeft.x,  fd.uvBotRight.y,  // Bottom-left
		1.0f, 0.0f, fd.uvBotRight.x, fd.uvTopLeft.y,   // Top-right
		0.0f, 0.0f, fd.uvTopLeft.x,  fd.uvTopLeft.y,   // Top-left

		0.0f, 1.0f, fd.uvTopLeft.x,  fd.uvBotRight.y,  // Bottom-left
		1.0f, 1.0f, fd.uvBotRight.x, fd.uvBotRight.y,  // Bottom-right
		1.0f, 0.0f, fd.uvBotRight.x, fd.uvTopLeft.y    // Top-right
	};

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	this->shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, { position, 0.0f });
	model = glm::translate(model, glm::vec3(spriteSize.x * 0.5f, spriteSize.y * 0.5f, 0.0f));
	model = glm::rotate(model, rotate, { 0.0f, 0.0f, 1.0f });
	model = glm::translate(model, glm::vec3(-spriteSize.x * 0.5f, -spriteSize.y * 0.5f, 0.0f));
	model = glm::scale(model, { spriteSize, 1.0f });
	this->shader.setVec3("spriteColor", color);
	this->shader.setMat4("model", model);

	//glActiveTexture(GL_TEXTURE0);
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

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &this->VBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::initFrameBufferData()
{
	unsigned int texture;

}

void SpriteRenderer::setShader(Shader& shader)
{
	this->shader = shader;
}
