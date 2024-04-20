#include "generation_technique.h"
#include "texture_config.h"

GenerationTechnique::GenerationTechnique()
{
}

void GenerationTechnique::Enable()
{
	Technique::Enable();
}

bool GenerationTechnique::Init()
{
	if (!Technique::Init()) {
		return false;
	}
	if (!AddShader(GL_COMPUTE_SHADER, "../shaders/heightmap.comp")) {
		return false;
	}
	if (!Finalize()) {
		return false;
	}

	// Get unifrom locations
	m_lacunarityLoc = GetUniformLocation("u_lacunarity");
	m_persistenceLoc = GetUniformLocation("u_persistence");
	m_octavesLoc = GetUniformLocation("u_octaves");
	m_offsetsLoc = GetUniformLocation("u_offsets");
	m_noiseScaleLoc = GetUniformLocation("u_noiseScale");
	m_exponentLoc = GetUniformLocation("u_exponent");

	// Check if any of them failed
	if (m_lacunarityLoc == INVALID_UNIFORM_LOCATION ||
		m_persistenceLoc == INVALID_UNIFORM_LOCATION ||
		m_octavesLoc == INVALID_UNIFORM_LOCATION ||
		m_offsetsLoc == INVALID_UNIFORM_LOCATION ||
		m_noiseScaleLoc == INVALID_UNIFORM_LOCATION ||
		m_exponentLoc == INVALID_UNIFORM_LOCATION
		) {
		return false;
	}
	Enable();


	glUseProgram(0);
	return true;
}

void GenerationTechnique::GenerateHeightMap(int width, int height, NoiseSettings settings)
{
	Enable(); // Ensure it's enabled.
	SetNoiseSettings(settings);

	glDispatchCompute(width / 32, height / 32, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void GenerationTechnique::SetNoiseSettings(NoiseSettings settings)
{
	glUniform1f(m_lacunarityLoc, settings.lacunarity);
	glUniform1f(m_persistenceLoc, settings.persistence);
	glUniform1f(m_exponentLoc, settings.exponent);
	glUniform1f(m_noiseScaleLoc, settings.noiseScale);

	glUniform1i(m_octavesLoc, settings.octaves);
	glUniform2fv(m_offsetsLoc, 16, (GLfloat*) &settings.offsets[0].x);
}
