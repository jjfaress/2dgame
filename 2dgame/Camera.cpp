#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float screenWidth, float screenHeight) :
	screen_width(screenWidth),
	screen_height(screenHeight),
	position(glm::vec2(0)),
	zoom(1.0)
{
}

glm::mat4 Camera::getProjectionMatrix() const
{
	float halfWidth = (screen_width / 2) / zoom;
	float halfHeight = (screen_height / 2) / zoom;

	float left = position.x;
	float right = position.x + screen_width;
	float top = position.y;
	float bottom = position.y + screen_height;

	return glm::ortho(left, right, bottom, top);
}

void Camera::move(glm::vec2 delta)
{
	position += delta;
}

void Camera::setPosition(glm::vec2 newPosition)
{
	position = newPosition;
}