#include "Collision.h"

namespace Collision {

	Grid buildSpacialGrid(
		const std::vector<CollisionObject>& objects,
		float cellSize)
	{
		Grid grid;

		for (size_t i = 0; i < objects.size(); i++)
		{
			std::visit([&](auto&& obj) {
				glm::vec2 min, max;
				if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, CollisionBox<RECTANGLE>>)
				{
					min = obj.position;
					max = obj.position + obj.size;
				}
				else if constexpr (std::is_same_v <std::decay_t<decltype(obj)>, CollisionBox<CIRCLE>>)
				{
					min = obj.position - obj.radius;
					max = obj.position + obj.radius;
				}
				else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, CollisionBox<ELLIPSE>>)
				{
					min = obj.position;
					max = obj.position + obj.size;
				}
				else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, CollisionBox<POLYGON>>)
				{
					min = max = obj.position + obj.points[0];
					for (auto& point : obj.points)
					{
						glm::vec2 newPos = obj.position + point;
						min = glm::min(min, newPos);
						max = glm::max(max, newPos);
					}
				}
				else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, CollisionBox<POINT>>)
				{
					min = max = obj.position;
				}

				int minCellX = static_cast<int>(std::floor(min.x / cellSize));
				int maxCellX = static_cast<int>(std::floor(max.x / cellSize));
				int minCellY = static_cast<int>(std::floor(min.y / cellSize));
				int maxCellY = static_cast<int>(std::floor(max.y / cellSize));

				for (int x{ minCellX }; x <= maxCellX; x++)
				{
					for (int y{ minCellY }; y <= maxCellY; y++)
					{
						grid[hashPosition(x, y)].push_back(i);
					}
				}

				}, objects[i]);
		}
		return grid;
	}

	size_t hashPosition(int x, int y)
	{
		const size_t p1 = 73856093;
		const size_t p2 = 19349663;
		return static_cast<size_t>((x * p1) ^ (y * p2));
	}

	std::vector<std::pair<int, int>> getAdjacentPairs(Grid& grid)
	{
		std::vector<std::pair<int, int>> pairs;

		for (auto& [cell, indices] : grid)
		{
			for (size_t i = 0; i < indices.size(); i++)
			{
				for (size_t j{ i + 1 }; j < indices.size(); j++)
				{
					pairs.push_back({ indices[i], indices[j] });
				}
			}
		}
		return pairs;
	}

	bool checkAABB(CollisionBox<RECTANGLE> a, CollisionBox<RECTANGLE> b)
	{
		return !(
			a.position.x + a.size.x < b.position.x ||
			b.position.x + b.size.x < a.position.x ||
			a.position.y + a.size.y < b.position.y ||
			b.position.y + b.size.y < a.position.y
			);
	}


}