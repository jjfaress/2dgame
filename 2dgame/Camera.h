#pragma once
#include <glm/glm.hpp>
#include "GameObject.h"

class Camera {
public:
	glm::vec2 position{};
	glm::vec2 lastPosition{};
	float zoom = 1.0;
	float screen_width, screen_height;
	
	Camera() = default;
	void init(float screenWidth, float screenHeight);
	glm::mat4 getProjectionMatrix() const;
	void move(glm::vec2 delta);
	void setPosition(glm::vec2 newPosition);
};