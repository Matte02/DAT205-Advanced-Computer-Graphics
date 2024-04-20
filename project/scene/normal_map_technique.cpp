#include "normal_map_technique.h"

NormalMapTechnique::NormalMapTechnique()
{
}

bool NormalMapTechnique::Init()
{
	if (!Technique::Init()) {
		return false;
	}
	if (!AddShader(GL_COMPUTE_SHADER, "../shaders/normalmap.comp")) {
		return false;
	}
	if (!Finalize()) {
		return false;
	}

	return true;
}

void NormalMapTechnique::GenerateNormalMap(int width, int height)
{
	Enable(); // Ensure it's enabled.

	glDispatchCompute(width / 32, height / 32, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}