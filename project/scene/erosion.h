#ifndef EROSION_H
#define EROSION_H

#include <vector>
#include <cmath>
#include <random>
#include <array2d.h>

struct HeightAndGradient {
    float height;
    float gradientX;
    float gradientY;
};

class Erosion {
public:
    int seed;
    int erosionRadius = 3;
    float inertia = 0.05f;
    float sedimentCapacityFactor = 4;
    float minSedimentCapacity = 0.01f;
    float erodeSpeed = 0.3f;
    float depositSpeed = 0.3f;
    float evaporateSpeed = 0.01f;
    float gravity = 4;
    int maxDropletLifetime = 30;
    float initialWaterVolume = 10;
    float initialSpeed = 1;

    Erosion();
    void Initialize(int mapSize, bool resetSeed);
    void Erode(Array2D<float>* map, int mapSize, int numIterations = 1, bool resetSeed = false);
    HeightAndGradient CalculateHeightAndGradient(Array2D<float>* nodes, float posX, float posY);

private:
    std::vector<std::vector<int>> erosionBrushIndices;
    std::vector<std::vector<float>> erosionBrushWeights;
    std::mt19937 prng;
    int currentSeed;
    int currentErosionRadius;
    int currentMapSize;

    void InitializeBrushIndices(int mapSize, int radius);
};

#endif // EROSION_H