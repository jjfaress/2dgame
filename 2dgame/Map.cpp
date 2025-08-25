#include "Map.h"
#include <queue>
#include <unordered_set>
#include <random>
#include <iostream>
#include "ResourceManager.h"

Map::Map(int width, int height, uint seed) :
	WIDTH(width),
	HEIGHT(height),
	isReady(false),
	seed(seed),
	config(ConfigLoader::getInstance())
{
}

void Map::init()
{
	this->grid.reserve(this->WIDTH);
	for (int i = 0; i < this->WIDTH; i++)
	{
		std::vector<Tile> row;
		row.reserve(this->HEIGHT);
		for (int j = 0; j < this->HEIGHT; j++)
		{
			row.emplace_back(Tile(glm::vec2(i, j), config.tileTypes));
		}
		this->grid.emplace_back(std::move(row));
	}

	for (const auto& type : config.tileTypes)
	{
		ResourceManager::loadTexture(("assets/" + config.textures[type]).c_str(), false, config.textures[type]);
	}
}

void Map::collapse(int x, int y, std::mt19937 rng, int& collapseCount)
{
	if (x < 0 || x >= this->WIDTH || y < 0 || y >= this->HEIGHT ||
		this->grid[x][y].collapsed || this->grid[x][y].possibilities.empty())
	{
		std::cerr << "Warning: Invalid collapse attempt at (" << x << ", " << y << ")\n";
		throw std::runtime_error("error collapsing");
		return;
	}

	Tile& tile = this->grid[x][y];
	std::uniform_int_distribution<std::mt19937::result_type> poss(0, tile.possibilities.size() - 1);
	tile.type = tile.possibilities[poss(rng)];
	tile.texture = config.textures[tile.type].c_str();
	tile.collapsed = true;
	collapseCount++;
	tile.possibilities.clear();
	tile.possibilities.shrink_to_fit();
}

void Map::collapse(int x, int y, uint& seed, int& collapseCount)
{
	std::mt19937 rng(seed);
	collapse(x, y, rng, collapseCount);
}

void Map::propagate(int x, int y, int& collapseCount)
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

			if (this->grid[neighbor.x][neighbor.y].collapsed)
			{
				continue;
			}

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
				std::vector<int>& sourcePos = sourceTile.possibilities;
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

			if (neighborTile.possibilities.size() != originalSize)
			{
				if (neighborTile.possibilities.size() == 1 && !neighborTile.collapsed)
				{
					collapse(neighborTile.position.x, neighborTile.position.y, this->seed, collapseCount);
					continue;
				}
				queue.push(glm::vec2(neighbor.x, neighbor.y));
			}
		}
	}
}

void Map::generate()
{
	std::mt19937 rng(this->seed);
	std::uniform_int_distribution<std::mt19937::result_type> disX(0, this->WIDTH - 1);
	std::uniform_int_distribution<std::mt19937::result_type> disY(0, this->HEIGHT - 1);
	int cellsCollapsed = 0;

	while (true)
	{
		int x = disX(rng);
		int y = disY(rng);
		if (cellsCollapsed == (this->WIDTH * this->HEIGHT))
		{
			break;
		}
		if (!this->grid[x][y].collapsed)
		{
			collapse(x, y, rng, cellsCollapsed);
			propagate(x, y, cellsCollapsed);
		}
	}
	this->isReady = true;
	postProcess();
}

void Map::postProcess()
{
	if (this->isReady)
	{
		for (const auto& row : this->grid)
		{
			this->finalTiles.insert(this->finalTiles.end(), row.begin(), row.end());
		}
	}
}

void Map::draw(SpriteRenderer& renderer)
{
	for (const Tile& tile : this->finalTiles)
	{
		Texture2D texture = ResourceManager::getTexture(tile.texture);
		renderer.drawSprite(texture, tile.position, glm::vec2(this->HEIGHT, this->WIDTH));
	}
}



