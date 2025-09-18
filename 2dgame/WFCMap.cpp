#include "WFCMap.h"
#include <unordered_set>
#include <iostream>
#include "ResourceManager.h"
#include <omp.h>

WFCMap::WFCMap(int width, int height, unsigned int seed, const char* path) :
	Map(width, height, path ? ConfigLoader::getInstance(path) : ConfigLoader::getInstance()),
	seed(seed),
	isReady(false)
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
		std::vector<WFCTile> col;
		col.reserve(this->HEIGHT);
		for (int j = 0; j < this->HEIGHT; j++)
		{
			col.emplace_back(WFCTile(glm::vec2(i, j), config.patternTypes));
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

	//todo fix

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
			collapse(target.x, target.y, rng, cellsCollapsed);
			propagate(target.x, target.y, cellsCollapsed);
		}
	}
	this->isReady = true;
	postProcess();
}

void WFCMap::collapse(int x, int y, std::mt19937& rng, int& collapseCount)
{
	if (x < 0 || x >= this->WIDTH || y < 0 || y >= this->HEIGHT ||
		this->grid[x][y].collapsed || this->grid[x][y].entropy.empty())
	{
		std::cerr << "Warning: Invalid collapse attempt at (" << x << ", " << y << ")\n" << this->grid[x][y].entropy.size();
		std::cout << "collapsed: " << this->grid[x][y].collapsed << ", entropy: " << this->grid[x][y].entropy.size() << "\n";
	}

	WFCTile& tile = this->grid[x][y];
	this->eq.remove({ tile.entropy.size(), tile.position });
	std::uniform_int_distribution<std::mt19937::result_type> poss(0, tile.entropy.size() - 1);
	int patternId = tile.entropy[poss(rng)];
	tile.pattern = config.patterns[patternId];
	tile.collapsed = true;
	collapseCount++;
	tile.entropy.clear();
	tile.entropy.shrink_to_fit();
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

		struct Update {
			glm::ivec2 position;
			std::vector<int> newEntropy;
			size_t originalEntropy;
		};
		std::vector<Update> updates;
		updates.reserve(8);

		#pragma omp parallel for
		for (int dir = 0; dir < 8; dir++)
		{
			glm::ivec2 neighbor = source + directions[dir];

			if (neighbor.x < 0 || neighbor.x >= this->WIDTH ||
				neighbor.y < 0 || neighbor.y >= this->HEIGHT)
			{
				continue;
			}

			WFCTile& sourceTile = this->grid[source.x][source.y];
			WFCTile& neighborTile = this->grid[neighbor.x][neighbor.y];

			if (this->grid[neighbor.x][neighbor.y].collapsed)
			{
				continue;
			}

			std::vector<int> neighborPoss = neighborTile.entropy;
			size_t originalEntropy = neighborPoss.size();

			if (sourceTile.collapsed)
			{
				std::unordered_map<int, std::unordered_set<int>> validNeighborsMap = config.validNeighbors[sourceTile.type];
				std::unordered_set<int> validNeighbors = validNeighborsMap[dir];
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
				std::vector<int>& sourcePos = sourceTile.entropy;

				for (const int type : sourcePos)
				{
					std::unordered_map<int, std::unordered_set<int>> neighborsMap = config.validNeighbors[type];
					for (const auto& neighbor : neighborsMap[dir])
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

			if (neighborPoss.size() != originalEntropy)
			{
				#pragma omp critical
				{
					updates.push_back({ neighbor, std::move(neighborPoss), originalEntropy });
				}
			}
		}

		for (const Update& update : updates)
		{
			WFCTile& neighbor = this->grid[update.position.x][update.position.y];
			if (!neighbor.collapsed && neighbor.entropy.size() == update.originalEntropy)
			{
				this->eq.remove({ update.originalEntropy, neighbor.position });
				neighbor.entropy = update.newEntropy;
				this->eq.push({ neighbor.entropy.size(), neighbor.position });

				if (neighbor.entropy.size() == 1)
				{
					collapse(neighbor.position.x, neighbor.position.y, this->seed, collapseCount);
				}
				queue.push(update.position);
			}
		}
	}
}

void WFCMap::postProcess()
{
	if (this->isReady)
	{
		for (const auto& row : this->grid)
		{
			this->finalTiles.insert(this->finalTiles.end(), row.begin(), row.end());
		}
	}
}

void WFCMap::draw(SpriteRenderer& renderer)
{
	for (WFCTile& tile : this->finalTiles)
	{
		tile.draw(renderer);
	}
}


void WFCTile::draw(SpriteRenderer& renderer)
{
	ConfigLoader& config = ConfigLoader::getInstance();
	float tileSize = 16.0;
	float patternSize = tileSize * config.n;
	Grid<int> pattern = this->pattern.tiles;
	for (const auto& row : pattern)
	{
		for (const int& tile : row)
		{
			const char* texture = config.textures[tile].c_str();
			renderer.drawSprite(ResourceManager::getTexture(texture), position, glm::vec2(tileSize));

		}
	}
}