#ifndef NOISE_SETTINGS_H
#define NOISE_SETTINGS_H

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include<ctime>
#include <random>

struct NoiseSettings {
	float lacunarity{ 1.4f };
	float persistence{ 0.71f };
	int octaves{ 16 };
	float noiseScale{ 2.0f };
	float exponent{ 1.0f };
	float minHeight{ 0.0f };
	float maxHeight{ 250.0f};
	glm::vec2 offsets[16];
	int seed{ 0 };

	NoiseSettings() {
		generateRandomOffsets();
	};

	void RandomizeSeed() {
		seed = rand();
	}

	// Function to generate random offsets based on the seed
	void generateRandomOffsets() {
		std::mt19937 rng(seed); // Initialize random number generator with seed
		std::uniform_real_distribution<float> dist(-1000.0f, 1000.0f); // Adjust range as needed

		// Generate random offsets using the random number generator
		for (int i = 0; i < 16; ++i) {
			offsets[i] = glm::vec2(dist(rng), dist(rng));
		}
	}
};

#endif // NOISE_SETTINGS_H