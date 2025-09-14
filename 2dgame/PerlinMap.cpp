#include "PerlinMap.h"
#include <noise/noise.h>
#include "noiseutils.h"

PerlinMap::PerlinMap(int width, int height) :
	WIDTH(width),
	HEIGHT(height)
{
}

void PerlinMap::generate()
{
	noise::module::Perlin module;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane builder;
	builder.SetSourceModule(module);
	builder.SetDestNoiseMap(heightMap);
	builder.SetDestSize(this->WIDTH, this->HEIGHT);
	builder.SetBounds(0.0, 1.0, 0.0, 1.0);
	builder.Build();


}