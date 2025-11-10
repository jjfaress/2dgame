#pragma once
#include <glm/glm.hpp>

class Texture2D {
public:
	unsigned int ID;
	unsigned int WIDTH, HEIGHT;
	unsigned int imageFormat;
	unsigned int internalFormat;
	unsigned int sWrap, tWrap;
	unsigned int minFilter, maxFilter;

	Texture2D();
	void generate(unsigned int width, unsigned int height, unsigned char* data);
	void bind() const;
};