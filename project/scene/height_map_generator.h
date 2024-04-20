#pragma once
#include <glm/glm.hpp>
#include "array2d.h"
using namespace glm;

enum Noise {
	DEFAULT,
	FBM,
	TURBULENCE,
	RIDGE
};

struct NoiseSettings123 {
	Noise noiseType = FBM;
	float lacunarity = 2.0f;
	float gain = 0.5f;
	int octaves = 6;
	float sampleScale = 0.005f;
	float minHeight = 0.0f;
	float maxHeight = 250.0f;
	float ridgeOffset = 1;
	vec3 sampleOffset{ 0 };
	ivec3 wrap{ 0 };

	NoiseSettings123() = default;
};



class HeightMapGenerator {

public:

	HeightMapGenerator();

	void GenerateHeightMap(Array2D<float>* heightMap, int worldSize, bool normalizeMap) const;


	// Getter for NoiseSettings
	NoiseSettings123& getNoiseSettings() {
		return settings;
	}

private:
	NoiseSettings123 settings;

};