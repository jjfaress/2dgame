#pragma once
#ifndef WFCMAP_H
#define WFCMAP_H
#include "Map.h"
#include <vector>
#include "ConfigLoader.h"
#include <random>
#include "Texture.h"
#include <glm/glm.hpp>
#include "SpriteRenderer.h"
#include <queue>
#include "GameObject.h"

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

struct WFCTile : public Tile {
	std::vector<int> entropy;
	bool collapsed = false;
	int type = NULL;
	const char* texture = nullptr;
	WFCTile(glm::vec2 position, std::vector<int> poss) : 
		Tile(position),
		entropy(poss) {}
	void draw(SpriteRenderer& renderer);
};

class WFCMap : public Map<WFCTile>
{
public:
	bool isReady;
	WFCMap(int width, int height, unsigned int seed);
	~WFCMap() override;
	void generate();
	void init() override;
	void draw(SpriteRenderer& renderer);

private:
	std::vector<WFCTile> finalTiles;
	unsigned int seed;
	ConfigLoader& config;
	EntropyQueue<std::pair<int, glm::vec2>, std::vector<std::pair<int, glm::vec2>>, compare> eq;
	void collapse(int x, int y, std::mt19937& rng, int& collapseCount);
	void collapse(int x, int y, unsigned int& seed, int& collapseCount);
	void propagate(int x, int y, int& collapseCount);
	void postProcess() override;
};
#endif
