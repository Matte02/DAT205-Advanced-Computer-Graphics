#include "height_map_generator.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb-master/stb_perlin.h"
#include <stdexcept>
#include <chrono>
#include <iostream>

HeightMapGenerator::HeightMapGenerator() = default;

void HeightMapGenerator::GenerateHeightMap(Array2D<float>* heightMap, int worldSize, bool normalizeMap) const
{
#if TIMING_ENABLED
    auto start = std::chrono::high_resolution_clock::now();
#endif
	heightMap->InitArray2D(worldSize, worldSize, 0.0f);
#pragma omp parallel for collapse(2) // Parallelize the nested loops
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
		heightMap->Normalize(settings.minHeight, settings.maxHeight);
    }
#if TIMING_ENABLED
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken for generating height map in Height Map Generator: " << duration.count() << " seconds" << std::endl;
#endif
}