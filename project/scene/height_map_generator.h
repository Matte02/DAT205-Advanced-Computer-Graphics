#pragma once
#include <glm/glm.hpp>
#include "array2d.h"
using namespace glm;

// TODO: LOOK OVER
struct NoiseSettings {
	float lacunarity = 2.0f;
	float gain = 0.5f;
	int octaves = 6;
	float sampleScale = 0.005;
	float minHeight = 0.0f;
	float maxHeight = 100.0f;
	float ridgeOffset = 1;
	vec3 sampleOffset = vec3(0);
	vec3 wrap = vec3(0);
};

enum Noise {
	DEFAULT,
	FBM,
	TURBULENCE,
	RIDGE
};

class HeightMapGenerator {

public:

	HeightMapGenerator() {
	};

	Array2D<float> GenerateHeightMap(int worldSize, Noise noise, bool normalizeMap);

	Array2D<float> GetHeightMap();

	NoiseSettings settings;

protected:

private:

	Array2D<float> m_heightMap;

};