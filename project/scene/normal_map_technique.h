#ifndef NORMAL_MAP_TECHNIQUE_H
#define NORMAL_MAP_TECHNIQUE_H

#include "technique.h"

class NormalMapTechnique : public Technique
{
public:
    NormalMapTechnique();

    virtual bool Init();

    void GenerateNormalMap(int width, int height);

private:

};
#endif  /* NORMAL_MAP_TECHNIQUE_H */