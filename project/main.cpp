
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif

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
using namespace glm;

#include <Model.h>
#include "hdr.h"
#include "fbo.h"
#include "scene/perline_noise_terrain.h"

#include <fstream> // For file handling
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Various globals
///////////////////////////////////////////////////////////////////////////////
SDL_Window* g_window = nullptr;
float currentTime = 0.0f;
float previousTime = 0.0f;
float deltaTime = 0.0f;
int windowWidth, windowHeight;
bool isWireframe = false;

// Mouse input
ivec2 g_prevMouseCoords = { -1, -1 };
bool g_isMouseDragging = false;

///////////////////////////////////////////////////////////////////////////////
// Shader programs
///////////////////////////////////////////////////////////////////////////////
GLuint shaderProgram;       // Shader for rendering the final image
GLuint simpleShaderProgram; // Shader used to draw the shadow map
GLuint backgroundProgram;
GLuint singleTexProgram;
GLuint normalTexShader;

///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
float environment_multiplier = 1.5f;
GLuint environmentMap;
const std::string envmap_base_name = "001";

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
vec3 lightPosition;
vec3 point_light_color = vec3(1.f, 1.f, 1.f);

float point_light_intensity_multiplier = 10000.0f;




///////////////////////////////////////////////////////////////////////////////
// Camera parameters.
///////////////////////////////////////////////////////////////////////////////
vec3 cameraPosition(140.0f, 50.0f, 140.0f);
vec3 cameraDirection = normalize(vec3(0.0f) - cameraPosition);
float cameraSpeed = 50.f;

vec3 worldUp(0.0f, 1.0f, 0.0f);

///////////////////////////////////////////////////////////////////////////////
// Models
///////////////////////////////////////////////////////////////////////////////
PerlinNoiseTerrain m_terrain;

// Terrain Texture
std::vector<std::string> textFilenames = {};
static float textureScale = 0.070;
std::vector<float> heightThresholds = {0.1, 0.3, 0.6, 0.9};
static float slopeThreshold = 35.0;
static float slopeRange = 12.5;

static int shaderProgramToUse = 1;
bool moveSun = true;

// Terrain Generation Variables:
static float worldScale = 1;
static float offset = 0;
static int worldSize = 1024;
static int patchSize = 5;
static int octaves = 6;
static float gain = 0.450;
static float lacunarity = 2.1;
static float sampleScale = 0.2;
static int maxHeight = 100;
static int wrap = 0;


labhelper::Model* sphereModel = nullptr;
void generateTerrain() {
	m_terrain.Destroy();
	m_terrain.InitTerrain(worldScale, worldSize, textureScale, patchSize, textFilenames);
	m_terrain.GenerateHeightMap(lacunarity, gain, octaves, offset, sampleScale/100, maxHeight, wrap);
}

void loadShaders(bool is_reload)
{
	GLuint shader = labhelper::loadShaderProgram("../shaders/simple.vert", "../shaders/simple.frag", is_reload);
	if(shader != 0)
	{
		simpleShaderProgram = shader;
	}

	shader = labhelper::loadShaderProgram("../shaders/background.vert", "../shaders/background.frag", is_reload);
	if(shader != 0)
	{
		backgroundProgram = shader;
	}

	shader = labhelper::loadShaderProgram("../shaders/shading.vert", "../shaders/shading.frag", is_reload);
	if(shader != 0)
	{
		shaderProgram = shader;
	}

	shader = labhelper::loadShaderProgram("../shaders/single_tex.vert", "../shaders/single_tex.frag", is_reload);
	if (shader != 0)
	{
		singleTexProgram = shader;
	}
	shader = labhelper::loadShaderProgram("../shaders/single_tex.vert", "../shaders/simple_with_norm.frag", is_reload);
	if (shader != 0)
	{
		normalTexShader = shader;
	}
}

void initTextures() {
	textFilenames.push_back("desert_sand_d.jpg");
	textFilenames.push_back("grass_green_d.jpg");
	textFilenames.push_back("mntn_dark_d.jpg");
	textFilenames.push_back("snow1_d.jpg");
	textFilenames.push_back("mntn_brown_d.jpg");
	textFilenames.push_back("snow_mntn2_d.jpg");
}

void initTextureNormals() {
	textFilenames.push_back("desert_sand_n.jpg");
	textFilenames.push_back("grass_green_n.jpg");
	textFilenames.push_back("mntn_dark_n.jpg");
	textFilenames.push_back("snow1_n.jpg");
	textFilenames.push_back("mntn_brown_n.jpg");
	textFilenames.push_back("snow_mntn2_n.jpg");
}


///////////////////////////////////////////////////////////////////////////////
/// This function is called once at the start of the program and never again
///////////////////////////////////////////////////////////////////////////////
void initialize()
{
	ENSURE_INITIALIZE_ONLY_ONCE();

	///////////////////////////////////////////////////////////////////////
	//		Load Shaders
	///////////////////////////////////////////////////////////////////////
	loadShaders(false);

	///////////////////////////////////////////////////////////////////////
	// Load models and set up model matrices
	///////////////////////////////////////////////////////////////////////
	sphereModel = labhelper::loadModelFromOBJ("../scenes/sphere.obj");

	///////////////////////////////////////////////////////////////////////
	// Load environment map
	///////////////////////////////////////////////////////////////////////
	environmentMap = labhelper::loadHdrTexture("../scenes/envmaps/" + envmap_base_name + ".hdr");
	initTextures();
	initTextureNormals();
	m_terrain.setSlope(slopeThreshold, slopeRange);
	generateTerrain();

	glEnable(GL_DEPTH_TEST); // enable Z-buffering
	glEnable(GL_CULL_FACE);  // enables backface culling
}

void debugDrawLight(const glm::mat4& viewMatrix,
	const glm::mat4& projectionMatrix,
	const glm::vec3& worldSpaceLightPos)
{
	mat4 modelMatrix = glm::translate(worldSpaceLightPos);
	glUseProgram(shaderProgram);
	labhelper::setUniformSlow(shaderProgram, "modelViewProjectionMatrix",
		projectionMatrix * viewMatrix * modelMatrix);
	labhelper::render(sphereModel);
}


void drawScene(GLuint currentShaderProgram,
	const mat4& viewMatrix,
	const mat4& projectionMatrix,
	const mat4& lightViewMatrix,
	const mat4& lightProjectionMatrix) 
{

	// Light source
	/*
	vec4 viewSpaceLightPosition = viewMatrix * vec4(lightPosition, 1.0f);
	labhelper::setUniformSlow(currentShaderProgram, "point_light_color", point_light_color);
	labhelper::setUniformSlow(currentShaderProgram, "point_light_intensity_multiplier",
		point_light_intensity_multiplier);
	labhelper::setUniformSlow(currentShaderProgram, "viewSpaceLightPosition", vec3(viewSpaceLightPosition)); */

	glUseProgram(currentShaderProgram);
	labhelper::setUniformSlow(currentShaderProgram, "reversedLightDir", normalize(vec3(-lightPosition)));

	m_terrain.Render(viewMatrix, projectionMatrix, currentShaderProgram, cameraPosition);
}

void drawBackground(const mat4& viewMatrix, const mat4& projectionMatrix)
{
	glUseProgram(backgroundProgram);
	labhelper::setUniformSlow(backgroundProgram, "environment_multiplier", environment_multiplier);
	labhelper::setUniformSlow(backgroundProgram, "inv_PV", inverse(projectionMatrix * viewMatrix));
	labhelper::setUniformSlow(backgroundProgram, "camera_pos", cameraPosition);
	labhelper::drawFullScreenQuad();
}


///////////////////////////////////////////////////////////////////////////////
/// This function will be called once per frame, so the code to set up
/// the scene for rendering should go here
///////////////////////////////////////////////////////////////////////////////
void display(void)
{
	labhelper::perf::Scope s( "Display" );

	///////////////////////////////////////////////////////////////////////////
	// Check if window size has changed and resize buffers as needed
	///////////////////////////////////////////////////////////////////////////
	{
		int w, h;
		SDL_GetWindowSize(g_window, &w, &h);
		if(w != windowWidth || h != windowHeight)
		{
			windowWidth = w;
			windowHeight = h;
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// setup matrices
	///////////////////////////////////////////////////////////////////////////
	mat4 projMatrix = perspective(radians(45.0f), float(windowWidth) / float(windowHeight), 5.0f, 2000.0f);
	mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraDirection, worldUp);

	vec4 lightStartPosition = vec4(400.0f, 400.0f, 0.0f, 1.0f);
	if (moveSun)
		lightPosition = vec3(rotate(currentTime, worldUp) * lightStartPosition);
	mat4 lightViewMatrix = lookAt(lightPosition, vec3(0.0f), worldUp);
	mat4 lightProjMatrix = perspective(radians(45.0f), 1.0f, 25.0f, 100.0f);

	///////////////////////////////////////////////////////////////////////////
	// Bind the environment map(s) to unused texture units
	///////////////////////////////////////////////////////////////////////////
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, environmentMap);
	glActiveTexture(GL_TEXTURE0);


	///////////////////////////////////////////////////////////////////////////
	// Draw from camera
	///////////////////////////////////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{
		labhelper::perf::Scope s("Background");
		drawBackground(viewMatrix, projMatrix);
	}

	{

		labhelper::perf::Scope s("Scene");
		if (shaderProgramToUse == 0) {
			drawScene(simpleShaderProgram, viewMatrix, projMatrix, lightViewMatrix, lightProjMatrix);
		}
		else if (shaderProgramToUse == 1) {
			drawScene(singleTexProgram, viewMatrix, projMatrix, lightViewMatrix, lightProjMatrix);
		}
		else {
			drawScene(normalTexShader, viewMatrix, projMatrix, lightViewMatrix, lightProjMatrix);
		}
	}
	//debugDrawLight(viewMatrix, projMatrix, vec3(lightPosition));
}


///////////////////////////////////////////////////////////////////////////////
/// This function is used to update the scene according to user input
///////////////////////////////////////////////////////////////////////////////
bool handleEvents(void)
{
	// check events (keyboard among other)
	SDL_Event event;
	bool quitEvent = false;
	while(SDL_PollEvent(&event))
	{
		labhelper::processEvent( &event );

		if(event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
		{
			quitEvent = true;
		}
		if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g)
		{
			if ( labhelper::isGUIvisible() )
			{
				labhelper::hideGUI();
			}
			else
			{
				labhelper::showGUI();
			}
		}

		if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_f)
		{
			isWireframe = !isWireframe;

			if (isWireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT
		   && (!labhelper::isGUIvisible() || !ImGui::GetIO().WantCaptureMouse))
		{
			g_isMouseDragging = true;
			int x;
			int y;
			SDL_GetMouseState(&x, &y);
			g_prevMouseCoords.x = x;
			g_prevMouseCoords.y = y;
		}

		if(!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		{
			g_isMouseDragging = false;
		}

		if(event.type == SDL_MOUSEMOTION && g_isMouseDragging)
		{
			// More info at https://wiki.libsdl.org/SDL_MouseMotionEvent
			int delta_x = event.motion.x - g_prevMouseCoords.x;
			int delta_y = event.motion.y - g_prevMouseCoords.y;
			float rotationSpeed = 0.1f;
			mat4 yaw = rotate(rotationSpeed * deltaTime * -delta_x, worldUp);
			mat4 pitch = rotate(rotationSpeed * deltaTime * -delta_y,
			                    normalize(cross(cameraDirection, worldUp)));
			cameraDirection = vec3(pitch * yaw * vec4(cameraDirection, 0.0f));
			g_prevMouseCoords.x = event.motion.x;
			g_prevMouseCoords.y = event.motion.y;
		}
	}

	// check keyboard state (which keys are still pressed)
	const uint8_t* state = SDL_GetKeyboardState(nullptr);
	vec3 cameraRight = cross(cameraDirection, worldUp);

	if(state[SDL_SCANCODE_W])
	{
		cameraPosition += cameraSpeed * deltaTime * cameraDirection;
	}
	if(state[SDL_SCANCODE_S])
	{
		cameraPosition -= cameraSpeed * deltaTime * cameraDirection;
	}
	if(state[SDL_SCANCODE_A])
	{
		cameraPosition -= cameraSpeed * deltaTime * cameraRight;
	}
	if(state[SDL_SCANCODE_D])
	{
		cameraPosition += cameraSpeed * deltaTime * cameraRight;
	}
	if(state[SDL_SCANCODE_Q])
	{
		cameraPosition -= cameraSpeed * deltaTime * worldUp;
	}
	if(state[SDL_SCANCODE_E])
	{
		cameraPosition += cameraSpeed * deltaTime * worldUp;
	}
	return quitEvent;
}


void saveSettings() {
	std::ofstream file("terrain_settings.txt");
	if (file.is_open()) {
		file << worldScale << "\n";
		file << worldSize << "\n";
		file << offset << "\n";
		file << octaves << "\n";
		file << gain << "\n";
		file << lacunarity << "\n";
		file << sampleScale << "\n";
		file << maxHeight << "\n";
		file << wrap << "\n";
		file.close();
	}
}

void loadSettings() {
	std::ifstream file("terrain_settings.txt");
	if (file.is_open()) {
		// Attempt to read settings from the file
		if (file >> worldScale >> worldSize >> offset >> octaves >> gain >> lacunarity >> sampleScale >> maxHeight >> wrap) {
			// Check if all variables were successfully read
			std::string extra;
			if (file >> extra) {
				// If there's extra content in the file, inform the user and prompt them to save a new file
				std::cerr << "Error: Unexpected content in settings file. Please save a new file." << std::endl;
			}
			else {
				// Close the file if all variables were successfully read
				file.close();
				return;
			}
		}
		else {
			// If any reading operation fails, inform the user and prompt them to save a new file
			std::cerr << "Error: Unable to read settings from file. Please save a new file." << std::endl;
		}

		// Close the file
		file.close();
	}
	else {
		// If file opening fails, display an error message
		std::cerr << "Error opening settings file" << std::endl;
	}
}


void renderTerrainUI() {
	static bool showTable = false;
	if (ImGui::CollapsingHeader("Texture")) {
		ImGui::Indent();
		// World Scale
		ImGui::Text("Move sun:");
		if (ImGui::Checkbox("##Move Sun", &moveSun))

		ImGui::Text("Shader Program:");
		if (ImGui::SliderInt("##Shader Program", &shaderProgramToUse, 0, 2))

		ImGui::Text("Texture Scale:");
		if (ImGui::SliderFloat("##Texture Scale", &textureScale, 0.0, 0.1))
			generateTerrain();
		if (ImGui::CollapsingHeader("Height Thresholds")) {
			ImGui::Indent();
			ImGui::Text("Keep the height values in order. I.e 1 should be lower than 2.");
			ImGui::Text("Height 1:");
			if (ImGui::SliderFloat("##Height 1", &heightThresholds[0], 0.0, 1)) {
				m_terrain.setTerrainHeights(heightThresholds);
				generateTerrain();
			}
			ImGui::Text("Height 2:");
			if (ImGui::SliderFloat("##Height 2", &heightThresholds[1], 0.0, 1)) {
				m_terrain.setTerrainHeights(heightThresholds);
				generateTerrain();
			}
			ImGui::Text("Height 3:");
			if (ImGui::SliderFloat("##Height 3", &heightThresholds[2], 0.0, 1)) {
				m_terrain.setTerrainHeights(heightThresholds);
				generateTerrain();
			}
			ImGui::Text("Height 4:");
			if (ImGui::SliderFloat("##Height 4", &heightThresholds[3], 0.0, 1)) {
				m_terrain.setTerrainHeights(heightThresholds);
				generateTerrain();
			}
			ImGui::Unindent();
		}
		ImGui::Text("Slope");
		if (ImGui::SliderFloat("##Slope", &slopeThreshold, 0.0, 90.0)) {
			m_terrain.setSlope(slopeThreshold, slopeRange);
			generateTerrain();
		}
		ImGui::Text("Slope Range");
		if (ImGui::SliderFloat("##Slope Range", &slopeRange, 0.0, 90.0)) {
			m_terrain.setSlope(slopeThreshold, slopeRange);
			generateTerrain();
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Terrain Generation")) {
		ImGui::Indent();

		// World Scale
		ImGui::Text("World Scale:");
		if (ImGui::SliderFloat("##World Scale", &worldScale, 0.0, 20.0))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		// World Size
		ImGui::Text("World Size:");
		if (ImGui::SliderInt("##World Size", &worldSize, 1, 2048))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		// World Size
		ImGui::Text("Patch Size:");
		if (ImGui::SliderInt("##Patch Size", &patchSize, 2, 7))
			;

		// Perlin Noise Parameters
		ImGui::Text("Offset:");
		if (ImGui::SliderFloat("##Offset", &offset, 0, 100000.0))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		ImGui::Text("Octaves:");
		if (ImGui::SliderInt("##Octaves", &octaves, 0, 8))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		ImGui::Text("Gain:");
		if (ImGui::SliderFloat("##Gain", &gain, 0.0, 2.0))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		ImGui::Text("Lacunarity:");
		if (ImGui::SliderFloat("##Lacunarity", &lacunarity, 0.0, 10.0))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		ImGui::Text("Sample Scale:");
		if (ImGui::SliderFloat("##SampleScale", &sampleScale, 0.0, 10))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		ImGui::Text("Height Scale:");
		if (ImGui::SliderInt("##Height Scale", &maxHeight, 0, 256))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		ImGui::Text("Wrap:");
		if (ImGui::SliderInt("##Wrap", &wrap, 0, 256))
			; // Do nothing here, as we'll generate terrain only when the button is pressed

		if (ImGui::Button("Generate Terrain")) {
			generateTerrain(); // Call generateTerrain function only when this button is pressed
		}

		if (ImGui::Button("Save Settings")) {
			saveSettings();
		}

		ImGui::SameLine();
		if (ImGui::Button("Load Settings")) {
			loadSettings();
			generateTerrain(); // Regenerate terrain after loading settings
		}

		ImGui::Unindent();
	}
}




///////////////////////////////////////////////////////////////////////////////
/// This function is to hold the general GUI logic
///////////////////////////////////////////////////////////////////////////////
void gui()
{
	// ----------------- Set variables --------------------------
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
	            ImGui::GetIO().Framerate);
	// ----------------------------------------------------------


	if (labhelper::isGUIvisible()) {
		renderTerrainUI();
	}

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	labhelper::perf::drawEventsWindow();


}



int main(int argc, char* argv[])
{
	g_window = labhelper::init_window_SDL("OpenGL Project");

	initialize();

	bool stopRendering = false;
	auto startTime = std::chrono::system_clock::now();

	while(!stopRendering)
	{
		//update currentTime
		std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
		previousTime = currentTime;
		currentTime = timeSinceStart.count();
		deltaTime = currentTime - previousTime;

		// check events (keyboard among other)
		stopRendering = handleEvents();

		// Inform imgui of new frame
		labhelper::newFrame( g_window );

		// render to window
		display();

		// Render overlay GUI.
		gui();

		// Finish the frame and render the GUI
		labhelper::finishFrame();

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);
	}

	// Shut down everything. This includes the window and all other subsystems.
	labhelper::shutDown(g_window);
	return 0;
}
