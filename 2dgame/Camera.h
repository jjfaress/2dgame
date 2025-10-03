#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	glm::vec2 position;
	float zoom;
	float screen_width, screen_height;

	Camera(float screenWidth, float screenHeight);
	glm::mat4 getProjectionMatrix() const;
	void move(glm::vec2 delta);
	void setPosition(glm::vec2 newPosition);

};