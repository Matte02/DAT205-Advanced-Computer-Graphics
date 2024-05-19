#include "hydraulic_erosion.h"

HydraulicErosion::HydraulicErosion()
{
}

bool HydraulicErosion::Init()
{
	if (!Technique::Init()) {
		return false;
	}
	if (!AddShader(GL_COMPUTE_SHADER, "../shaders/hydraulic_erosion.comp")) {
		return false;
	}
	if (!Finalize()) {
		return false;
	}

	return true;
}

void HydraulicErosion::Erode(int iterations)
{
	Enable(); // Ensure it's enabled.


	glDispatchCompute(iterations / 32, iterations / 32, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}