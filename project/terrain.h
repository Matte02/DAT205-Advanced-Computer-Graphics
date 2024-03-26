#ifndef TERRAIN_H
#define TERRAIN_H

#include "triangle_list.h"


class BaseTerrain
{
public:
	BaseTerrain() {}

	void InitTerrain(float WorldScale);

	void Render(const mat4 viewProjMatrix, GLuint currentShaderProgram);


	float GetHeight(int x, int z) const { return 0.0f; }

	float GetWorldScale() const { return m_worldScale; }

protected:

	int m_terrainSize = 0;
	float m_worldScale = 1.0f;
	// TODO: Array2D<float> m_heightMap;
	TriangleList m_triangleList;
};

#endif