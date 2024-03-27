#pragma once

#include "terrain.h"

class PerlinNoiseTerrain : public BaseTerrain {

public:

	PerlinNoiseTerrain() {
	};

	void GenerateHeightMap(float lacunarity, float gain, int octaves, float offset, float sampleScale, float maxHeight, int wrap);

protected:

private:


};