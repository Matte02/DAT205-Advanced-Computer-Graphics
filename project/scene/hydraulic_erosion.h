#pragma once
#ifndef HYDRAULIC_EROSION_H
#define HYDRAULIC_EROSION_H

#include "technique.h"

class HydraulicErosion : public Technique
{
public:
    HydraulicErosion();

    virtual bool Init();

    void Erode(int iterations);

private:

};
#endif  /* HYDRAULIC_EROSION_H */