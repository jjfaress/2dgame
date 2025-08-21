#include "Map.h"
#include <queue>
#include <unordered_set>

Map::Map(int width, int height, unsigned int seed) :
	WIDTH(width),
	HEIGHT(height),
	isReady(false),
	seed(seed),
	config(ConfigLoader::getInstance("map.yaml"))
{
	this->grid.reserve(this->HEIGHT);
	for (int j = 0; j < HEIGHT; j++)
	{
		std::vector<Tile> row;
		row.reserve(this->WIDTH);
		for (int i = 0; i < WIDTH; i++)
		{
			row.emplace_back(Tile(glm::vec2(i, j), config.tiletypes));
		}
		this->grid.emplace_back(std::move(row));
	}
	std::srand(this->seed);
}

void Map::collapse(int x, int y)
{
	if (x < 0 || x >= this->WIDTH || y < 0 || y >= this->HEIGHT ||
		this->grid[x][y].collapsed || this->grid[x][y].possibilities.empty())
	{
		throw std::runtime_error("error collapsing");
		return;
	}

	Tile& tile = this->grid[x][y];
	int rand = std::rand() % tile.possibilities.size();
	tile.type = tile.possibilities[rand];
	tile.collapsed = true;
	tile.possibilities.clear();
	tile.possibilities.shrink_to_fit();
	propagate(x, y);
}
void Map::propagate(int x, int y)
{
	std::queue<glm::ivec2> queue;
	queue.push(glm::ivec2(x, y));

	glm::ivec2 directions[] = {
		glm::ivec2(0, 1), //north
		glm::ivec2(1, 0), // east
		glm::ivec2(0, -1), // south
		glm::ivec2(-1, 0) // west
	};

	while (!queue.empty())
	{
		glm::ivec2 source = queue.front();
		queue.pop();

		for (int dir = 0; dir < 4; dir++)
		{
			glm::vec2 neighbor = source + directions[dir];
			if (neighbor.x < 0 || neighbor.x >= this->WIDTH ||
				neighbor.y < 0 || neighbor.y >= this->HEIGHT)
			{
				continue;
			}

			Tile& sourceTile = this->grid[source.x][source.y];
			Tile& neighborTile = this->grid[neighbor.x][neighbor.y];

			if (this->grid[neighbor.x][neighbor.y].collapsed) continue;

			std::vector<int>& neighborPos = neighborTile.possibilities;
			size_t originalSize = neighborPos.size();

			if (sourceTile.collapsed)
			{
				std::unordered_set<int> validNeighbors = config.validNeighbors[sourceTile.type];
				neighborPos.erase(
					std::remove_if(neighborPos.begin(), neighborPos.end(),
						[&validNeighbors](int type) {
							return validNeighbors.find(type) == validNeighbors.end();
						}),
					neighborPos.end()
				);
			}
			else
			{
				std::unordered_set<int> validNeighbors;
				std::vector<int> sourcePos = sourceTile.possibilities;
				for (int type : sourcePos)
				{
					for (int neighbor : config.validNeighbors[type])
					{
						validNeighbors.insert(neighbor);
					}
				}
				neighborPos.erase(
					std::remove_if(neighborPos.begin(), neighborPos.end(),
						[&validNeighbors](int type) {
							return validNeighbors.find(type) == validNeighbors.end();
						}),
					neighborPos.end()
				);
			}

			neighborPos.shrink_to_fit();

			if (neighborTile.possibilities.size() != originalSize)
			{
				if (neighborTile.possibilities.size() == 1)
				{
					neighborTile.collapsed = true;
					neighborTile.type = neighborTile.possibilities[0];
					neighborTile.possibilities.clear();
					neighborTile.possibilities.shrink_to_fit();
				}
				queue.push(glm::vec2(neighbor.x, neighbor.y));
			}
		}
	}
}
