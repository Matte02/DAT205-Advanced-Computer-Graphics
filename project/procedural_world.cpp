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
const std::string envmap_base_name = "001";
ProceduralWorld::ProceduralWorld() {

	g_window = labhelper::init_window_SDL("Procedural World");

	// Init Camera
	camera.position = vec3(140.0f, 50.0f, 140.0f);
	camera.direction = normalize(vec3(0.0f) - camera.position);
	camera.speed = 50;

	GenerateTerrain();

	ENSURE_INITIALIZE_ONLY_ONCE();
	CreateShaderPrograms();

	glEnable(GL_DEPTH_TEST); // enable Z-buffering
	glEnable(GL_CULL_FACE);  // enables backface culling

}

ProceduralWorld::~ProceduralWorld() {
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

void ProceduralWorld::Render()
{

	labhelper::perf::Scope s("Render");

	// Calculate Projection Matrix. TODO EXTRACT MAGIC NUMBERS AS VARIABLES
	mat4 projMatrix = perspective(radians(45.0f), float(windowWidth) / float(windowHeight), 5.0f, 2000.0f);

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
	auto currentShaderProgram = TerrainShaders.at(1);
	glUseProgram(currentShaderProgram);
	// TODO: EXTRACT INTO UPDATE WORLD FUNCTION
	lightPosition = vec3(rotate(currentTime, camera.worldUp) * lightStartPosition);
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

// TODO: FIX TERRAIN GENERATION
void ProceduralWorld::GenerateTerrain()
{
	// YUCK FIX
	std::vector<std::string> textFilenames = {};
	textFilenames.push_back("desert_sand_d.jpg");
	textFilenames.push_back("grass_green_d.jpg");
	textFilenames.push_back("mntn_dark_d.jpg");
	textFilenames.push_back("snow1_d.jpg");
	textFilenames.push_back("mntn_brown_d.jpg");
	textFilenames.push_back("snow_mntn2_d.jpg");
	textFilenames.push_back("desert_sand_n.jpg");
	textFilenames.push_back("grass_green_n.jpg");
	textFilenames.push_back("mntn_dark_n.jpg");
	textFilenames.push_back("snow1_n.jpg");
	textFilenames.push_back("mntn_brown_n.jpg");
	textFilenames.push_back("snow_mntn2_n.jpg");

	// TODO: LOOK OVER SLOPE IN SHADERS
	m_terrain.setSlope(35.0f, 12.5f);

	// FIX AS WELL
	m_terrain.InitTerrain(1, 1024, 0.07, 5, textFilenames);

	// TODO: PROBABLY EXTRACT INTO STRUCT
	m_terrain.GenerateHeightMap(2, 0.5, 6, 1, 0.2 / 100, 100, 0);
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
