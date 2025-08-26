#include "Map.h"
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
			this->eq.push({ config.tileTypes.size(), glm::vec2(i,j) });
		}
		this->grid.emplace_back(std::move(row));
	}

	for (const auto& type : config.tileTypes)
	{
		ResourceManager::loadTexture(("assets/" + config.textures[type]).c_str(), false, config.textures[type]);
	}
}

void Map::collapse(int x, int y, std::mt19937& rng, int& collapseCount)
{
	if (x < 0 || x >= this->WIDTH || y < 0 || y >= this->HEIGHT ||
		this->grid[x][y].collapsed || this->grid[x][y].possibilities.empty())
	{
		std::cerr << "Warning: Invalid collapse attempt at (" << x << ", " << y << ")\n";
		throw std::runtime_error("error collapsing");
		return;
	}

	Tile& tile = this->grid[x][y];
	this->eq.remove({ tile.possibilities.size(), tile.position });
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

			std::vector<int>& neighborPoss = neighborTile.possibilities;
			size_t originalSize = neighborPoss.size();

			if (sourceTile.collapsed)
			{
				std::unordered_set<int> validNeighbors = config.validNeighbors[sourceTile.type];

				neighborPoss.erase(
					std::remove_if(neighborPoss.begin(), neighborPoss.end(),
						[&validNeighbors](int type) {
							return validNeighbors.find(type) == validNeighbors.end();
						}),
					neighborPoss.end()
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
				neighborPoss.erase(
					std::remove_if(neighborPoss.begin(), neighborPoss.end(),
						[&validNeighbors](int type) {
							return validNeighbors.find(type) == validNeighbors.end();
						}),
					neighborPoss.end()
				);
			}

			if (neighborPoss.size() != originalSize)
			{
				this->eq.remove({ originalSize, neighborTile.position });
				this->eq.push({ neighborPoss.size(), neighborTile.position });
				if (neighborPoss.size() == 1)
				{
					collapse(neighborTile.position.x, neighborTile.position.y, this->seed, collapseCount);
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
	const int totalCells = this->WIDTH * this->HEIGHT;

	while (cellsCollapsed < totalCells)
	{
		glm::vec2 target;

		if (!this->eq.empty())
		{
			target = this->eq.top().second;
			this->eq.pop();
		}
		else
		{
			do
			{
				target = glm::vec2(disX(rng), disY(rng));
			} while (this->grid[target.x][target.y].collapsed);
		}

		if (!this->grid[target.x][target.y].collapsed)
		{
			collapse(target.x, target.y, rng, cellsCollapsed);
			propagate(target.x, target.y, cellsCollapsed);
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
		renderer.drawSprite(texture, tile.position * 32.0f, glm::vec2(32.0f, 32.0f));
	}
}
