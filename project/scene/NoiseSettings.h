#ifndef NOISE_SETTINGS_H
#define NOISE_SETTINGS_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include<ctime>

struct NoiseSettings {
	float lacunarity{ 1.4f };
	float persistence{ 0.7f };
	int octaves{ 16 };
	float noiseScale{ 5.0f };
	float exponent{ 1.2f };
	float minHeight{ 0.0f };
	float maxHeight{ 150.0f};
	glm::vec2 offsets[16];
	int seed{ 0 };

	NoiseSettings() {
		srand(seed);
		generateRandomOffsets();
	};

	void RandomizeSeed() {
		seed = static_cast<int>(std::time(nullptr)); // Use current time as seed
		seed = rand();
		srand(seed);
	}

	// Function to generate random offsets based on the seed
	void generateRandomOffsets() {
		
		for (int i = 0; i < 16; ++i) {
			offsets[i] = glm::vec2{ glm::linearRand(-10000.0f, 10000.0f), -glm::linearRand(-10000.0f, 10000.0f) };
		}
	}
};

#endif // NOISE_SETTINGS_H