#include "Game.h"
#include "ResourceManager.h"
#include <utility>
#include <array>
#include <GLFW/glfw3.h>

Game::Game(uint screen_width, uint screen_height) :
	WIDTH(screen_width),
	HEIGHT(screen_height)
{
}

Game::~Game()
{
	this->renderer.destroy();
}

void Game::init()
{
	this->camera.init(static_cast<float>(this->WIDTH), static_cast<float>(this->HEIGHT));

	Shader spriteShader =
		ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
	spriteShader.use().setInt("image", 0);
	this->renderer.init(spriteShader);

	std::string spriteDir("assets/sprites/");
	std::string mapPath(spriteDir + "map.json");
	TiledMap::MapData md = TiledMap::loadMap(mapPath, spriteDir, this->renderer);

	this->map = md.texture;
	this->colliders.insert(
		this->colliders.end(), md.colliders.begin(), md.colliders.end());

	this->player.init("cat.json", spriteDir, md.POIs["playerSpawn"]);
	CollisionBox<RECTANGLE> pc;
	pc.size = glm::vec2(this->player.sprite.HEIGHT);
	pc.position = md.POIs["playerSpawn"];

	pc.rotation = 0;
	this->player.collider = pc;
}

void Game::update(float dt)
{
	this->player.update(dt);
}

void Game::tick()
{
	this->player.move();
	std::vector<CollisionObject> allColliders(this->colliders);
	allColliders.push_back(this->player.collider);
	checkCollision(allColliders);
}

void Game::render(float alpha)
{
	updateCamera(alpha);
	this->renderer.drawSprite(this->map, glm::vec2(0));
	this->player.draw(this->renderer, alpha);
}

void Game::processInput(float dt)
{
	Character& player = this->player;
	if (this->keys[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
	}

	glm::vec2 inputDir(0.0);

	if (this->keys[GLFW_KEY_W]) inputDir.y -= 1;
	if (this->keys[GLFW_KEY_A]) inputDir.x -= 1;
	if (this->keys[GLFW_KEY_S]) inputDir.y += 1;
	if (this->keys[GLFW_KEY_D]) inputDir.x += 1;

	if (glm::length(inputDir) > 0)
	{
		player.setSpeed(glm::normalize(inputDir));

		if (this->keys[GLFW_KEY_LEFT_SHIFT])
		{
			player.setState(RUNNING);
		}
		else
		{
			player.setState(WALKING);
		}
	}
	else
	{
		player.setSpeed(glm::vec2(0.0));
		player.setState(IDLE);
	}
}

void Game::updateCamera(float alpha)
{
	glm::vec2 interpolatedPlayerPos = glm::mix(
		this->player.prevPos,
		this->player.position,
		alpha
	);

	glm::vec2 cameraPos = interpolatedPlayerPos
		- glm::vec2(this->WIDTH / 2.0f, this->HEIGHT / 2.0f);

	if (cameraPos != this->camera.lastPosition)
	{
		this->camera.setPosition(cameraPos);
		this->renderer.shader.setMat4("projection", this->camera.getProjectionMatrix());
		this->camera.lastPosition = cameraPos;
	}
}

void Game::checkCollision(std::vector<CollisionObject>& objects)
{
	Grid grid = buildSpacialGrid(objects, 32.0f);
	auto pairs = getAdjacentPairs(grid);

	int playerIdx = objects.size() - 1;

	for (auto& [i, j] : pairs)
	{
		CollisionObject& a = objects[i];
		CollisionObject& b = objects[j];
		bool collided = std::visit([&](auto&& aCol, auto&& bCol) {
			using aType = std::decay_t<decltype(aCol)>;
			using bType = std::decay_t<decltype(bCol)>;

			if constexpr (
				std::is_same_v<aType, CollisionBox<RECTANGLE>> &&
				std::is_same_v<bType, CollisionBox<RECTANGLE>>
				)
			{
				return checkAABB(aCol, bCol);
			}
			return false;

			}, a, b);

		if (collided)
		{
			if (i == playerIdx || j == playerIdx)
			{
				this->player.position = this->player.prevPos;
				std::visit([&](auto& c) {
					c.position = this->player.position;
					}, this->player.collider);
			}
		}
	}

}
