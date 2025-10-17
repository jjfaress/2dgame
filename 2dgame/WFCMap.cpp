#include "WFCMap.h"
#include "Globals.h"
#include <unordered_set>
#include <iostream>
#include "ResourceManager.h"
#include <stack>
#include <omp.h>
#include <mutex>

WFCMap::WFCMap(int width, int height, unsigned int seed, SpriteRenderer& renderer) :
	Map(width, height, renderer),
	rng(seed)
{
}

WFCMap::~WFCMap()
{
}

void WFCMap::init()
{
	this->grid.reserve(this->WIDTH);
	for (int i = 0; i < this->WIDTH; i++)
	{
		std::vector<Chunk> col;
		col.reserve(this->HEIGHT);
		for (int j = 0; j < this->HEIGHT; j++)
		{
			col.emplace_back(Chunk(glm::vec2(i, j), config.patternTypes));
			this->eq.push({ config.patternTypes.size(), glm::vec2(i,j) });
		}
		this->grid.emplace_back(std::move(col));
	}

	for (const auto& type : config.tileTypes)
	{
		ResourceManager::loadTexture(("assets/" + config.textures[type]).c_str(),
			config.textures[type]);
	}
	this->initialized = true;
}

void WFCMap::generate()
{
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
			target = glm::vec2(disX(rng), disY(rng));
		}

		if (!this->grid[target.x][target.y].collapsed)
		{
			collapse(target.x, target.y, cellsCollapsed);
			propagate(target.x, target.y, cellsCollapsed);
		}
	}
	this->isReady = true;
}


int cellsCollapsed = 0;


void WFCMap::stepGenerate()
{
	std::uniform_int_distribution<std::mt19937::result_type> disX(0, this->WIDTH - 1);
	std::uniform_int_distribution<std::mt19937::result_type> disY(0, this->HEIGHT - 1);

	const int totalCells = this->WIDTH * this->HEIGHT;
	if (cellsCollapsed < totalCells)
	{
		glm::vec2 target;
		if (!this->eq.empty())
		{
			target = this->eq.top().second;
			this->eq.pop();
		}
		else
		{
			target = glm::vec2(disX(rng), disY(rng));
		}

		if (!this->grid[target.x][target.y].collapsed)
		{
			collapse(target.x, target.y, cellsCollapsed);
			propagate(target.x, target.y, cellsCollapsed);
		}
	}
	else
	{
		this->isReady = true;
	}
	for (auto& row : this->grid)
	{
		for (Chunk& chunk : row)
		{
			if (chunk.collapsed)
			{
				chunk.draw(*renderer);
			}
		}
	}
}

void WFCMap::collapse(int x, int y, int& collapseCount)
{
	if (x < 0 || x >= this->WIDTH || y < 0 || y >= this->HEIGHT ||
		this->grid[x][y].collapsed || this->grid[x][y].entropy.empty())
	{
		std::cerr << "Warning: Invalid collapse attempt at (" << x << ", " << y << ")\n";
		std::cout << "collapsed: " << this->grid[x][y].collapsed << ", entropy: " << this->grid[x][y].entropy.size() << "\n";
		return;
	}
	Chunk& chunk = this->grid[x][y];
	this->eq.remove({ chunk.entropy.size(), chunk.position });
	//std::uniform_int_distribution<std::mt19937::result_type> poss(0, chunk.entropy.size() - 1);

	std::vector<double> weights;
	for (int& id : chunk.entropy)
	{
		weights.push_back(config.patterns[id].frequency);
	}
	std::discrete_distribution<int> poss(weights.begin(), weights.end());

	int patternId = chunk.entropy[poss(rng)];
	chunk.pattern = config.patterns[patternId];
	chunk.collapsed = true;
	collapseCount++;
	chunk.entropy.clear();
	chunk.entropy.shrink_to_fit();
	chunk.draw(*renderer);
}


void WFCMap::propagate(int x, int y, int& collapseCount)
{
	std::queue<glm::ivec2> queue;
	queue.push(glm::ivec2(x, y));

	std::vector<glm::ivec2> directions = {
		glm::ivec2(0, 1), //north
		glm::ivec2(1, 1), //north east
		glm::ivec2(1, 0), //east
		glm::ivec2(1, -1), //south east
		glm::ivec2(0, -1), //south
		glm::ivec2(-1, -1), //south west
		glm::ivec2(-1, 0), //west
		glm::ivec2(-1, 1) //north west
	};

	while (!queue.empty())
	{
		glm::ivec2 source = queue.front();
		queue.pop();

		for (int dir = 0; dir < directions.size(); dir++)
		{
			glm::ivec2 neighbor = source + directions[dir];

			if (neighbor.x < 0 || neighbor.x >= this->WIDTH ||
				neighbor.y < 0 || neighbor.y >= this->HEIGHT)
			{
				continue;
			}

			Chunk& sourceChunk = this->grid[source.x][source.y];
			Chunk& neighborChunk = this->grid[neighbor.x][neighbor.y];

			if (neighborChunk.collapsed)
			{
				continue;
			}



			std::vector<int>& neighborPoss = neighborChunk.entropy;
			size_t originalEntropy = neighborPoss.size();


			if (sourceChunk.collapsed)
			{

				std::unordered_set<int>& validNeighbors = config.validNeighbors[sourceChunk.pattern.id][dir];

				int validCount = std::count_if(neighborPoss.begin(), neighborPoss.end(),
					[&validNeighbors](int pattern) {
						return validNeighbors.find(pattern) != validNeighbors.end();
					}
				);
				if (validCount > 0)
				{
					neighborPoss.erase(
						std::remove_if(neighborPoss.begin(), neighborPoss.end(),
							[&validNeighbors](int pattern) {
								return validNeighbors.find(pattern) == validNeighbors.end();
							}),
						neighborPoss.end()
					);
				}
			}
			else
			{
				std::unordered_set<int> validNeighbors;
				std::vector<int>& sourcePos = sourceChunk.entropy;

				for (const int& pattern : sourcePos)
				{
					std::unordered_map<int, std::unordered_set<int>> neighborsMap = config.validNeighbors[pattern];

					for (const int& neighbor : neighborsMap[dir])
					{
						validNeighbors.insert(neighbor);
					}

				}

				int validCount = std::count_if(neighborPoss.begin(), neighborPoss.end(),
					[&validNeighbors](int pattern) {
						return validNeighbors.find(pattern) != validNeighbors.end();
					}
				);

				if (validCount > 0)
				{
					neighborPoss.erase(
						std::remove_if(neighborPoss.begin(), neighborPoss.end(),
							[&validNeighbors](int pattern) {
								return validNeighbors.find(pattern) == validNeighbors.end();
							}),
						neighborPoss.end()
					);
				}
			}

			if (neighborPoss.size() != originalEntropy && !neighborChunk.collapsed)
			{

				this->eq.remove({ originalEntropy, neighborChunk.position });
				this->eq.push({ neighborPoss.size(), neighborChunk.position });
				if (neighborPoss.size() == 1)
				{
					collapse(
						neighborChunk.position.x,
						neighborChunk.position.y,
						collapseCount);
				}
				queue.push(glm::ivec2(neighbor.x, neighbor.y));
			}
		}

	}
}


void WFCMap::draw()
{
	for (auto& row : this->grid)
	{
		for (Chunk& chunk : row)
		{
			chunk.draw(*renderer);
		}
	}
}


void Chunk::draw(SpriteRenderer& renderer)
{
	float tileSize = 16.0;
	float chunkSize = tileSize * config.n;
	Grid<int>& pattern = this->pattern.tiles;
	for (int x = 0; x < pattern.size(); x++)
	{
		for (int y = 0; y < pattern[0].size(); y++)
		{
			int tile = pattern[x][y];
			const char* texName = config.textures[tile].c_str();
			Texture2D texture = ResourceManager::getTexture(texName);
			glm::vec2 tilePos = 
				(this->position * chunkSize) + glm::vec2(x * tileSize, y * tileSize);
			renderer.drawSprite(texture, tilePos, TL, glm::vec2(0.125));
		}
	}
}