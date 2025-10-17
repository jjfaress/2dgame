#include "TiledMap.h"
#include "ResourceManager.h"

TiledMap::TiledMap(std::string& mapPath, std::string& spriteDir, SpriteRenderer& renderer) :
	Map(renderer)
{
	tson::Tileson t;
	std::unique_ptr<tson::Map> map = t.parse(mapPath);
	if (map->getStatus() == tson::ParseStatus::OK)
	{
		this->WIDTH = map->getSize().x;
		this->HEIGHT = map->getSize().y;
		this->tileWidth = map->getTileSize().x;
		this->tileHeight = map->getTileSize().y;
		this->tiles = parseTiles(map->getTileset("ts"));
		this->layers = parseLayers(map->getLayers());
	}
}

std::vector<tson::Tile> TiledMap::parseTiles(tson::Tileset* tileset)
{
	for (int i = 1; i <= tileset->getTileCount(); i++)
	{
		tson::Tile* tile = tileset->getTile(i);
		std::string texName = tile->getImage().string();
		std::string texPath = "assets/sprites/" + texName;
		ResourceManager::loadTexture(texPath.c_str(), std::to_string(i));
		auto& og = tile->getObjectgroup();
		parseObjects(og.getObjects());
	}
	return tileset->getTiles();
}

void TiledMap::parseObjects(std::vector<tson::Object>& objects)
{
	for (const auto& obj : objects)
	{
		if (obj.getType() == "collision")
		{
			CollisionBox box;
			box.left = obj.getPosition().x;
			box.top = obj.getPosition().y;
			box.right = obj.getSize().x;
			box.bottom = obj.getSize().y;
			this->collisions.push_back(box);
		}
		else if (obj.getType() == "POI")
		{
			this->POIs[obj.getName()] = glm::vec2(
				std::floorf(obj.getPosition().x),
				std::floorf(obj.getPosition().y)
			);
		}
	}
}

std::vector<tson::Layer> TiledMap::parseLayers(std::vector<tson::Layer>& layers)
{
	std::vector<tson::Layer> newLayers;
	for (auto& layer : layers)
	{
		if (layer.getTypeStr() == "objectgroup")
		{
			parseObjects(layer.getObjects());
			continue;
		}
		newLayers.push_back(layer);
	}
	return newLayers;
}

void TiledMap::init()
{
}

void TiledMap::draw()
{
	for (const auto& layer : this->layers)
	{
		std::vector<uint32_t> data = layer.getData();
		for (int y = 0; y < layer.getSize().y; y++)
		{
			for (int x = 0; x < layer.getSize().x; x++)
			{
				int idx = (y * layer.getSize().x) + x;
				if (data[idx] == 0) continue;
				this->renderer->drawSprite(
					ResourceManager::getTexture(std::to_string(data[idx])),
					glm::vec2((x * this->tileWidth), (y * this->tileHeight)),
					BL
				);
			}
		}
	}
}
