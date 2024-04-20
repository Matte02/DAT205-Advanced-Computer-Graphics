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

void BaseTerrain::InitTerrain(float WorldScale, int WorldSize, float TextureScale, int numPatches, const std::vector<std::string>& TextureFilenames, NoiseSettings settings)
{
    if (!m_technique.Init()) {
        printf("Error initializing tech\n");
        exit(0);
    }
    if (!m_heightMapGen.Init()) {
        printf("Error initializing generation tech\n");
        exit(0);
    }

    m_numPatches = numPatches;
    m_worldScale = WorldScale;
    m_textureScale = TextureScale;
    m_terrainSize = WorldSize;

    std::vector<std::string> textureNames(TextureFilenames.begin(), TextureFilenames.begin() + TextureFilenames.size() / 2);
    std::vector<std::string> TextureNormalNames(TextureFilenames.begin() + TextureFilenames.size() / 2, TextureFilenames.end());
    InitTextures(textureNames, m_pTextures);
    InitTextures(textureNames, m_pTextureNormals);


    m_quadList.CreateQuadList(numPatches, numPatches, this);
    m_heightMapTexture.CreateEmpty32FTexture(m_terrainSize , m_terrainSize);

    m_heightMapTexture.BindImage(HEIGHT_MAP_TEXTURE_UNIT, GL_READ_ONLY);
    m_heightMapGen.GenerateHeightMap(m_terrainSize, m_terrainSize, settings);
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
    UpdateTerrain(settings);
}

void BaseTerrain::UpdateTerrain(NoiseSettings settings)
{
    m_heightMapTexture.BindImage(HEIGHT_MAP_TEXTURE_UNIT, GL_READ_ONLY);
    m_heightMapGen.GenerateHeightMap(m_terrainSize, m_terrainSize, settings);
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

void BaseTerrain::InitTextures(const std::vector<std::string>& TextureFilenames, labhelper::Texture TextureArray[]) {
    if (TextureFilenames.size() != 6) {
        printf("%s:%d - number of provided textures (%llu) is not equal to the size of the texture array (%i)\n",
            __FILE__, __LINE__, TextureFilenames.size(), 6);
        exit(0);
    }
    int i = 0;
    for (std::string filename : TextureFilenames) {
        labhelper::Texture tex;
        if (!TextureArray[i++].load("../scenes/Textures/", filename, 4)) {
            printf("%s:%d - Failed to load texture (%s) \n", __FILE__, __LINE__, filename.c_str());
            exit(0);
        }
    }
}

BaseTerrain::~BaseTerrain()
{
    Destroy();
}

void BaseTerrain::Destroy()
{
    m_quadList.Destroy();

    for (int i = 0; i < 6; i++) {
        if (m_pTextures[i].valid && m_pTextures[i].gl_id != 0) {
            glDeleteTextures(1, &m_pTextures[i].gl_id);
            m_pTextures[i].gl_id = 0; 
            glDeleteTextures(1, &m_pTextureNormals[i].gl_id);
            m_pTextureNormals[i].gl_id = 0;
        }
    }

}

void BaseTerrain::Render(const mat4 viewMatrix, const mat4 projectionMatrix, const vec3& CameraPos, const vec3& lightDirection, const int viewMode)
{

    m_technique.Enable();
    m_technique.SetViewMatrix(viewMatrix);
    m_technique.SetViewProjectionMatrix(projectionMatrix * viewMatrix);
    m_technique.SetMaxHeight(m_maxHeight);
    m_technique.SetViewMode(viewMode);
    //m_technique.SetLightDir(lightDirection);

    m_heightMapTexture.Bind(HEIGHT_MAP_TEXTURE_UNIT);

    m_quadList.Render();
}

