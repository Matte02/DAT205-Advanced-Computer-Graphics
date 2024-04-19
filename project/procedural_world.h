#pragma once
#include "scene/height_map_generator.h"
#include "scene/terrain.h"
#include <chrono>
#include <labhelper.h>
#include "scene/camera.h"
#include <erosion.h>

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
	void GuiErosion();


	// Generate Terrain

	void InitializeWorld();
	void RegenerateTerrain();
	void GenerateTerrain();
	void UpdateHeightMap();
	void ErodeHeightMap();


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
	int terrrainShaderProgramIndex = 1;

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
	// TODO: LOOK OVER
	struct SlopeSettings {
		float slope = 35.0f;
		float slopeRange = 12.5f;
	};

	// YUCK FIX
	std::vector<std::string> textFilenames = { "desert_sand_d.jpg",
	"grass_green_d.jpg",
	"mntn_dark_d.jpg",
	"snow1_d.jpg",
	"mntn_brown_d.jpg",
	"snow_mntn2_d.jpg",
	"desert_sand_n.jpg",
	"grass_green_n.jpg",
	"mntn_dark_n.jpg",
	"snow1_n.jpg",
	"mntn_brown_n.jpg",
	"snow_mntn2_n.jpg" };

	// TODO: LOOK OVER
	SlopeSettings slopeSettings;
	WorldSettings worldSettings;

	bool autoUpdate = false;
	bool updateHeightMap = false;
	bool regenerateWorld = false;

	BaseTerrain m_terrain;
	HeightMapGenerator heightMapGenerator;
	Erosion erosion;
	float maxChangeThreshold = 1;
	int erosionIteration = 1;
	Array2D<float> heightMap;
};