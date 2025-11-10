#pragma once
#include "SpriteRenderer.h"
#include "Collision.h"
#include <variant>
#include <nlohmann/json.hpp>

namespace TiledMap {

	using namespace Collision;
	using json = nlohmann::json;

	struct Temp {
		int32_t width, height, tileWidth, tileHeight;
		std::vector<json> layers;
		std::vector<CollisionObject> colliders;
		std::unordered_map<std::string, glm::vec2> POIs;
		std::unordered_map<int, std::vector<json>> tileObjects;
		Texture2D texture;
	};

	struct MapData {
		Texture2D texture;
		std::vector<CollisionObject> colliders;
		std::unordered_map<std::string, glm::vec2> POIs;
	};

	void parseTiles(
		json& tileset,
		const std::string& spriteDir,
		Temp& mapData);


	void parseLayers(
		json& layers,
		Temp& mapData);

	std::vector<CollisionObject> parseTileObjects(
		const std::vector<uint32_t>& layerData,
		Temp& mapData);

	CollisionObject buildObject(json& object);

	void drawToFrameBuffer(
		Temp& mapData, 
		SpriteRenderer& renderer);

	MapData loadMap(
		const std::string& mapPath,
		const std::string& spriteDir,
		SpriteRenderer& renderer);
}