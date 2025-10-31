#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "ResourceManager.h"
#include <glm/glm.hpp>
#include "TiledMap.h"

int SCREEN_WIDTH = 480;
int SCREEN_HEIGHT = 270;

void framebufferSize(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

Game* gameInst = nullptr;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2D Game", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSize);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
	gameInst = &game;

	game.init();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glClear(GL_COLOR_BUFFER_BIT);
		gameInst->processInput(deltaTime);
		gameInst->tick();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ResourceManager::clear();
	glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (gameInst != nullptr && key >= 0 && key < 512)
	{
		if (action == GLFW_PRESS)
		{
			gameInst->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			gameInst->keys[key] = false;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (gameInst != nullptr && button >= 0 && button < 16)
	{
		if (action == GLFW_PRESS)
		{
			gameInst->buttons[button] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			gameInst->buttons[button] = false;
		}
	}
}

void framebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}