#include "terrain_technique.h"
#include "texture_config.h"
#include "array"

constexpr auto SHADER_DIR = "../shaders/";

TerrainTechnique::TerrainTechnique()
{

}

void TerrainTechnique::Enable() {
	Technique::Enable();

	glUniform1f(m_maxDistanceLoc, m_maxDistance);
	glUniform1f(m_minDistanceLoc, m_minDistance);

	glUniform1i(m_maxTessLevelLoc, m_maxTessLevel);
	glUniform1i(m_minTessLevelLoc, m_minTessLevel);
}

bool TerrainTechnique::Init()
{
	if (!Technique::Init()) {
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "../shaders/terrain.vert")) {
		return false;
	}

	if (!AddShader(GL_TESS_CONTROL_SHADER, "../shaders/terrain.tesc")) {
		return false;
	}

	if (!AddShader(GL_TESS_EVALUATION_SHADER, "../shaders/terrain.tese")) {
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "../shaders/terrain.frag")) {
		return false;
	}

	if (!Finalize()) {
		return false;
	}

	m_viewModeLoc = GetUniformLocation("u_viewMode");
	m_ViewProjectionLoc = GetUniformLocation("u_viewProjectionMatrix");
	
	m_terrainTexturesLoc = GetUniformLocation("u_terrainTextures");
	m_textureScaleLoc = GetUniformLocation("u_textureScale");
	m_ViewLoc = GetUniformLocation("u_viewMatrix");
	m_heightMapLoc = GetUniformLocation("u_heightMap");
	m_normalMapLoc = GetUniformLocation("u_normalMap");
	m_maxHeightLoc = GetUniformLocation("u_maxHeight");
	m_offSetHeightLoc = GetUniformLocation("u_offSetHeight");
	m_maxDistanceLoc = GetUniformLocation("u_maxDistance");
	m_minDistanceLoc = GetUniformLocation("u_minDistance");
	m_maxTessLevelLoc = GetUniformLocation("u_maxTessLevel");
	m_minTessLevelLoc = GetUniformLocation("u_minTessLevel");


	m_textureStartHeightsLoc = GetUniformLocation("u_startHeights");
	m_textureEndHeightsLoc = GetUniformLocation("u_endHeights");
	m_textureStartSlopeLoc = GetUniformLocation("u_startSlope");
	m_textureEndSlopeLoc = GetUniformLocation("u_endSlope");

	if (m_viewModeLoc		== INVALID_UNIFORM_LOCATION ||
		m_ViewProjectionLoc == INVALID_UNIFORM_LOCATION ||
		m_ViewLoc			== INVALID_UNIFORM_LOCATION ||
		m_heightMapLoc		== INVALID_UNIFORM_LOCATION ||
		m_normalMapLoc		== INVALID_UNIFORM_LOCATION ||
		m_terrainTexturesLoc == INVALID_UNIFORM_LOCATION ||
		m_textureScaleLoc	== INVALID_UNIFORM_LOCATION ||
		m_maxHeightLoc		== INVALID_UNIFORM_LOCATION ||
		m_offSetHeightLoc	== INVALID_UNIFORM_LOCATION ||
		m_maxDistanceLoc	== INVALID_UNIFORM_LOCATION ||
		m_minDistanceLoc	== INVALID_UNIFORM_LOCATION ||
		m_maxTessLevelLoc	== INVALID_UNIFORM_LOCATION ||

		m_textureStartHeightsLoc == INVALID_UNIFORM_LOCATION ||
		m_textureEndHeightsLoc == INVALID_UNIFORM_LOCATION ||
		m_textureStartSlopeLoc == INVALID_UNIFORM_LOCATION ||
		m_textureEndSlopeLoc == INVALID_UNIFORM_LOCATION
		) {
		return false;
	}

	Enable();

	glUniform1i(m_heightMapLoc, HEIGHT_MAP_TEXTURE_UNIT_INDEX);
	glUniform1i(m_terrainTexturesLoc, TERRAIN_TEXTURES_UNIT_INDEX);
	glUniform1i(m_normalMapLoc, NORMAL_MAP_TEXTURE_UNIT_INDEX);

	glUseProgram(0);

	return true;
}

void TerrainTechnique::SetViewProjectionMatrix(const mat4& viewProjectionMatrix)
{
	glUniformMatrix4fv(m_ViewProjectionLoc, 1, GL_FALSE, (const GLfloat*)&viewProjectionMatrix[0].x);
}

void TerrainTechnique::SetViewMatrix(const mat4& viewMatrix)
{
	glUniformMatrix4fv(m_ViewLoc, 1, GL_FALSE, (const GLfloat*)&viewMatrix[0].x);
}

void TerrainTechnique::SetLightDir(const vec3& lightDirection)
{
	vec3 ReversedLightDir = lightDirection * -1.0f;
	ReversedLightDir = normalize(ReversedLightDir);
	glUniform3f(m_reversedLightDirLoc, ReversedLightDir.x, ReversedLightDir.y, ReversedLightDir.z);
}

void TerrainTechnique::SetMaxHeight(const float maxHeight)
{
	glUniform1f(m_maxHeightLoc, maxHeight);
}

void TerrainTechnique::SetOffSetHeight(const float offSetHeight)
{
	glUniform1f(m_offSetHeightLoc, offSetHeight);
}

void TerrainTechnique::SetViewMode(const int viewMode)
{
	glUniform1i(m_viewModeLoc, viewMode);
}

void TerrainTechnique::SetTextureScale(const float textureScale)
{
	glUniform1f(m_textureScaleLoc, textureScale);
}

void TerrainTechnique::SetTextureStartHeights(std::array<float, 6> heights)
{
	glUniform1fv(m_textureStartHeightsLoc, 6, (const GLfloat*) &heights);
}

void TerrainTechnique::SetTextureEndHeights(std::array<float, 6> heights)
{
	glUniform1fv(m_textureEndHeightsLoc, 6, (const GLfloat*)&heights);
}

void TerrainTechnique::SetTextureStartSlopes(std::array<float, 6> slopes)
{
	glUniform1fv(m_textureStartSlopeLoc, 6, (const GLfloat*)&slopes);
}

void TerrainTechnique::SetTextureEndSlopes(std::array<float, 6> slopes)
{
	glUniform1fv(m_textureEndSlopeLoc, 6, (const GLfloat*)&slopes);
}
