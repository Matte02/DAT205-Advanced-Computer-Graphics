#include "perline_noise_terrain.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb-master/stb_perlin.h"
#include <limits>


void PerlinNoiseTerrain::GenerateHeightMap(float lacunarity, float gain, int octaves, float offset, float sampleScale, float heightScale) {
	m_maxHeight = -std::numeric_limits<float>::max();
	m_minHeight = std::numeric_limits<float>::max();

	for (int z = 0; z < m_terrainSize; z++) {
		for (int x = 0; x < m_terrainSize; x++) {
			float height = stb_perlin_ridge_noise3((float)x* sampleScale, 0, float(z)* sampleScale, lacunarity, gain, offset, octaves, 0, 0, 0) *heightScale;
			if (height > m_maxHeight) {
				m_maxHeight = height;
			}
			if (height < m_minHeight) {
				m_minHeight = height;
			}
			//printf("Height (%i, %i): %d \n", x, z, height);
			m_heightMap.Set(x, z, height);
		}
	}
	m_triangleList.CreateTriangleList(m_terrainSize, m_terrainSize, this);
}