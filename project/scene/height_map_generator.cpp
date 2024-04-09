#include "height_map_generator.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb-master/stb_perlin.h"
#include <stdexcept>

Array2D<float> HeightMapGenerator::GenerateHeightMap(int worldSize, Noise noise, bool normalizeMap)
{
    m_heightMap.InitArray2D(worldSize, worldSize, 0.0f);
    for (int z = 0; z < worldSize; z++) {
        for (int x = 0; x < worldSize; x++) {
            float height = 0;
            switch (noise)
            {
            case DEFAULT:
                height = stb_perlin_noise3(
                    static_cast<float>(x + settings.sampleOffset.x) * settings.sampleScale,
                    static_cast<float>(settings.sampleOffset.y) * settings.sampleScale,
                    static_cast<float>(z + settings.sampleOffset.z),
                    settings.wrap.x,
                    settings.wrap.y,
                    settings.wrap.z);
                break;
            case FBM:
                height = stb_perlin_fbm_noise3(
                    static_cast<float>(x + settings.sampleOffset.x) * settings.sampleScale,
                    static_cast<float>(settings.sampleOffset.y) * settings.sampleScale,
                    static_cast<float>(z + settings.sampleOffset.z),
                    settings.lacunarity,
                    settings.gain,
                    settings.octaves,
                    settings.wrap.x,
                    settings.wrap.y,
                    settings.wrap.z);
                break;
            case TURBULENCE:
                height = stb_perlin_turbulence_noise3(
                    static_cast<float>(x + settings.sampleOffset.x) * settings.sampleScale,
                    static_cast<float>(settings.sampleOffset.y) * settings.sampleScale,
                    static_cast<float>(z + settings.sampleOffset.z),
                    settings.lacunarity,
                    settings.gain,
                    settings.octaves,
                    settings.wrap.x,
                    settings.wrap.y,
                    settings.wrap.z);
                break;
            case RIDGE:
                height = stb_perlin_ridge_noise3(
                    static_cast<float>(x + settings.sampleOffset.x) * settings.sampleScale,
                    static_cast<float>(settings.sampleOffset.y) * settings.sampleScale,
                    static_cast<float>(z + settings.sampleOffset.z),
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

            m_heightMap.Set(x, z, height);
        }
    }
    if (normalizeMap) {
        m_heightMap.Normalize(settings.minHeight, settings.maxHeight);
    }
    return m_heightMap;
}
