#include "perline_noise_terrain.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb-master/stb_perlin.h"
#include <limits>


void PerlinNoiseTerrain::GenerateHeightMap(float lacunarity, float gain, int octaves, float offset, float sampleScale, float maxHeight, int wrap) {
	for (int z = 0; z < m_terrainSize; z++) {
		for (int x = 0; x < m_terrainSize; x++) {
			float height = stb_perlin_turbulence_noise3((float)(x+offset)* sampleScale, 0, (float)(z+offset)* sampleScale, lacunarity, gain, octaves, wrap, wrap, wrap);
			m_heightMap.Set(x, z, height);
		}
	}
	m_maxHeight = maxHeight;
	m_heightMap.Normalize(0, maxHeight);
	m_triangleList.CreateTriangleList(m_terrainSize, m_terrainSize, this);
}