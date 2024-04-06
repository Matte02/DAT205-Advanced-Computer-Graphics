#pragma once

#include <vector>
#include "array2d.h"

#include <glm/glm.hpp>
using namespace glm;


class LodManager
{
public:

	int InitLodManager(int PatchSize, int NumPatchesX, int NumPatchesZ, float WorldScale);

	void Update(const vec3& CameraPos);

	/** The Lod for the different parts of a patch.
	*   T  
	* L C R
	*   B  
	* 
	* Core stores the level of detail of the core of the patch.
	* All other are either 1 if they should be a lod higher than the core,
	* or 0 if they should be the same as the core.
	*/
	struct PatchLod {
		int Core = 0;
		int Left = 0;
		int Right = 0;
		int Top = 0;
		int Bottom = 0;
	};

	const PatchLod& GetPatchLod(int PatchX, int PatchZ) const;
	
	void PrintLodMap();

private:
	void CalcLodRegions();
	void CalcMaxLOD();
	void UpdateLodMapPass1(const vec3& CameraPos);
	void UpdateLodMapPass2(const vec3& CameraPos);

	int DistanceToLod(float Distance);

	int m_maxLOD = 0;
	int m_patchSize = 0;
	int m_numPatchesX = 0;
	int m_numPatchesZ = 0;
	float m_worldScale = 0.0f;

	const float m_ZFAR = 2000;

	Array2D<PatchLod> m_map;
	std::vector<int> m_regions;
};

