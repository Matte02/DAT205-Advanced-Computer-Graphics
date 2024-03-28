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

//#define DEBUG_PRINT

void BaseTerrain::InitTerrain(float WorldScale, int WorldSize, float TextureScale, const std::vector<std::string>& TextureFilenames)
{
    m_worldScale = WorldScale;
    m_terrainSize = WorldSize;
    m_textureScale = TextureScale;
    
    m_heightMap.InitArray2D(m_terrainSize, m_terrainSize, 0.0f);
    InitTextures(TextureFilenames);
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

void BaseTerrain::InitTextures(const std::vector<std::string>& TextureFilenames) {
    if (TextureFilenames.size() != 6) {
        printf("%s:%d - number of provided textures (%llu) is not equal to the size of the texture array (%i)\n",
            __FILE__, __LINE__, TextureFilenames.size(), 6);
        exit(0);
    }
    int i = 0;
    for (std::string filename : TextureFilenames) {
        labhelper::Texture tex;
        if (!tex.load("../scenes/Textures/", filename, 4)) {
            printf("%s:%d - Failed to load texture (%s) \n", __FILE__, __LINE__, filename.c_str());
            exit(0);
        }
        m_pTextures[i++] = tex;
    }
}

BaseTerrain::~BaseTerrain()
{
    Destroy();
}

void BaseTerrain::Destroy()
{
    m_heightMap.Destroy();
    m_triangleList.Destroy();

    for (int i = 0; i < 6; i++) {
        if (m_pTextures[i].valid && m_pTextures[i].gl_id != 0) {
            glDeleteTextures(1, &m_pTextures[i].gl_id);
            m_pTextures[i].gl_id = 0; 
        }
    }
}


void BaseTerrain::GenerateHeightMap() {
    m_triangleList.CreateTriangleList(m_terrainSize, m_terrainSize, this);
}


void BaseTerrain::Render(const mat4 viewMatrix, const mat4 projMatrix, GLuint currentShaderProgram)
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

    m_triangleList.Render();
}

