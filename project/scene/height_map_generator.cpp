#include "height_map_generator.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb-master/stb_perlin.h"
#include <stdexcept>

HeightMapGenerator::HeightMapGenerator() = default;

void HeightMapGenerator::GenerateHeightMap(Array2D<float>* heightMap, int worldSize, bool normalizeMap) const
{
	heightMap->InitArray2D(worldSize, worldSize, 0.0f);
	for (int z = 0; z < worldSize; z++) {
		for (int x = 0; x < worldSize; x++) {
			float height = 0;
			switch (settings.noiseType)
			{
			case Noise::DEFAULT:
				height = stb_perlin_noise3(
					((float)x + settings.sampleOffset.x) * settings.sampleScale,
					settings.sampleOffset.y * settings.sampleScale,
					((float)z + settings.sampleOffset.z) * settings.sampleScale,
					settings.wrap.x,
					settings.wrap.y,
					settings.wrap.z);
				break;
			case Noise::FBM:
				height = stb_perlin_fbm_noise3(
					((float)x + settings.sampleOffset.x) * settings.sampleScale,
					settings.sampleOffset.y * settings.sampleScale,
					((float)z + settings.sampleOffset.z) * settings.sampleScale,
					settings.lacunarity,
					settings.gain,
					settings.octaves,
					settings.wrap.x,
					settings.wrap.y,
					settings.wrap.z);
				break;
			case Noise::TURBULENCE:
				height = stb_perlin_turbulence_noise3(
					((float)x + settings.sampleOffset.x) * settings.sampleScale,
					settings.sampleOffset.y * settings.sampleScale,
					((float)z + settings.sampleOffset.z) * settings.sampleScale,
					settings.lacunarity,
					settings.gain,
					settings.octaves,
					settings.wrap.x,
					settings.wrap.y,
					settings.wrap.z);
				break;
			case Noise::RIDGE:
				height = stb_perlin_ridge_noise3(
					((float)x + settings.sampleOffset.x) * settings.sampleScale,
					settings.sampleOffset.y * settings.sampleScale,
					((float)z + settings.sampleOffset.z) * settings.sampleScale,
					settings.lacunarity,
					settings.gain,
					settings.ridgeOffset,
					settings.octaves,
					settings.wrap.x,
					settings.wrap.y,
					settings.wrap.z);
				break;
			default:
				throw std::invalid_argument("Unsupported Noise Enum selected in GenerateHeightMap function.");
				break;
			}

			heightMap->Set(x, z, height);
		}
	}
	if (normalizeMap) {
		heightMap->Normalize(0.0f, 100.0f);
	}
}
