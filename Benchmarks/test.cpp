#include "pch.h"
#include <benchmark/benchmark.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../2dgame/WFCMap.h"
#include "../2dgame/ResourceManager.h"
#include "../2dgame/Globals.h"
#include <chrono>
#include "../2dgame/SpriteRenderer.h"

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

static void BM_CONSTRUCTION(benchmark::State& state)
{
	for (auto _ : state)
	{
		WFCMap* map = new WFCMap(10, 10, 12345, *render.renderer);
		delete map;
	}
}

static void BM_INITIALIZATION(benchmark::State& state)
{
	int mapSize = static_cast<int>(state.range(0));
	for (auto _ : state)
	{
		state.PauseTiming();
		WFCMap* map = new WFCMap(mapSize, mapSize, 12345, *render.renderer);
		state.ResumeTiming();
		map->init();
		state.PauseTiming();
		delete map;
	}
}

static void BM_GENERATION(benchmark::State& state)
{
	int mapSize = static_cast<int>(state.range(0));
	config.n = static_cast<int>(state.range(1));

	for (auto _ : state)
	{
		state.PauseTiming();
		WFCMap* map = new WFCMap(mapSize, mapSize, 12345, *render.renderer);
		map->init();
		state.ResumeTiming();
		map->generate();
		state.PauseTiming();
		delete map;
	}
	state.SetComplexityN(mapSize * mapSize);
	state.counters["PatternSize"] = config.n;
	state.counters["MapSize"] = mapSize;
	state.counters["TotalCells"] = mapSize * mapSize;
}

BENCHMARK(BM_CONSTRUCTION);
BENCHMARK(BM_INITIALIZATION)
->RangeMultiplier(2)
->Range(8, 64)
->Complexity(benchmark::oAuto);
BENCHMARK(BM_GENERATION)
->RangeMultiplier(2)
->Ranges({{8, 64}, {2, 5}})
->Complexity(benchmark::oAuto);



int main(int argc, char** argv)
{
	(void)config;

	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream timestamp;
	timestamp << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");

	char* outDir = std::getenv("BENCHMARK_OUT_DIR");
	std::string path =
		std::string(outDir) + "benchmark_results_" + timestamp.str() + ".csv";

	std::string arg1 = "--benchmark_out=" + path;

	argv[1] = const_cast<char*>(arg1.c_str());
	argv[2] = "--benchmark_out_format=csv";

	argc = 3;

	benchmark::Initialize(&argc, argv);
	benchmark::RunSpecifiedBenchmarks();
	benchmark::Shutdown();

	return 0;
}

