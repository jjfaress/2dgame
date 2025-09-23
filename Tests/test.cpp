#include "pch.h"
#include <benchmark/benchmark.h>
//#include "../2dgame/Globals.h"
#include "../2dgame/WFCMap.h"

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
	//EXPECT_TRUE(map->initialized);
	map->generate();
	EXPECT_TRUE(map->isReady);
	delete map;
}

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
	WFCMap* map = new WFCMap(10, 10, 12345);
	map->init();
	for (auto _ : state)
	{
		map->generate();
	}
	delete map;
}

BENCHMARK(BM_CONSTRUCTION);
BENCHMARK(BM_INITIALIZATION);
BENCHMARK(BM_GENERATION);

BENCHMARK_MAIN();