#pragma once
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct InstanceData {
	glm::vec2 position;
	glm::vec2 atlasOffset;
	glm::vec2 atlasSize;
};

enum Origin {
	TL, T, TR, R, BR, B, BL, L, C
};

class SpriteRenderer {
public:
	Shader shader;

	SpriteRenderer() = default;
	~SpriteRenderer() = default;

	SpriteRenderer(const SpriteRenderer& other) = delete;
	SpriteRenderer(const SpriteRenderer&& other) = delete;
	SpriteRenderer& operator=(const SpriteRenderer& other) = delete;

	void init();
	void destroy();
	void drawSprite(
		Texture2D texture,
		glm::vec2 position,
		Origin origin = TL,
		glm::vec2 size = glm::vec2(1.0f),
		float rotate = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));
	void setShader(Shader& shader);

private:
	unsigned int quadVAO = 0;
	void initRenderData();
	void initFrameBufferData();
};