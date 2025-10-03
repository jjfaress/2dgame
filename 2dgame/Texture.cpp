#include "Texture.h"
#include <glad/glad.h>

Texture2D::Texture2D() : 
	WIDTH(0), 
	HEIGHT(0), 
	internalFormat(GL_RGBA), 
	imageFormat(GL_RGBA), 
	sWrap(GL_REPEAT), 
	tWrap(GL_REPEAT), 
	minFilter(GL_NEAREST), 
	maxFilter(GL_NEAREST)
{
	glGenTextures(1, &this->ID);
}

void Texture2D::generate(unsigned int width, unsigned int height, unsigned char* data)
{
	this->WIDTH = width;
	this->HEIGHT = height;
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->sWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->tWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->maxFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}