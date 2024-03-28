#ifndef TERRAIN_H
#define TERRAIN_H

#include "triangle_list.h"
#include "array2d.h"
#include <Model.h>
#include <string>

class BaseTerrain
{
public:
	BaseTerrain() {}

	~BaseTerrain();

	void Destroy();

	void InitTerrain(float WorldScale, int WorldSize, float TextureScale, const std::vector<std::string>& TextureFilenames);

	void GenerateHeightMap();

	void Render(const mat4 viewMatrix, const mat4 projMatrix, GLuint currentShaderProgram);

	// Returns 
	float GetHeight(int x, int z) const { return m_heightMap.Get(x, z); }

	float GetWorldScale() const { return m_worldScale; }

	float GetMaxHeight() const { return m_maxHeight; };

	void setTerrainHeights(const std::vector<float> heights);
	void setSlope(const float slope, const float slopeRange);

	float GetTextureScale() const { return m_textureScale; };

	int GetSize() const { return m_terrainSize; }

protected:

	void InitTextures(const std::vector<std::string>& TextureFilenames);

	float m_maxHeight = 0;
	int m_terrainSize = 0;
	float m_worldScale = 1.0f;
	Array2D<float> m_heightMap;
	TriangleList m_triangleList;

	float m_pTerrainHeghts[4] = { 0.1, 0.3, 0.6, 0.9 };
	float m_slope = 1;
	float m_slopeRange = 0.5;
	labhelper::Texture m_pTextures[6] = { 0 };

	float m_textureScale = 1.0f;
};

#endif