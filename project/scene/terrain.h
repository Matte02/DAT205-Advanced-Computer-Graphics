#ifndef TERRAIN_H
#define TERRAIN_H

#include "array2d.h"
#include <Model.h>
#include <string>
#include "terrain_technique.h"
#include "texture.h"
#include "quad_list.h"
#include <generation_technique.h>
#include <normal_map_technique.h>

class BaseTerrain
{
public:
	BaseTerrain();

	~BaseTerrain();

	void Destroy();

	void InitTerrain(float WorldScale, int WorldSize, float TextureScale, int numPatches, NoiseSettings settings, TerrainTechnique* terrainTechnique);

	void UpdateTerrain(int numPatches, float WorldScale, float TextureScale);
	// Should only be used if the heights of the height map has changed, not when changing size or scale.
	void UpdateTerrain(NoiseSettings settings, int WorldSize);

	void Render(const mat4 viewMatrix, const mat4 projMatrix, const vec3& CameraPos, const vec3& lightDirection);

	float GetWorldScale() const { return m_worldScale; }

	float GetMaxHeight() const { return m_maxHeight; };

	void setTerrainHeights(const std::vector<float> heights);
	void setSlope(const float slope, const float slopeRange);

	float GetTextureScale() const { return m_textureScale; };

	int GetSize() const { return m_terrainSize; }



protected:
	void BaseTerrain::InitTextures();

	float m_maxHeight = 0;
	float m_minHeight = 0;
	int m_terrainSize = 0;
	int m_numPatches;
	float m_worldScale = 1.0f;

	float m_pTerrainHeghts[4] = { 0.1f, 0.3f, 0.6f, 0.9f };
	float m_slope = 1;
	float m_slopeRange = 0.5;
	labhelper::Texture m_pTextures[6] = { 0 };
	labhelper::Texture m_pTextureNormals[6] = { 0 };

	
	int m_numTextures = 6;
	Texture m_terrainTextures = Texture(GL_TEXTURE_2D_ARRAY);

	float m_textureScale = 1.0f;

	Texture m_heightMapTexture = Texture(GL_TEXTURE_2D);
	Texture m_normalMapTexture = Texture(GL_TEXTURE_2D);

	GenerationTechnique m_heightMapGen;
	NormalMapTechnique m_normalMapGen;

	TerrainTechnique* m_technique;
	QuadList m_quadList;
};

#endif