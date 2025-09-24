#include "pch.h"
#include <benchmark/benchmark.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../2dgame/WFCMap.h"
#include "../2dgame/ResourceManager.h"
#include "../2dgame/Globals.h"
#include <chrono>

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

static void BM_CONSTRUCTION(benchmark::State& state)
{
	for (auto _ : state)
	{
		WFCMap* map = new WFCMap(10, 10, 12345);
		delete map;
	}
}

static void BM_INITIALIZATION(benchmark::State& state)
{
	WFCMap* map = new WFCMap(10, 10, 12345);
	for (auto _ : state)
	{
		map->init();
	}
	delete map;
}

static void BM_GENERATION(benchmark::State& state)
{
	for (auto _ : state)
	{
		WFCMap* map = new WFCMap(10, 10, 12345);
		map->init();
		state.ResumeTiming();
		map->generate();
		state.PauseTiming();
		delete map;
	}
}

BENCHMARK(BM_CONSTRUCTION);
BENCHMARK(BM_INITIALIZATION);
BENCHMARK(BM_GENERATION);



int main(int argc, char** argv)
{
	(void)config;

	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream timestamp;
	timestamp << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");

	char* outDir = std::getenv("BENCHMARK_OUT_DIR");
	std::string path =
		std::string(outDir) + "benchmark_results_" + timestamp.str() + ".json";

	std::string arg1 = "--benchmark_out=" + path;
	
	argv[1] = const_cast<char*>(arg1.c_str());
	argv[2] = "--benchmark_out_format=json";

	argc = 3;

	benchmark::Initialize(&argc, argv);
	benchmark::RunSpecifiedBenchmarks();
	benchmark::Shutdown();

	return 0;
}

