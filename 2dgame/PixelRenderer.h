#pragma once
#include "Shader.h"

struct Pixel {
	uint8_t size;
	glm::vec2 position;
	glm::vec3 color;
};

struct Sprite {
	glm::vec2 worldOrigin;
	glm::vec2 resolution;
};

class PixelRenderer {
public:
	PixelRenderer(Shader& shader);
	~PixelRenderer();
	void drawSprite();

private:
	Shader shader;
};
