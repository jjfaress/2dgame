#pragma once
#include "Map.h"
#include "tileson.hpp"

struct CollisionBox {
	float top, bottom, left, right;
};

struct Layer {
	uint32_t width, height;
	std::vector<uint32_t> data;
};

class TiledMap : public Map {
public:
	TiledMap(std::string& mapPath, 
		std::string& spriteDir, 
		SpriteRenderer& renderer);
	void init() override;
	void draw() override;

private:
	int32_t tileWidth, tileHeight;
	std::vector<Layer> layers;
	std::vector<tson::Tile> tiles;
	std::vector<CollisionBox> collisions;

	std::vector<tson::Tile> parseTiles(tson::Tileset* tileset);
	void parseObjects(std::vector<tson::Object>& objects);
	std::vector<Layer> parseLayers(std::vector<tson::Layer>& layers);
};

