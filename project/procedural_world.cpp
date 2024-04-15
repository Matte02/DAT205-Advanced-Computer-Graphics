#include "procedural_world.h"
#include <GL/glew.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <chrono>

#include <labhelper.h>
#include <imgui.h>

#include <perf.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <Model.h>
#include "hdr.h"
#include "fbo.h"

#include <fstream> // For file handling
#include <iostream>

using namespace glm;

constexpr auto SHADER_DIR = "../shaders/";
ProceduralWorld::ProceduralWorld() {

	g_window = labhelper::init_window_SDL("Procedural World");

	InitializeWorld();


	ENSURE_INITIALIZE_ONLY_ONCE();
	CreateShaderPrograms();

	glEnable(GL_DEPTH_TEST); // enable Z-buffering
	glEnable(GL_CULL_FACE);  // enables backface culling

}

ProceduralWorld::~ProceduralWorld() {
	heightMap.Destroy();
}

void ProceduralWorld::Run() {
	startTime = std::chrono::system_clock::now();
	while (!stopRunning) {
		UpdateTimers();

		// Check for events (Keyboard etc)
		stopRunning = HandleEvents();

		// Inform imgui of new frame
		labhelper::newFrame(g_window);

		UpdateWindowDimensions();
		
		UpdateWorld();
		// Render Scene
		Render();
		// Render GUI Overlay
		RenderGuiOverlay();
		// Finish the frame and render the GUI
		labhelper::finishFrame();

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);
	}
	labhelper::shutDown(g_window);
}

// TODO: FIX TEXTURES
void ProceduralWorld::InitTextures()
{

	environmentMap = labhelper::loadHdrTexture("../scenes/envmaps/001.hdr");
}

void ProceduralWorld::UpdateWorld()
{
	lightPosition = vec3(rotate(currentTime, camera.worldUp) * lightStartPosition);
}

void ProceduralWorld::Render()
{

	labhelper::perf::Scope s("Render");

	mat4 projMatrix = perspective(radians(viewAngleR), float(windowWidth) / float(windowHeight), nearPlaneZ, farPlaneZ);

	if (isWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	// TODO: MAYBE EXTRACT IN TO FUNCTION
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		labhelper::perf::Scope s("Background");
		RenderBackground(projMatrix);
	}
	auto currentShaderProgram = TerrainShaders.at(terrrainShaderProgramIndex);

	glUseProgram(currentShaderProgram);

	labhelper::setUniformSlow(currentShaderProgram, "reversedLightDir", normalize(vec3(-lightPosition)));

	m_terrain.Render(camera.getViewMatrix(), projMatrix, currentShaderProgram, camera.position);
}

void ProceduralWorld::RenderBackground(const mat4& projectionMatrix)
{
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, environmentMap);
	glActiveTexture(GL_TEXTURE0);

	GLuint program = EnvoirmentShaders.at(0);
	glUseProgram(program);
	labhelper::setUniformSlow(program, "environment_multiplier", environment_multiplier);
	labhelper::setUniformSlow(program, "inv_PV", inverse(projectionMatrix * camera.getViewMatrix()));
	labhelper::setUniformSlow(program, "camera_pos", camera.position);
	labhelper::drawFullScreenQuad();
}

void ProceduralWorld::UpdateTimers()
{
	std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
	previousTime = currentTime;
	currentTime = timeSinceStart.count();
	deltaTime = currentTime - previousTime;
}

void ProceduralWorld::UpdateWindowDimensions()
{
	///////////////////////////////////////////////////////////////////////////
	// Check if window size has changed and resize buffers as needed
	///////////////////////////////////////////////////////////////////////////
	{
		int w;
		int h;
		SDL_GetWindowSize(g_window, &w, &h);
		if (w != windowWidth || h != windowHeight)
		{
			windowWidth = w;
			windowHeight = h;
		}
	}

}

void ProceduralWorld::InitializeWorld()
{
	GenerateTerrain();

	// Init Camera
	camera.position = vec3(140.0f, 50.0f, 140.0f);
	camera.direction = normalize(vec3(0.0f) - camera.position);
	camera.speed = 50;

}
void ProceduralWorld::RegenerateTerrain() {
#if TIMING_ENABLED
	auto start = std::chrono::high_resolution_clock::now();
#endif
	m_terrain.Destroy();
	GenerateTerrain();
#if TIMING_ENABLED
	auto end = std::chrono::high_resolution_clock::now(); // End timing
	std::chrono::duration<double> duration = end - start;
	std::cout << "Time taken for REGENERATING new world: " << duration.count() << " seconds" << std::endl;
#endif
}

// TODO: FIX TERRAIN GENERATION
void ProceduralWorld::GenerateTerrain()
{
	// TODO: LOOK OVER SLOPE IN SHADERS
	m_terrain.setSlope(slopeSettings.slope, slopeSettings.slopeRange);


	heightMapGenerator.GenerateHeightMap(&heightMap, worldSettings.worldSize, true);
	// FIX AS WELL
	m_terrain.InitTerrain(worldSettings.worldScale,
		worldSettings.worldSize,
		worldSettings.textureScale,
		worldSettings.patchSize,
		textFilenames,
		&heightMap);
}

void ProceduralWorld::UpdateHeightMap()
{	// Start timing
#if TIMING_ENABLED
	auto start = std::chrono::high_resolution_clock::now();
#endif
	heightMapGenerator.GenerateHeightMap(&heightMap, worldSettings.worldSize, true);
	m_terrain.UpdateHeightMapHeights(&heightMap);

#if TIMING_ENABLED
	auto end = std::chrono::high_resolution_clock::now(); // End timing
	std::chrono::duration<double> duration = end - start;
	std::cout << "Time taken for generating new heightmap and updating the vertices: " << duration.count() << " seconds" << std::endl;
#endif
}



void ProceduralWorld::ErodeHeightMap() {
    // Copy the original terrain map
    Array2D<float> originalHeightMapCopy(worldSettings.worldSize, worldSettings.worldSize);
    for (int i = 0; i < heightMap.GetSize(); ++i) {
        originalHeightMapCopy.Set(i, heightMap.Get(i));
    }

#if TIMING_ENABLED
    auto start = std::chrono::high_resolution_clock::now();
#endif
    erosion.Erode(&heightMap, worldSettings.worldSize, erosionIteration, false);
#if TIMING_ENABLED
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    std::chrono::duration<double> duration = end - start;
    std::cout << "Erosion time (" << erosionIteration << " iterations) : " << duration.count() << " seconds" << std::endl;
#endif

    // Compare the original map with the eroded map
    for (int i = 0; i < heightMap.GetSize(); ++i) {
        float originalHeight = originalHeightMapCopy.Get(i);
        float erodedHeight = heightMap.Get(i);
        float heightChange = std::abs(erodedHeight - originalHeight);
        if (heightChange > maxChangeThreshold) {
            std::cout << "Height at position " << i << " changed by " << heightChange << std::endl;
        }
    }
    m_terrain.UpdateHeightMapHeights(&heightMap);
}



void ProceduralWorld::CreateShaderPrograms()
{
	CreateEnvoirmentShaders();
	CreateTerrainShaders();

}

void ProceduralWorld::CreateEnvoirmentShaders()
{
	EnvoirmentShaders.emplace_back(CreateShaderProgram("background", "background"));
	InitTextures();
}

void ProceduralWorld::CreateTerrainShaders()
{
	TerrainShaders.emplace_back(CreateShaderProgram("simple", "simple"));
	TerrainShaders.emplace_back(CreateShaderProgram("single_tex", "single_tex"));
	TerrainShaders.emplace_back(CreateShaderProgram("simple_with_norm", "single_tex"));
}


GLuint ProceduralWorld::CreateShaderProgram(const std::string& fragName, const std::string& vertName) const
{
	const std::string fragPath = SHADER_DIR + fragName + ".frag";
	const std::string vertPath = SHADER_DIR + vertName + ".vert";
	GLuint shader = labhelper::loadShaderProgram(vertPath, fragPath, false);
	if (shader == 0)
	{
		printf("Could not load shader program with Fragment Shader (%s) and Vertex Shader (%s).", fragName.c_str(), vertName.c_str());
		exit(1);
	}
	return shader;
}
