#pragma once
#include "tileson.hpp"
#include "SpriteRenderer.h"
#include "Collision.h"
#include <variant>

namespace TiledMap {

	using namespace Collision;


	struct Temp {
		int32_t width, height, tileWidth, tileHeight;
		std::vector<tson::Layer> layers;
		std::vector<CollisionObject> worldCollisions;
		std::unordered_map<std::string, glm::vec2> POIs;
		std::unordered_map<int, std::vector<tson::Object>> tileObjects;
		Texture2D texture;
	};

	struct MapData {
		Texture2D texture;
		std::vector<CollisionObject> worldCollisions;
		std::unordered_map<std::string, glm::vec2> POIs;
	};

	void parseTiles(
		tson::Tileset* tileset,
		const std::string& spriteDir,
		Temp& mapData);

	void parseLayers(
		std::vector<tson::Layer>& layers,
		Temp& mapData);

	std::vector<CollisionObject> parseTileObjects(
		const std::vector<uint32_t>& layerData,
		Temp& mapData);

	CollisionObject buildObject(tson::Object& object);

	void drawToFrameBuffer(
		Temp& mapData, 
		SpriteRenderer& renderer);

	MapData loadMap(
		const std::string& mapPath,
		const std::string& spriteDir,
		SpriteRenderer& renderer);
}