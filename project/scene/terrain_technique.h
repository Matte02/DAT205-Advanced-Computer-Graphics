#ifndef TERRAIN_TECHNIQUE_H
#define TERRAIN_TECHNIQUE_H

#include "technique.h"
#include <glm/glm.hpp>
using namespace glm;

class TerrainTechnique : public Technique
{
public:
    TerrainTechnique();

    virtual bool Init();

    void SetViewProjectionMatrix(const mat4& viewProjectionMatrix);

    void SetViewMatrix(const mat4& viewMatrix);

    void SetLightDir(const vec3& lightDirection);

private:
    GLuint m_ViewProjectionLoc = -1;
    GLuint m_ViewLoc = -1;

    GLuint m_reversedLightDirLoc = -1;
    GLuint m_heightMapLoc = -1;

};
#endif  /* TERRAIN_TECHNIQUE_H */