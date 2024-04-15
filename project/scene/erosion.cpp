#include "erosion.h"

Erosion::Erosion() {}

void Erosion::Initialize(int mapSize, bool resetSeed) {
	if (resetSeed || currentSeed != seed) {
		prng.seed(seed);
		currentSeed = seed;
	}

	if (erosionBrushIndices.empty() || currentErosionRadius != erosionRadius || currentMapSize != mapSize) {
		InitializeBrushIndices(mapSize, erosionRadius);
		currentErosionRadius = erosionRadius;
		currentMapSize = mapSize;
	}
}

void Erosion::Erode(Array2D<float>* map, int mapSize, int numIterations, bool resetSeed) {
	Initialize(mapSize, resetSeed);

	for (int iteration = 0; iteration < numIterations; iteration++) {
		// Create water droplet at random point on map
		float posX = prng() % (mapSize - 1);
		float posY = prng() % (mapSize - 1);
		if (posX < 0 || posY < 0) {

		}
		float dirX = 0;
		float dirY = 0;
		float speed = initialSpeed;
		float water = initialWaterVolume;
		float sediment = 0;

		for (int lifetime = 0; lifetime < maxDropletLifetime; lifetime++) {
			int nodeX = (int)posX;
			int nodeY = (int)posY;
			int dropletIndex = nodeY * mapSize + nodeX;
			// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
			float cellOffsetX = posX - nodeX;
			float cellOffsetY = posY - nodeY;
			// Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
			HeightAndGradient heightAndGradient = CalculateHeightAndGradient(map, posX, posY);

			// Update the droplet's direction and position (move position 1 unit regardless of speed)
			dirX = (dirX * inertia - heightAndGradient.gradientX * (1 - inertia));
			dirY = (dirY * inertia - heightAndGradient.gradientY * (1 - inertia));
			// Normalize direction
			float len = std::max(0.01f, std::sqrt(dirX * dirX + dirY * dirY));
			if (len != 0) {
				dirX /= len;
				dirY /= len;
			}
			posX += dirX;
			posY += dirY;

			// Stop simulating droplet if it's not moving or has flowed over edge of map
			if ((dirX == 0 && dirY == 0) || posX < 0 || posX >= mapSize - 1 || posY < 0 || posY >= mapSize - 1) {
				break;
			}

			// Find the droplet's new height and calculate the deltaHeight
			float newHeight = CalculateHeightAndGradient(map, posX, posY).height;
			float deltaHeight = (newHeight - heightAndGradient.height)*10;

			// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
			float sedimentCapacity = std::max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);

			// If carrying more sediment than capacity, or if flowing uphill:
			if (sediment > sedimentCapacity || deltaHeight > 0) {
				// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
				float amountToDeposit = (deltaHeight > 0) ? std::min(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
				sediment -= amountToDeposit;

				// Add the sediment to the four nodes of the current cell using bilinear interpolation
				// Deposition is not distributed over a radius (like erosion) so that it can fill small pits
				map->At(nodeX, nodeY) += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);

				map->At(nodeX + 1, nodeY) += amountToDeposit * cellOffsetX * (1 - cellOffsetY);

				map->At(nodeX, nodeY + 1) += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;

				map->At(nodeX + 1, nodeY + 1) += amountToDeposit * cellOffsetX * cellOffsetY;
			}
			else {
				// Erode a fraction of the droplet's current carry capacity.
				// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
				float amountToErode = std::min((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);

				// Use erosion brush to erode from all nodes inside the droplet's erosion radius
				for (int brushPointIndex = 0; brushPointIndex < erosionBrushIndices[dropletIndex].size(); brushPointIndex++) {
					int nodeIndex = erosionBrushIndices[dropletIndex][brushPointIndex];
					float weighedErodeAmount = amountToErode * erosionBrushWeights[dropletIndex][brushPointIndex];
					float deltaSediment = (map->Get(nodeIndex) < weighedErodeAmount) ? map->Get(nodeIndex) : weighedErodeAmount;
					float value = map->Get(nodeIndex) - deltaSediment;
					map->Set(nodeIndex, value);
					sediment += deltaSediment;
				}
			}

			// Update droplet's speed and water content

			speed = std::max(0.0f, std::sqrt(speed * speed + deltaHeight * gravity));
			water *= (1 - evaporateSpeed);
		}
	}
}

HeightAndGradient Erosion::CalculateHeightAndGradient(Array2D<float>* nodes, float posX, float posY) {
	// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
	int coordX = static_cast<int>(posX);
	int coordY = static_cast<int>(posY);
	float x = posX - coordX;
	float y = posY - coordY;

	// Calculate heights of the four nodes of the droplet's cell
	float heightNW = nodes->At(coordX, coordY);
	float heightNE = nodes->At(coordX + 1, coordY);
	float heightSW = nodes->At(coordX, coordY + 1);
	float heightSE = nodes->At(coordX + 1, coordY + 1);

	// Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
	float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
	float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

	// Calculate height with bilinear interpolation of the heights of the nodes of the cell
	float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;

	return HeightAndGradient{ height, gradientX, gradientY };
}

void Erosion::InitializeBrushIndices(int mapSize, int radius) {
	erosionBrushIndices.resize(mapSize * mapSize);
	erosionBrushWeights.resize(mapSize * mapSize);

	for (int i = 0; i < erosionBrushIndices.size(); i++) {
		int centreX = i % mapSize;
		int centreY = i / mapSize;

		if (centreY <= radius || centreY >= mapSize - radius || centreX <= radius + 1 || centreX >= mapSize - radius) {
			std::vector<int> xOffsets;
			std::vector<int> yOffsets;
			std::vector<float> weights;
			float weightSum = 0;

			for (int y = -radius; y <= radius; y++) {
				for (int x = -radius; x <= radius; x++) {
					int sqrDst = x * x + y * y;
					if (sqrDst < radius * radius) {
						int coordX = centreX + x;
						int coordY = centreY + y;

						if (coordX >= 0 && coordX < mapSize && coordY >= 0 && coordY < mapSize) {
							float weight = 1 - std::sqrt(sqrDst) / radius;
							weightSum += weight;
							weights.push_back(weight);
							xOffsets.push_back(x);
							yOffsets.push_back(y);
						}
					}
				}
			}

			size_t numEntries = xOffsets.size();
			erosionBrushIndices[i].resize(numEntries);
			erosionBrushWeights[i].resize(numEntries);

			for (int j = 0; j < numEntries; j++) {
				erosionBrushIndices[i][j] = (yOffsets[j] + centreY) * mapSize + xOffsets[j] + centreX;
				erosionBrushWeights[i][j] = weights[j] / weightSum;
			}
		}
	}
}