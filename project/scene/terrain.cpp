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

void BaseTerrain::InitTerrain(float WorldScale, int WorldSize, float TextureScale, int PatchSizePower, const std::vector<std::string>& TextureFilenames, Array2D<float>* heightMap)
{


    m_patchSize = pow(2, PatchSizePower) + 1;

    int i = 0;
    while (true) {
        int RecommendedWorldSize = ((WorldSize - i - 1 + m_patchSize - 1) / (m_patchSize - 1)) * (m_patchSize - 1) + 1;
        i++;
        if (RecommendedWorldSize <= WorldSize) {

            printf("Setting World Size to: %d\n", RecommendedWorldSize);
            m_terrainSize = RecommendedWorldSize;
            break;
        }
    }
    m_worldScale = WorldScale;
    m_textureScale = TextureScale;
    heightMap->GetMinMax(m_minHeight, m_maxHeight);
    
    m_heightMap = heightMap;

    std::vector<std::string> textureNames(TextureFilenames.begin(), TextureFilenames.begin() + TextureFilenames.size() / 2);
    std::vector<std::string> TextureNormalNames(TextureFilenames.begin() + TextureFilenames.size() / 2, TextureFilenames.end());
    InitTextures(textureNames, m_pTextures);
    InitTextures(textureNames, m_pTextureNormals);

    m_geomipGrid.CreateGeomipGrid(m_terrainSize, m_terrainSize, m_patchSize, this);
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
    m_geomipGrid.Destroy();

    for (int i = 0; i < 6; i++) {
        if (m_pTextures[i].valid && m_pTextures[i].gl_id != 0) {
            glDeleteTextures(1, &m_pTextures[i].gl_id);
            m_pTextures[i].gl_id = 0; 
            glDeleteTextures(1, &m_pTextureNormals[i].gl_id);
            m_pTextureNormals[i].gl_id = 0;
        }
    }

}

void BaseTerrain::Render(const mat4 viewMatrix, const mat4 projMatrix, GLuint currentShaderProgram, const vec3& CameraPos)
{
    glUseProgram(currentShaderProgram);
    labhelper::setUniformSlow(currentShaderProgram, "viewMatrix", viewMatrix);
    labhelper::setUniformSlow(currentShaderProgram, "projectionMatrix", projMatrix);

    labhelper::setUniformSlow(currentShaderProgram, "max_height", m_maxHeight);

    labhelper::setUniformSlow(currentShaderProgram, "TextureScale", m_textureScale);

    labhelper::setUniformSlow(currentShaderProgram, "slopeThreshold", m_slope);
    labhelper::setUniformSlow(currentShaderProgram, "slopeMixRange", m_slopeRange);

    for (int i = 0; i < 4; i++) {
        std::string name = "height" + std::to_string(i);
        labhelper::setUniformSlow(currentShaderProgram, name.c_str(), m_pTerrainHeghts[i]);
    }

    for (int i = 0x0; i < 6; i++) {
         glActiveTexture(GL_TEXTURE5+i);
         glBindTexture(GL_TEXTURE_2D, m_pTextures[i].gl_id);
    }
    for (int i = 0x0; i < 6; i++) {
        glActiveTexture(GL_TEXTURE11 + i);
        glBindTexture(GL_TEXTURE_2D, m_pTextureNormals[i].gl_id);
    }

    m_geomipGrid.Render(CameraPos);
}

