#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "ResourceManager.h"

#include "Level.h"

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

void framebufferSize(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

Game game(SCREEN_WIDTH, SCREEN_HEIGHT);

Level level(30, 30);

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
	glfwSetKeyCallback(window, keyCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glClearColor(0.52f, 0.155f, 0.235f, 0.0f);
	level.init();
	level.collapse(3, 5);
	game.init();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glClear(GL_COLOR_BUFFER_BIT);
		game.render();
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}
	ResourceManager::clear();
	glfwTerminate();
}

void framebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}
