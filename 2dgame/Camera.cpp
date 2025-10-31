#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

void Camera::init(float screenWidth, float screenHeight)
{
	this->screen_width = screenWidth;
	this->screen_height = screenHeight;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	float halfWidth = (this->screen_width / 2) / zoom;
	float halfHeight = (this->screen_height / 2) / zoom;

	float left = this->position.x;
	float right = this->position.x + this->screen_width;
	float top = this->position.y;
	float bottom = this->position.y + this->screen_height;

	return glm::ortho(left, right, bottom, top);
}

void Camera::move(glm::vec2 delta)
{
	this->lastPosition = this->position;
	this->position += delta;
}

void Camera::setPosition(glm::vec2 newPosition)
{
	this->lastPosition = this->position;
	this->position = newPosition;
}
