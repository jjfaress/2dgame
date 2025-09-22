#include "pch.h"
#include "../2dgame/WFCMap.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../2dgame/SpriteRenderer.h"
#include "../2dgame/ResourceManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GLTestContext {
	GLFWwindow* window = nullptr;
	GLTestContext()
	{
		glfwInit();
		window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	}
	~GLTestContext()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

static GLTestContext glContext;

WFCMap* map = nullptr;

int mapSize = 10;

static TEST(base, CONSTRUCTION)
{
	map = new WFCMap(mapSize, mapSize, 12345, "config.yaml");
	EXPECT_TRUE(map != nullptr);
}

static TEST(base, INIT)
{
	if (map != nullptr)
	{
		map->init();
	}
	EXPECT_TRUE(map->initialized);
}

static TEST(base, GENERATION)
{
	if (map->initialized)
	{
		map->generate();
	}
	EXPECT_TRUE(map->isReady);
}

static TEST(base, DRAWING)
{
	ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", std::string("sprite"));
	glm::mat4 projection = glm::ortho(0.0f, 50.0f, 50.0f, 0.0f);
	ResourceManager::getShader("sprite").use().setInt("image", 0);
	ResourceManager::getShader("sprite").setMat4("projection", projection);
	Shader spriteSpader = ResourceManager::getShader("sprite");
	SpriteRenderer* renderer = new SpriteRenderer(spriteSpader);
	if (map->isReady)
	{
		map->draw(*renderer);
	}
}

//TEST_F(MapTest, construction)
//{
//	map = new WFCMap(50, 50, 12345, "config.yaml");
//	ASSERT_NE(map, nullptr);
//}
//
//TEST_F(MapTest, init)
//{
//	map->init();
//}
//
//TEST_F(MapTest, generation)
//{
//	map->generate();
//}
