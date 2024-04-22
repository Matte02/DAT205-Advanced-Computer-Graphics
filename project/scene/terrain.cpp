#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <string.h>

#include "terrain.h"
#include "labhelper.h"
#include <Model.h>
#include "texture_config.h"

BaseTerrain::BaseTerrain() {
}

void BaseTerrain::InitTerrain(float WorldScale, int WorldSize, float TextureScale, int numPatches, NoiseSettings settings, TerrainTechnique* terrainTechnique )
{

	m_technique = terrainTechnique;
	if (!m_technique->Init()) {
		printf("Error initializing tech\n");
		exit(0);
	}

	if (!m_heightMapGen.Init()) {
		printf("Error initializing generation tech\n");
		exit(0);
	}
	if (!m_normalMapGen.Init()) {
		printf("Error initializing normalmap tech\n");
		exit(0);
	}
	m_numPatches = numPatches;
	m_worldScale = WorldScale;
	m_textureScale = TextureScale;
	m_terrainSize = WorldSize;

	InitTextures();
	m_quadList.CreateQuadList(numPatches, numPatches, this);

	m_heightMapTexture.CreateEmptyTexture(m_terrainSize, m_terrainSize, GL_R32F);
	m_normalMapTexture.CreateEmptyTexture(m_terrainSize, m_terrainSize, GL_RGBA32F);

	m_heightMapTexture.BindImage(HEIGHT_MAP_TEXTURE_UNIT, GL_READ_ONLY);
	m_heightMapGen.GenerateHeightMap(m_terrainSize, m_terrainSize, settings);

	m_normalMapTexture.BindImage(NORMAL_MAP_TEXTURE_UNIT, GL_READ_ONLY);
	m_normalMapGen.GenerateNormalMap(m_terrainSize, m_terrainSize);
	m_maxHeight = settings.maxHeight;
	m_minHeight = settings.minHeight;

}

void BaseTerrain::UpdateTerrain(int numPatches, float WorldScale, float TextureScale) {
	// Clean up
	m_quadList.~QuadList();

	// Set new values
	m_numPatches = numPatches;
	m_worldScale = WorldScale;
	m_textureScale = TextureScale;

	// Recreate QuadList
	m_quadList.CreateQuadList(numPatches, numPatches, this);
}

void BaseTerrain::UpdateTerrain(NoiseSettings settings, int WorldSize)
{
	m_terrainSize = WorldSize;
	m_heightMapTexture.BindImage(HEIGHT_MAP_TEXTURE_UNIT, GL_READ_ONLY);
	m_heightMapGen.GenerateHeightMap(m_terrainSize, m_terrainSize, settings);

	m_normalMapTexture.BindImage(NORMAL_MAP_TEXTURE_UNIT, GL_READ_ONLY);
	m_normalMapGen.GenerateNormalMap(m_terrainSize, m_terrainSize);
	m_maxHeight = settings.maxHeight;
	m_minHeight = settings.minHeight;
}


void BaseTerrain::setTerrainHeights(const std::vector<float> heights)
{
	for (int i = 0; i < 4; i++) {
		m_pTerrainHeghts[i] = heights.at(i);
	}
}

void BaseTerrain::setSlope(const float slope, const float slopeRange)
{
	m_slope = slope;
	m_slopeRange = slopeRange;
}

void BaseTerrain::InitTextures() {
	std::vector<std::string> textFilenames = {
	"../scenes/Textures/desert_sand_d.jpg",
	"../scenes/Textures/grass_green_d.jpg",
	"../scenes/Textures/mntn_dark_d.jpg",
	"../scenes/Textures/snow1_d.jpg",
	"../scenes/Textures/mntn_brown_d.jpg",
	"../scenes/Textures/snow_mntn2_d.jpg" };

	m_terrainTextures.Load(textFilenames, 4);
}

BaseTerrain::~BaseTerrain()
{
	Destroy();
}

void BaseTerrain::Destroy()
{
	m_quadList.Destroy();
	m_normalMapTexture.~Texture();
	m_heightMapTexture.~Texture();
	m_terrainTextures.~Texture();

	for (int i = 0; i < 6; i++) {
		if (m_pTextures[i].valid && m_pTextures[i].gl_id != 0) {
			glDeleteTextures(1, &m_pTextures[i].gl_id);
			m_pTextures[i].gl_id = 0;
			glDeleteTextures(1, &m_pTextureNormals[i].gl_id);
			m_pTextureNormals[i].gl_id = 0;
		}
	}

}

void BaseTerrain::Render(const mat4 viewMatrix, const mat4 projectionMatrix, const vec3& CameraPos, const vec3& lightDirection)
{

	m_technique->Enable();
	m_technique->SetViewMatrix(viewMatrix);
	m_technique->SetViewProjectionMatrix(projectionMatrix * viewMatrix);

	m_heightMapTexture.Bind(HEIGHT_MAP_TEXTURE_UNIT);
	m_normalMapTexture.Bind(NORMAL_MAP_TEXTURE_UNIT);
	m_terrainTextures.Bind(TERRAIN_TEXTURES_UNIT);

	m_quadList.Render();
}

