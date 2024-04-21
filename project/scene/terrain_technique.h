#ifndef TERRAIN_TECHNIQUE_H
#define TERRAIN_TECHNIQUE_H

#include "technique.h"
#include <glm/glm.hpp>
using namespace glm;

class TerrainTechnique : public Technique
{
public:
    TerrainTechnique();

    virtual void Enable();

    virtual bool Init();

    void SetViewProjectionMatrix(const mat4& viewProjectionMatrix);

    void SetViewMatrix(const mat4& viewMatrix);

    void SetLightDir(const vec3& lightDirection);

    void SetMaxHeight(const float maxHeight);
    void SetOffSetHeight(const float offSetHeight);

    void SetViewMode(const int viewMode);

    void SetTextureScale(const float textureScale);

private:
    GLuint m_viewModeLoc = -1;

    GLuint m_terrainTexturesLoc = -1;
    GLuint m_textureScaleLoc = -1;

    GLuint m_ViewProjectionLoc = -1;
    GLuint m_ViewLoc = -1;

    GLuint m_reversedLightDirLoc = -1;
    GLuint m_heightMapLoc = -1;
    GLuint m_normalMapLoc = -1;
    GLuint m_maxHeightLoc = -1;
    GLuint m_offSetHeightLoc = -1;

    // Max Distance (Least amount of detail)
    GLuint m_maxDistanceLoc = -1;
    float m_maxDistance = 1024;

    // Min Distance (Most amount of detail)
    GLuint m_minDistanceLoc = -1;
    float m_minDistance = 128;

    // Max Tessellation Level
    GLuint m_maxTessLevelLoc = -1;
    int m_maxTessLevel = 6;

    // Min Tessellation Level
    GLuint m_minTessLevelLoc = -1;
    int m_minTessLevel = 0;
};
#endif  /* TERRAIN_TECHNIQUE_H */