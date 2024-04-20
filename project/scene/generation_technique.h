#ifndef GENERATION_TECHNIQUE_H
#define GENERATION_TECHNIQUE_H

#include "technique.h"
#include "NoiseSettings.h" 

class GenerationTechnique : public Technique
{
public:
    GenerationTechnique();

    virtual void Enable();

    virtual bool Init();

    void GenerateHeightMap(int width, int height, NoiseSettings settings);

private:
    void SetNoiseSettings(NoiseSettings settings);

    GLuint m_minHeightLoc = -1;
    GLuint m_lacunarityLoc = -1;
    GLuint m_persistenceLoc = -1;
    GLuint m_octavesLoc = -1;
    GLuint m_offsetsLoc = -1;
    GLuint m_noiseScaleLoc = -1;
    GLuint m_exponentLoc = -1;
};
#endif  /* GENERATION_TECHNIQUE_H */