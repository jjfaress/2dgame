#pragma once
#include "Map.h"
#include <vector>
#include <random>
#include "Texture.h"
#include <glm/glm.hpp>
#include "SpriteRenderer.h"
#include <queue>
#include "ConfigLoader.h"

struct compare {
	bool operator()(const std::pair<int, glm::vec2>& a,
		const std::pair<int, glm::vec2>& b) const
	{
		return a.first > b.first;
	}
};

template<typename T, typename Container = std::vector<T>, typename Comp = std::greater<T>>
struct EntropyQueue : public std::priority_queue<T, Container, Comp>
{
public:
	using Base = std::priority_queue<T, Container, Comp>;
	using Base::Base;
	bool remove(const T& value)
	{
		auto item = std::find(this->c.begin(), this->c.end(), value);

		if (item == this->c.end()) return false;
		if (item == this->c.begin()) this->pop();
		else
		{
			this->c.erase(item);
			std::make_heap(this->c.begin(), this->c.end(), this->comp);
		}
		return true;
	}
};

struct Chunk {
	Pattern pattern;
	std::vector<int> entropy;
	glm::vec2 position;
	bool collapsed = false;

	Chunk(glm::vec2 pos, std::vector<int> ent) : 
		position(pos), 
		entropy(ent) {}

	void draw(SpriteRenderer& renderer);
};

class WFCMap : public Map<Chunk>
{
public:
	bool initialized = false;
	bool isReady = false;
	WFCMap(int width,
		int height,
		unsigned int seed);
	~WFCMap() override;
	void generate();
	bool stepGenerate(SpriteRenderer& renderer);
	void init() override;
	void draw(SpriteRenderer& renderer);

private:

	std::vector<Chunk> finalChunks;
	unsigned int seed;
	EntropyQueue<std::pair<int, glm::vec2>, std::vector<std::pair<int, glm::vec2>>, compare> eq;
	void collapse(int x, int y, std::mt19937& rng, int& collapseCount);
	void collapse(int x, int y, unsigned int& seed, int& collapseCount);
	void propagate(int x, int y, int& collapseCount);
	//void postProcess() override;
};
