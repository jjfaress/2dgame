#include "Level.h"
#include <yaml-cpp/yaml.h>
#include <queue>
#include <unordered_map>

Level::Level(int width, int height, unsigned int seed) :
	WIDTH(width),
	HEIGHT(height),
	isReady(false),
	seed(seed)
{
	this->tiles.reserve(this->HEIGHT);
	for (int j = 0; j < HEIGHT; j++)
	{
		std::vector<Tile> row;
		row.reserve(this->WIDTH);
		for (int i = 0; i < WIDTH; i++)
		{
			row.emplace_back(Tile(glm::vec2(i,j)));
		}
		this->tiles.emplace_back(std::move(row));
	}
	config.loadFromFile("map.yaml");
}

void Level::collapse(int x, int y)
{
	if (x < 0 || x > this->WIDTH || y > this->HEIGHT ||
		this->tiles[x][y].collapsed || this->tiles[x][y].possibilities.empty())
	{
		return;
	}

	Tile& tile = this->tiles[x][y];
	std::srand(this->seed);
	int rand = std::rand() % tile.possibilities.size();
	tile.type = tile.possibilities[rand];
	tile.collapsed = true;
	tile.possibilities.clear();
	tile.possibilities.shrink_to_fit();

	propagate(x, y);
}
void Level::propagate(int x, int y)
{
	std::queue<glm::ivec2> queue;
	queue.push(glm::ivec2(x, y));

	glm::ivec2 directions[] = {
		glm::ivec2(0,-1),
		glm::ivec2(1, 0),
		glm::ivec2(0, 1),
		glm::ivec2(1, 0)
	};

	while (!queue.empty())
	{
		glm::ivec2 source = queue.front();
		queue.pop();

		for (int dir = 0; dir < 4; dir++)
		{
			glm::vec2 neighbor = source + directions[dir];
			if (neighbor.x < 0 || neighbor.x > this->WIDTH ||
				neighbor.y < 0 || neighbor.y > this->HEIGHT)
			{
				continue;
			}

			if (this->tiles[neighbor.x][neighbor.y].collapsed) continue;

			if (this->tiles[source.x][source.y].collapsed)
			{
				Tile& sourceTile = this->tiles[source.x][source.y];
			}
		}
	}
}
