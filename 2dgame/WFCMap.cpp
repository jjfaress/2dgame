#include "WFCMap.h"
#include "Globals.h"
#include <unordered_set>
#include <iostream>
#include "ResourceManager.h"
#include <omp.h>


WFCMap::WFCMap(int width, int height, unsigned int seed) :
	Map(width, height),
	seed(seed)
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
		ResourceManager::loadTexture(("assets/" + config.textures[type]).c_str(), false, config.textures[type]);
	}
	this->initialized = true;
}


void WFCMap::generate()
{
	std::mt19937 rng(this->seed);
	std::uniform_int_distribution<std::mt19937::result_type> disX(0, this->WIDTH - 1);
	std::uniform_int_distribution<std::mt19937::result_type> disY(0, this->HEIGHT - 1);

	int cellsCollapsed = 0;
	const int totalCells = this->WIDTH * this->HEIGHT;

	while (cellsCollapsed < totalCells)
	{
		glm::vec2 target;
		std::cout << cellsCollapsed << "\n";
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
			collapse(target.x, target.y, rng, cellsCollapsed);
			propagate(target.x, target.y, cellsCollapsed);
		}
	}
	this->isReady = true;
}

void WFCMap::collapse(int x, int y, std::mt19937& rng, int& collapseCount)
{
	if (x < 0 || x >= this->WIDTH || y < 0 || y >= this->HEIGHT ||
		this->grid[x][y].collapsed || this->grid[x][y].entropy.empty())
	{
		std::cerr << "Warning: Invalid collapse attempt at (" << x << ", " << y << ")\n";
		std::cout << "collapsed: " << this->grid[x][y].collapsed << ", entropy: " << this->grid[x][y].entropy.size() << "\n";
	}

	Chunk& chunk = this->grid[x][y];
	this->eq.remove({ chunk.entropy.size(), chunk.position });
	std::uniform_int_distribution<std::mt19937::result_type> poss(0, chunk.entropy.size() - 1);
	int patternId = chunk.entropy[poss(rng)];
	chunk.pattern = config.patterns[patternId];
	chunk.collapsed = true;
	collapseCount++;
	chunk.entropy.clear();
	chunk.entropy.shrink_to_fit();
}

void WFCMap::collapse(int x, int y, unsigned int& seed, int& collapseCount)
{
	std::mt19937 rng(seed);
	collapse(x, y, rng, collapseCount);
}

void WFCMap::propagate(int x, int y, int& collapseCount)
{
	std::queue<glm::ivec2> queue;
	queue.push(glm::ivec2(x, y));
	glm::ivec2 directions[] = {
		glm::ivec2(0, 1), //north
		//glm::ivec2(1, 1), //north east
		glm::ivec2(1, 0), //east
		//glm::ivec2(1, -1), //south east
		glm::ivec2(0, -1), //south
		//glm::ivec2(-1, -1), //south west
		glm::ivec2(-1, 0), //west
		//glm::ivec2(-1, 1) //north west
	};

	while (!queue.empty())
	{
		glm::ivec2 source = queue.front();
		queue.pop();
		
		for (int dir = 0; dir < 4; dir++)
		{
			glm::ivec2 neighbor = source + directions[dir];

			if (neighbor.x < 0 || neighbor.x >= this->WIDTH ||
				neighbor.y < 0 || neighbor.y >= this->HEIGHT)
			{
				continue;
			}

			Chunk& sourceChunk = this->grid[source.x][source.y];
			Chunk& neighborChunk = this->grid[neighbor.x][neighbor.y];
			
			if (this->grid[neighbor.x][neighbor.y].collapsed)
			{
				continue;
			}

			std::vector<int>& neighborPoss = neighborChunk.entropy;
			size_t originalEntropy = neighborPoss.size();

			//std::cout << config.validNeighbors[sourceChunk.pattern.id][dir].size();

			if (sourceChunk.collapsed)
			{
				std::unordered_set<int>& validNeighbors = config.validNeighbors[sourceChunk.pattern.id][dir];
				neighborPoss.erase(
					std::remove_if(neighborPoss.begin(), neighborPoss.end(),
						[&validNeighbors](int pattern) {
							return validNeighbors.find(pattern) == validNeighbors.end();
						}),
					neighborPoss.end()
				);
			}
			else
			{
				std::unordered_set<int> validNeighbors;
				std::vector<int>& sourcePos = sourceChunk.entropy;

				for (const int& type : sourcePos)
				{
					std::unordered_map<int, std::unordered_set<int>> neighborsMap = config.validNeighbors[type];
					for (const int& neighbor : neighborsMap[dir])
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

			if (neighborPoss.size() != originalEntropy && !neighborChunk.collapsed)
			{
				this->eq.remove({ originalEntropy, neighborChunk.position });
				this->eq.push({ neighborPoss.size(), neighborChunk.position });
				if (neighborPoss.size() == 1)
				{
					collapse(neighborChunk.position.x, neighborChunk.position.y, this->seed, collapseCount);
				}
				queue.push(glm::vec2(neighbor.x, neighbor.y));
			}
		}

	}
}

void WFCMap::draw(SpriteRenderer& renderer)
{
	for (auto& row : this->grid)
	{
		for (Chunk& chunk : row)
		{
			chunk.draw(renderer);
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
			const char* texture = config.textures[tile].c_str();
			glm::vec2 tilePosition = (this->position * chunkSize) + glm::vec2(x * tileSize, y * tileSize);
			renderer.drawSprite(ResourceManager::getTexture(texture), tilePosition, glm::vec2(tileSize));
		}
	}
}