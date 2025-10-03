#include "ConfigLoader.h"
#include <iostream>
#include "stb_image.h"
#include "ResourceManager.h"

ConfigLoader::ConfigLoader(const char* path)
{
	try
	{
		YAML::Node config = YAML::LoadFile(path);

		if (config["pattern_size"])
		{
			this->n = config["pattern_size"].as<int>();
		}
		else
		{
			throw std::runtime_error(
				std::string("Error: Pattern size not defined in ") + path);
		}
		if (config["seed"])
		{
			this->seed = config["seed"].as<unsigned int>();
		}
		if (config["map_width"] && config["map_height"])
		{
			this->mapWidth = config["map_width"].as<int>();
			this->mapHeight = config["map_height"].as<int>();
		}
		else
		{
			throw std::runtime_error(
				std::string("Error: Map dimensions not fully defined in ") + path);
		}
		if (config["tile_data"])
		{
			loadTileData(config["tile_data"]);
		}
		else
		{
			throw std::runtime_error(
				std::string("Error: Tile data not defined in ") + path);
		}
		if (config["bitmap"])
		{
			loadBitmap(config["bitmap"].as<std::string>().c_str());
		}
		else
		{
			throw std::runtime_error(
				std::string("Error: Bitmap not defined in ") + path);
		}
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "Config not loaded :: YAML error: " << e.what() << std::endl;
	}
	catch (std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

ConfigLoader& ConfigLoader::getInstance(const char* path)
{
	static ConfigLoader instance(path);
	return instance;
}

void ConfigLoader::loadTileData(const YAML::Node& node)
{
	for (const auto& pair : node)
	{
		glm::vec4 color = pair.second["bitColor"].as<glm::vec4>();
		int id = pair.first.as<int>();
		this->tileIds[color] = id;
		std::string texName = pair.second["texture"].as<std::string>();
		//ResourceManager::loadTexture(texName.c_str(), true, texName);
		this->textures[id] = texName;
		this->tileTypes.push_back(id);
	}
}


void ConfigLoader::loadBitmap(const char* file)
{
	std::vector<glm::ivec2> directions = {
		glm::ivec2(0, 1), //north
		glm::ivec2(1, 1), //north east
		glm::ivec2(1, 0), //east
		glm::ivec2(1, -1), //south east
		glm::ivec2(0, -1), //south
		glm::ivec2(-1, -1), //south west
		glm::ivec2(-1, 0), //west
		glm::ivec2(-1, 1) //north west
	};

	int width, height, channels;
	unsigned char* data = stbi_load(file, &width, &height, &channels, 0);
	if (!data)
	{
		std::cerr << "Failed to load bitmap: " << stbi_failure_reason() << std::endl;
		return;
	}

	Grid<int> bitmap(height, std::vector<int>(width));

	int patternCount = 0;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int idx = (y * width + x) * channels;
			unsigned char r = data[idx];
			unsigned char g = data[idx + 1];
			unsigned char b = data[idx + 2];
			unsigned char a = data[idx + 3];
			glm::vec4 color(r, g, b, a);
			bitmap[x][y] = this->tileIds[color];
		}
	}

	stbi_image_free(data);

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Pattern source = extractPattern(bitmap, x, y);
			if (patternIsUnique(source))
			{
				source.id = patternCount++;
				this->patternIds[source] = source.id;
				this->patterns[source.id] = source;
				this->patternTypes.push_back(source.id);
			}
			else
			{
				source.id = this->patternIds[source];
				this->patterns[source.id].frequency++;
			}


			for (int dir = 0; dir < directions.size(); dir++)
			{
				glm::ivec2 direction = directions[dir];
				glm::ivec2 stride = glm::ivec2(x, y) + (direction * this->n);

				Pattern neighbor = extractPattern(bitmap, stride.x, stride.y);

				if (patternIsUnique(neighbor))
				{
					neighbor.id = patternCount++;
					this->patternIds[neighbor] = neighbor.id;
					this->patterns[neighbor.id] = neighbor;
					this->patternTypes.push_back(neighbor.id);
				}
				else
				{
					neighbor.id = this->patternIds[neighbor];
					this->patterns[neighbor.id].frequency++;
				}
				this->validNeighbors[source.id][dir].insert(neighbor.id);
			}
		}
	}
}

Pattern ConfigLoader::extractPattern(const Grid<int>& bitmap, int x, int y)
{
	Pattern pattern(-1);
	int width = bitmap[0].size();
	int height = bitmap.size();
	pattern.tiles.resize(this->n, std::vector<int>(this->n));
	for (int i = 0; i < this->n; i++)
	{
		for (int j = 0; j < this->n; j++)
		{
			int sourceX = (x + i) % width;
			int sourceY = (y + j) % height;
			if (sourceX < 0) sourceX += width;
			if (sourceY < 0) sourceY += height;

			pattern.tiles[i][j] = bitmap[sourceX][sourceY];
		}
	}
	return pattern;
}

bool ConfigLoader::patternIsUnique(Pattern& pattern)
{
	if (this->patternIds.find(pattern) == this->patternIds.end())
	{
		return true;
	}
	return false;
}

Pattern ConfigLoader::rotate(const Pattern& pattern, int rot)
{
	Pattern rotated(-1);
	rotated.tiles = pattern.tiles;

	for (int r = 0; r < (rot % 4); r++)
	{
		std::vector<std::vector<int>> temp = rotated.tiles;
		int size = this->n;

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				rotated.tiles[j][size - 1 - i] = temp[i][j];
			}
		}
	}

	return rotated;
}

Pattern ConfigLoader::flipHor(const Pattern& pattern)
{
	Pattern flipped(-1);
	int size = this->n;
	flipped.tiles.resize(this->n, std::vector<int>(this->n));

	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			flipped.tiles[x][y] = pattern.tiles[size - 1 - x][y];
		}
	}
	return flipped;
}

Pattern ConfigLoader::flipVer(const Pattern& pattern)
{
	Pattern flipped(-1);
	int size = this->n;
	flipped.tiles.resize(this->n, std::vector<int>(this->n));

	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			flipped.tiles[x][y] = pattern.tiles[x][size - 1 - y];
		}
	}
	return flipped;
}

std::vector<Pattern> ConfigLoader::generateSymmetries(const Pattern& base)
{
	std::vector<Pattern> symmetries;
	std::unordered_set<Pattern, PatternHash> symmetrySet;

	for (int rot = 0; rot < 4; rot++)
	{
		Pattern rotated = rotate(base, rot);
		//symmetriesSet.insert(rotated);
		if (symmetrySet.find(rotated) == symmetrySet.end())
		{
			symmetrySet.insert(rotated);
			symmetries.push_back(rotated);
		}

		Pattern flippedH = flipHor(base);
		if (symmetrySet.find(flippedH) == symmetrySet.end())
		{
			symmetrySet.insert(flippedH);
			symmetries.push_back(flippedH);
		}

		Pattern flippedV = flipHor(base);
		if (symmetrySet.find(flippedV) == symmetrySet.end())
		{
			symmetrySet.insert(flippedV);
			symmetries.push_back(flippedV);
		}
	}
	return symmetries;
}

