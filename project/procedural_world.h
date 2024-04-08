#pragma once
#include "scene/perline_noise_terrain.h"
#include <chrono>
#include <labhelper.h>
#include "scene/camera.h"

class ProceduralWorld {
public:

	ProceduralWorld();

	~ProceduralWorld();

	void Run();



private:

	// Shader Programs
	void CreateShaderPrograms();
	void CreateEnvoirmentShaders();
	void CreateTerrainShaders();
	GLuint CreateShaderProgram(const std::string& fragName, const std::string& vertName) const;

	// Textures
	void InitTextures();

	// Render
	void Render();
	void RenderBackground(const mat4& projectionMatrix);
	void RenderGuiOverlay();

	void UpdateTimers();
	void UpdateWindowDimensions();

	// GUI
	bool HandleEvents(void);
	void HandleCameraMovement();
	

	// Generate Terrain
	void GenerateTerrain();


	bool stopRunning = false;


	///////////////////////////////////////////////////////////////////////////////
	// Shader Programs
	///////////////////////////////////////////////////////////////////////////////
	std::vector<GLuint> EnvoirmentShaders = {};
	std::vector<GLuint> TerrainShaders = {};

	///////////////////////////////////////////////////////////////////////////////
	// Textures
	///////////////////////////////////////////////////////////////////////////////
	GLuint environmentMap;
	float environment_multiplier = 1.5f;

	///////////////////////////////////////////////////////////////////////////////
	// Rendering Settings
	///////////////////////////////////////////////////////////////////////////////
	bool isWireframe = false;

	///////////////////////////////////////////////////////////////////////////////
	// Window
	///////////////////////////////////////////////////////////////////////////////
	SDL_Window* g_window = nullptr;
	int windowWidth;
	int windowHeight;

	///////////////////////////////////////////////////////////////////////////////
	// Mouse
	///////////////////////////////////////////////////////////////////////////////
	ivec2 g_prevMouseCoords = { -1, -1 };
	bool g_isMouseDragging = false;

	///////////////////////////////////////////////////////////////////////////////
	// Time
	///////////////////////////////////////////////////////////////////////////////
	float currentTime = 0.0f;
	float previousTime = 0.0f;
	float deltaTime = 0.0f;
	std::chrono::time_point<std::chrono::system_clock> startTime;

	Camera camera;
	vec3 lightPosition = vec3(-400.0f, 400.0f, 0.0f);
	vec4 lightStartPosition = vec4(lightPosition, 1.0f);

	// TODO: REFACTOR m_terrain
	PerlinNoiseTerrain m_terrain;
};