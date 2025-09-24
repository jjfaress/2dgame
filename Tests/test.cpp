#include "pch.h"
#include "../2dgame/WFCMap.h"
#include "../2dgame/Globals.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

TEST(BASE, CONSTRUCT)
{
	WFCMap* map = new WFCMap(10, 10, 12345);
	EXPECT_TRUE(map != nullptr);
	delete map;
}

TEST(BASE, INIT)
{
	WFCMap* map = new WFCMap(10, 10, 12345);
	map->init();
	EXPECT_TRUE(map->initialized);
	delete map;
}

TEST(BASE, GENERATE)
{
	WFCMap* map = new WFCMap(10, 10, 12345);
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