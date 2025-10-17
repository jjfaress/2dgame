#pragma once
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Origin {
	TL,
	T,
	TR,
	R,
	BR,
	B,
	BL,
	L,
	C
};

class SpriteRenderer {
public:
	SpriteRenderer(Shader& shader);
	~SpriteRenderer();
	void drawSprite(
		Texture2D texture, 
		glm::vec2 position,
		Origin origin = TL,
		glm::vec2 size = glm::vec2(1.0f),
		float rotate = 0.0f, 
		glm::vec3 color = glm::vec3(1.0f));
	void setShader(Shader& shader);

private:
	Shader shader;
	unsigned int quadVAO;
	void initRenderData();
};