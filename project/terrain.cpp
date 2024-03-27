#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <string.h>

#include "terrain.h"
#include "labhelper.h"

//#define DEBUG_PRINT

void BaseTerrain::InitTerrain(float WorldScale, int WorldSize)
{
    m_worldScale = WorldScale;
    m_terrainSize = WorldSize;
    m_heightMap.InitArray2D(m_terrainSize, m_terrainSize, 0.0f);
    m_minHeight = 0.0f;
    m_maxHeight = 0.0f;
}

BaseTerrain::~BaseTerrain()
{
    Destroy();
}

void BaseTerrain::Destroy()
{
    m_heightMap.Destroy();
    m_triangleList.Destroy();
}


void BaseTerrain::GenerateHeightMap() {
    m_triangleList.CreateTriangleList(m_terrainSize, m_terrainSize, this);
}


void BaseTerrain::Render(const mat4 viewProjMatrix, GLuint currentShaderProgram)
{
    glUseProgram(currentShaderProgram);
    labhelper::setUniformSlow(currentShaderProgram, "viewProjectionMatrix", viewProjMatrix);
    labhelper::setUniformSlow(currentShaderProgram, "material_color", vec3(1.0,1.0,1.0));

    labhelper::setUniformSlow(currentShaderProgram, "min_height", m_minHeight);
    labhelper::setUniformSlow(currentShaderProgram, "max_height", m_maxHeight);

    m_triangleList.Render();
}

