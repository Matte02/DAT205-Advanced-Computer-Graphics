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

void BaseTerrain::InitTerrain(float WorldScale)
{
    m_worldScale = WorldScale;
    m_terrainSize = 1024;
    m_triangleList.CreateTriangleList(m_terrainSize, m_terrainSize, this);
}


void BaseTerrain::Render(const mat4 viewProjMatrix, GLuint currentShaderProgram)
{
    glUseProgram(currentShaderProgram);
    labhelper::setUniformSlow(currentShaderProgram, "viewProjectionMatrix", viewProjMatrix);
    labhelper::setUniformSlow(currentShaderProgram, "material_color", vec3(1.0,1.0,1.0));

    m_triangleList.Render();
}

