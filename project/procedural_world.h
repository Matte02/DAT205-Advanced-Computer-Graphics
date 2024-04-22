#pragma once
#include "scene/terrain.h"
#include <chrono>
#include <labhelper.h>
#include "scene/camera.h"
#include <erosion.h>
#include "scene/NoiseSettings.h"
#include <array>

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

	void UpdateWorld();

	// Render
	void Render();
	void RenderBackground(const mat4& projectionMatrix);
	void RenderGuiOverlay();

	void UpdateTimers();
	void UpdateWindowDimensions();

	// GUI
	bool HandleEvents(void);
	void HandleCameraMovement();
	void GuiTexture();
	void GuiTerrain();
	void GuiNoiseSettings();


	// Generate Terrain

	void InitializeWorld();
	void RegenerateTerrain();
	void GenerateTerrain();
	void UpdateHeightMap();


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
	int viewMode = 0;

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


	// Camera and View
	Camera camera;
	float nearPlaneZ = 5.0f;
	float farPlaneZ = 2000.0f;
	float viewAngleR = 45.0f;

	vec3 lightPosition = vec3(-400.0f, 400.0f, 0.0f);
	vec4 lightStartPosition = vec4(lightPosition, 1.0f);


	// TODO: LOOK OVER
	struct WorldSettings {
		float worldScale = 64.0f;
		int worldSize = 2048;
		float textureScale = 1;
		int patchSize = 32;
	};

	WorldSettings worldSettings;
	NoiseSettings noiseSettings;

	bool autoUpdate = false;
	bool updateHeightMap = false;
	bool regenerateWorld = false;

	// Texture Settings
#define NUMBER_OF_TEXTURES 6
	const float MAX_SLOPE = 90;
	// Heights are between 0 and 1
	std::array<float, NUMBER_OF_TEXTURES> textureStartHeights = { 0.0f, 0.35f, 0.425f, 0.55f, 0.0, 0.55f };
	std::array<float, NUMBER_OF_TEXTURES> textureBlends = { 0.4f, 0.475f, 0.65f, 1.0f, 0.6, 1.0f };
	// Slope are between 0 and 90
	std::array<float, NUMBER_OF_TEXTURES> textureStartSlope = { 0.0, 0.0, 0.0, 0.0, 22.5, 22};
	std::array<float, NUMBER_OF_TEXTURES> textureEndSlope = { 27.5, 32.5, 32, 32.5, 90, 90 };

	float colorTextureScale = 64;

	float offSetHeight = 0;
	BaseTerrain m_terrain;
	TerrainTechnique terrainTechnique;


	Erosion erosion;
	float maxChangeThreshold = 1;
	int erosionIteration = 1;


};