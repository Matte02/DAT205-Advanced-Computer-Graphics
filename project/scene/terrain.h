#ifndef TERRAIN_H
#define TERRAIN_H

#include "geomip_grid.h"
#include "array2d.h"
#include <Model.h>
#include <string>

class BaseTerrain
{
public:
	BaseTerrain() {}

	~BaseTerrain();

	void Destroy();

	void InitTerrain(float WorldScale, int WorldSize, float TextureScale, int PatchSize, const std::vector<std::string>& TextureFilenames, Array2D<float>* heightMap);

	void Render(const mat4 viewMatrix, const mat4 projMatrix, GLuint currentShaderProgram, const vec3& CameraPos);

	// Returns 
	float GetHeight(int x, int z) const { return m_heightMap->Get(x, z); }

	float GetWorldScale() const { return m_worldScale; }

	float GetMaxHeight() const { return m_maxHeight; };

	void setTerrainHeights(const std::vector<float> heights);
	void setSlope(const float slope, const float slopeRange);

	float GetTextureScale() const { return m_textureScale; };

	int GetSize() const { return m_terrainSize; }



protected:
	void BaseTerrain::InitTextures(const std::vector<std::string>& TextureFilenames, labhelper::Texture TextureArray[]);

	float m_maxHeight = 0;
	float m_minHeight = 0;
	int m_terrainSize = 0;
	int m_patchSize;
	float m_worldScale = 1.0f;
	Array2D<float>* m_heightMap;
	GeomipGrid m_geomipGrid;

	float m_pTerrainHeghts[4] = { 0.1f, 0.3f, 0.6f, 0.9f };
	float m_slope = 1;
	float m_slopeRange = 0.5;
	labhelper::Texture m_pTextures[6] = { 0 };
	labhelper::Texture m_pTextureNormals[6] = { 0 };

	float m_textureScale = 1.0f;
};

#endif