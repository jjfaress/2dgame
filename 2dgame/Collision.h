#pragma once
#include <glm/glm.hpp>
#include <variant>

namespace Collision {

	using Grid = std::unordered_map<size_t, std::vector<size_t>>;

	enum Shape {
		RECTANGLE, ELLIPSE, POLYGON, CIRCLE, POINT
	};

	template<Shape S>
	struct CollisionBox {
		glm::vec2 position;
	};

	template<>
	struct CollisionBox<Shape::RECTANGLE> {
		glm::vec2 position;
		glm::vec2 size;
		float rotation;
	};

	template<>
	struct CollisionBox<Shape::CIRCLE> {
		glm::vec2 position;
		float radius;
	};

	template<>
	struct CollisionBox<Shape::POLYGON> {
		glm::vec2 position;
		float rotation;
		std::vector<glm::vec2> points;
	};

	template<>
	struct CollisionBox<Shape::ELLIPSE> {
		glm::vec2 position;
		float rotation;
		glm::vec2 size;
	};

	using CollisionObject = std::variant<
		CollisionBox<RECTANGLE>,
		CollisionBox<CIRCLE>,
		CollisionBox<POLYGON>,
		CollisionBox<POINT>,
		CollisionBox<ELLIPSE>
	>;

	Grid buildSpacialGrid(
		const std::vector<CollisionObject>& objects, 
		float cellSize);

	size_t hash(int x, int y);


}