#include "pch.h"
#include "../2dgame/WFCMap.h"
#include "../2dgame/ConfigLoader.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../2dgame/SpriteRenderer.h"
#include "../2dgame/ResourceManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct GLTestContext {
    GLFWwindow* window = nullptr;
    GLTestContext() {
        glfwInit();
        window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }
    ~GLTestContext() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

static GLTestContext glContext;

class MapTest : public ::testing::Test {
protected:
	WFCMap* map;
	SpriteRenderer* renderer = nullptr;
	void SetUp() override
	{
		try
		{
			ResourceManager::loadShader("spriteShader.vert", "spriteShader.frag", std::string("sprite"));
			glm::mat4 projection = glm::ortho(0.0f, 50.0f, 50.0f, 0.0f);
			ResourceManager::getShader("sprite").use().setInt("image", 0);
			ResourceManager::getShader("sprite").setMat4("projection", projection);
			renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));
			map = new WFCMap(50, 50, 12345, "config.yaml");
			ASSERT_NE(map, nullptr) << "Failed to create WFCMap object";
		}
		catch (const std::exception& e)
		{
			FAIL() << "Exception in WFCMap constructor: " << e.what();
		}
	}
	void TearDown() override
	{
		if (map != nullptr)
		{
			delete map;
			map = nullptr;
		}
	}
};


TEST_F(MapTest, 50X50)
{
	map = new WFCMap(50, 50, 12345, "config.yaml");
	map->init();
	map->generate();
}
