#include "TiledMap.h"
#include "ResourceManager.h"
#include <glad/glad.h>

namespace TiledMap {

	void parseTiles(
		tson::Tileset* tileset,
		const std::string& spriteDir,
		Temp& mapData)
	{
		std::unordered_map<int, std::vector<tson::Object>> newObjects;
		for (int i = 1; i <= tileset->getTileCount(); i++)
		{
			tson::Tile* tile = tileset->getTile(i);
			std::string texName(tile->getImage().string());
			ResourceManager::loadTexture(
				(spriteDir + texName).c_str(), std::to_string(i), true);

			auto& objects = tile->getObjectgroup().getObjects();
			if (!objects.empty())
			{
				mapData.tileObjects[i] = objects;
			}
		}
	}

	void parseLayers(std::vector<tson::Layer>& layers, Temp& mapData)
	{
		std::vector<tson::Layer> tileLayers;
		std::vector<CollisionObject> collisions;
		for (auto& layer : layers)
		{
			if (layer.getType() == tson::LayerType::TileLayer)
			{
				tileLayers.push_back(layer);
				const std::vector<uint32_t>& data = layer.getData();
				std::vector<CollisionObject> tileObjects = parseTileObjects(data, mapData);
				collisions.insert(collisions.end(), tileObjects.begin(), tileObjects.end());
			}
			else if (layer.getType() == tson::LayerType::ObjectGroup)
			{
				for (auto& object : layer.getObjects())
				{
					if (object.getType() == "collision")
					{
						collisions.push_back(buildObject(object));
					}
					else if (object.getType() == "POI")
					{
						mapData.POIs[object.getName()] = {
							object.getPosition().x,
							object.getPosition().y
						};
					}
				}
			}
		}
		mapData.layers = tileLayers;
		mapData.worldCollisions = collisions;
	}

	std::vector<CollisionObject> parseTileObjects(
		const std::vector<uint32_t>& data,
		Temp& mapData)
	{
		std::vector<CollisionObject> collisions;
		for (int y = 0; y < mapData.height; y++)
		{
			for (int x = 0; x < mapData.width; x++)
			{
				int idx = (y * mapData.width) + x;

				if (mapData.tileObjects.contains(data[idx]))
				{
					for (auto& object : mapData.tileObjects[data[idx]])
					{
						glm::vec2 newPos(
							(x * mapData.tileWidth) + object.getPosition().x,
							(y * mapData.tileHeight) + object.getPosition().y
						);
						CollisionObject cb = buildObject(object);
						std::visit([&](auto& box) {
							box.position = newPos;
							}, cb);
						collisions.push_back(cb);
					}
				}
			}
		}
		return collisions;
	}

	CollisionObject buildObject(tson::Object& object)
	{
		if (object.isPoint())
		{
			CollisionBox<POINT> cb{};
			return cb;
		}
		if (object.isEllipse())
		{
			CollisionBox<ELLIPSE> cb{};
			cb.size = { object.getSize().x, object.getSize().y };
			cb.rotation = object.getRotation();
			return cb;
		}
		if (!object.getPolygons().empty())
		{
			CollisionBox<POLYGON> cb{};
			cb.rotation = object.getRotation();
			std::vector<glm::vec2> points;
			for (auto& point : object.getPolygons())
			{
				points.push_back({ point.x, point.y });
			}
			cb.points = points;
			return cb;
		}
		CollisionBox<RECTANGLE> cb{};
		cb.size = { object.getSize().x, object.getSize().y };
		cb.rotation = object.getRotation();
		return cb;
	}

	void drawToFrameBuffer(Temp& mapData, SpriteRenderer& renderer)
	{
		unsigned int fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		int texWidth = mapData.width * mapData.tileWidth;
		int texHeight = mapData.height * mapData.tileHeight;
		mapData.texture.generate(texWidth, texHeight, nullptr);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			mapData.texture.ID,
			0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "problem setting up framebuffer";
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &fbo);
			return;
		}

		int oldVp[4];
		glGetIntegerv(GL_VIEWPORT, oldVp);
		glViewport(0, 0, texWidth, texHeight);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		glm::mat4 projection = glm::ortho(
			0.0f, (float)texWidth,
			(float)texHeight, 0.0f,
			-1.0f, 1.0f);

		renderer.shader.use();
		renderer.shader.setMat4("projection", projection);

		for (auto& layer : mapData.layers)
		{
			std::vector<uint32_t> data = layer.getData();
			for (int y = 0; y < layer.getSize().y; y++)
			{
				for (int x = 0; x < layer.getSize().x; x++)
				{
					int idx = (y * mapData.width) + x;
					if (data[idx] == 0) continue;
					renderer.drawSprite(
						ResourceManager::getTexture(std::to_string(data[idx])),
						{ x * mapData.tileWidth, (mapData.height - y - 1) * mapData.tileHeight},
						TL);
				}
			}
		}
		glViewport(oldVp[0], oldVp[1], oldVp[2], oldVp[3]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);
	}

	MapData loadMap(
		const std::string& mapPath, 
		const std::string& spriteDir, 
		SpriteRenderer& renderer)
	{
		tson::Tileson t;
		std::unique_ptr<tson::Map> map = t.parse(mapPath);
		Temp temp{};
		if (map->getStatus() == tson::ParseStatus::OK)
		{
			temp.width = map->getSize().x;
			temp.height = map->getSize().y;
			temp.tileWidth = map->getTileSize().x;
			temp.tileHeight = map->getTileSize().y;
			parseTiles(map->getTileset("ts"), spriteDir, temp);
			parseLayers(map->getLayers(), temp);
			drawToFrameBuffer(temp, renderer);
		}
		return MapData({ temp.texture, temp.worldCollisions, temp.POIs });
	}
}