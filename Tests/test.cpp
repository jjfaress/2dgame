#include "pch.h"
#include "../2dgame/WFCMap.h"
#include "../2dgame/Globals.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../2dgame/ResourceManager.h"

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

struct Render {
	SpriteRenderer* renderer;
	Render()
	{
		ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", "sprite");
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(800), static_cast<float>(600), 0.0f);
		ResourceManager::getShader("sprite").use().setInt("image", 0);
		ResourceManager::getShader("sprite").setMat4("projection", projection);
		Shader spriteShader = ResourceManager::getShader("sprite");
		renderer = new SpriteRenderer(spriteShader);
	}
	~Render()
	{
		delete renderer;
	}
};

Render render;

TEST(BASE, CONSTRUCT)
{
	WFCMap* map = new WFCMap(10, 10, 12345, *render.renderer);
	EXPECT_TRUE(map != nullptr);
	delete map;
}

TEST(BASE, INIT)
{
	WFCMap* map = new WFCMap(10, 10, 12345, *render.renderer);
	map->init();
	EXPECT_TRUE(map->initialized);
	delete map;
}

TEST(BASE, GENERATE)
{
	WFCMap* map = new WFCMap(10, 10, 12345, *render.renderer);
	map->init();
	EXPECT_TRUE(map->initialized);
	map->generate();
	EXPECT_TRUE(map->isReady);
	delete map;
}

int main(int argc, char** argv)
{
	(void)config;

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}