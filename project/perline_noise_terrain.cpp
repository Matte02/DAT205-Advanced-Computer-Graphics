#include "perline_noise_terrain.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb-master/stb_perlin.h"
#include <limits>


void PerlinNoiseTerrain::GenerateHeightMap(float lacunarity, float gain, int octaves, float offset, float sampleScale, float maxHeight, int wrap) {
	for (int z = 0; z < m_terrainSize; z++) {
		for (int x = 0; x < m_terrainSize; x++) {
			float height = stb_perlin_ridge_noise3((float)x* sampleScale, 0, float(z)* sampleScale, lacunarity, gain, offset, octaves, wrap, 0, wrap);
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
	m_maxHeight = maxHeight;
	m_minHeight = 0;
	m_heightMap.Normalize(0, m_maxHeight);
	m_triangleList.CreateTriangleList(m_terrainSize, m_terrainSize, this);
}