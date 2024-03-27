#ifndef TERRAIN_H
#define TERRAIN_H

#include "triangle_list.h"
#include "array2d.h"


class BaseTerrain
{
public:
	BaseTerrain() {}

	~BaseTerrain();

	void Destroy();

	void InitTerrain(float WorldScale, int WorldSize);

	void GenerateHeightMap();

	void Render(const mat4 viewProjMatrix, GLuint currentShaderProgram);

	// Returns 
	float GetHeight(int x, int z) const { return m_heightMap.Get(x, z); }

	float GetWorldScale() const { return m_worldScale; }

	float GetMaxHeight() const { return m_maxHeight; };

	float GetMinHeight() const { return m_minHeight; };

protected:
	float m_maxHeight = 0;
	float m_minHeight = 0;
	int m_terrainSize = 0;
	float m_worldScale = 1.0f;
	Array2D<float> m_heightMap;
	TriangleList m_triangleList;
};

#endif