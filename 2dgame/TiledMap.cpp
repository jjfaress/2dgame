#include "TiledMap.h"
#include "ResourceManager.h"
#include <glad/glad.h>
#include <fstream>

namespace TiledMap {

	void parseTiles(json& tilesets, const std::string& spriteDir, Temp& mapData)
	{
		for (auto& tileset : tilesets)
		{
			for (auto& tile : tileset["tiles"])
			{
				int i = tile["id"].get<int>() + 1;
				std::string texName = tile["image"];
				ResourceManager::loadTexture(
					(spriteDir + texName).c_str(), std::to_string(i), true);
				if (tile.contains("objectgroup"))
				{
					mapData.tileObjects[i] = tile["objectgroup"]["objects"];
				}
			}
		}
	}

	void parseLayers(json& layers, Temp& mapData)
	{
		std::vector<json> tileLayers;
		std::vector<CollisionObject> collisions;
		for (auto& layer : layers)
		{
			if (layer["type"] == "tilelayer")
			{
				tileLayers.push_back(layer);
				const std::vector<uint32_t>& data = layer["data"].get<std::vector<uint32_t>>();
				std::vector<CollisionObject> tileObjects = parseTileObjects(data, mapData);
				collisions.insert(collisions.end(), tileObjects.begin(), tileObjects.end());
			}
			else if (layer["type"] == "objectgroup")
			{
				for (auto& object : layer["objects"])
				{
					if (object["type"] == "collision")
					{
						collisions.push_back(buildObject(object));
					}
					else if (object["type"] == "POI")
					{
						mapData.POIs[object["name"]] = {
							object["x"], object["y"]
						};
					}
				}
			}
		}
		mapData.layers = tileLayers;
		mapData.colliders = collisions;
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
							(x * mapData.tileWidth) + object["x"],
							(y * mapData.tileHeight) + object["y"]
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

	CollisionObject buildObject(json& object)
	{
		if (object.contains("point"))
		{
			CollisionBox<POINT> cb{};
			cb.position = {
				object["x"],
				object["y"]
			};
			return cb;
		}
		if (object.contains("ellipse"))
		{
			if (object["width"] == object["height"])
			{
				CollisionBox<CIRCLE> cb{};
				cb.position = {
					object["width"] / 2,
					object["height"] / 2 };
				cb.radius = object["width"] / 2;
				return cb;
			}
			CollisionBox<ELLIPSE> cb{};
			cb.position = {
				object["x"],
				object["y"] };
			cb.size.x = object["width"];
			cb.size.y = object["height"];
			cb.rotation = object["rotation"];
			return cb;
		}
		if (object.contains("polygon"))
		{
			CollisionBox<POLYGON> cb{};
			cb.position = { object["x"], object["y"] };
			std::vector<glm::vec2> points;
			for (auto& point : object["polygon"])
			{
				points.push_back({ point["x"], point["y"] });
			}
			cb.points = points;
			cb.rotation = object["rotation"];
			return cb;
		}
		CollisionBox<RECTANGLE> cb{};
		cb.position.x = object["x"];
		cb.position.y = object["y"];
		cb.size.x = object["width"];
		cb.size.y = object["height"];
		cb.rotation = object["rotation"];
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
			std::vector<uint32_t> data = layer["data"];
			for (int y = 0; y < layer["height"]; y++)
			{
				for (int x = 0; x < layer["width"]; x++)
				{
					int idx = (y * mapData.width) + x;
					if (data[idx] == 0) continue;
					renderer.drawSprite(
						ResourceManager::getTexture(std::to_string(data[idx])),
						//{ x * mapData.tileWidth, y * mapData.tileHeight },
						{ x * mapData.tileWidth, (mapData.height - y - 1) * mapData.tileHeight },
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
		std::ifstream f(mapPath);
		if (!f.is_open())
		{
			std::cout << "ERROR: Could not open file: " << mapPath << std::endl;
		}
		json map = json::parse(f);
		Temp temp{};
		temp.width = map["width"];
		temp.height = map["height"];
		temp.tileWidth = map["tilewidth"];
		temp.tileHeight = map["tileheight"];
		parseTiles(map["tilesets"], spriteDir, temp);
		parseLayers(map["layers"], temp);
		drawToFrameBuffer(temp, renderer);
		return MapData({ temp.texture, temp.colliders, temp.POIs });
	}
}