#include "terrain_technique.h"
#include "texture_config.h"

#define INVALID_UNIFORM_LOCATION 0xffffffff
constexpr auto SHADER_DIR = "../shaders/";

TerrainTechnique::TerrainTechnique()
{

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

	m_ViewProjectionLoc = GetUniformLocation("viewProjectionMatrix");
	m_ViewLoc = GetUniformLocation("viewMatrix");
	//m_reversedLightDirLoc = GetUniformLocation("reversedLightDir");
	m_heightMapLoc = GetUniformLocation("heightMap");

	if (m_ViewProjectionLoc == INVALID_UNIFORM_LOCATION ||
		m_ViewLoc == INVALID_UNIFORM_LOCATION ||
		//m_reversedLightDirLoc == INVALID_UNIFORM_LOCATION ||
		m_heightMapLoc == INVALID_UNIFORM_LOCATION) {
		return false;
	}

	Enable();

	glUniform1i(m_heightMapLoc, HEIGHT_MAP_TEXTURE_UNIT_INDEX);

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