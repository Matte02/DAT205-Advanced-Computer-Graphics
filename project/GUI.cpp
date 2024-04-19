#include "procedural_world.h" // Include the header file for ProceduralWorld class
#include <imgui.h> // Include ImGui library
#include <perf.h>

// Define named constants for key bindings
enum KeyBindings {
	KEY_W = SDL_SCANCODE_W,
	KEY_S = SDL_SCANCODE_S,
	KEY_A = SDL_SCANCODE_A,
	KEY_D = SDL_SCANCODE_D,
	KEY_Q = SDL_SCANCODE_Q,
	KEY_E = SDL_SCANCODE_E,
	KEY_ESCAPE = SDLK_ESCAPE,
	KEY_G = SDLK_g,
	KEY_F = SDLK_f,
};

// Handle events function definition
bool ProceduralWorld::HandleEvents(void) {
	// Check events (keyboard among others)
	SDL_Event event;
	bool quitEvent = false;
	while (SDL_PollEvent(&event)) {
		labhelper::processEvent(&event); // Process events with labhelper library

		if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		{
			g_isMouseDragging = false;
		}
		// Switch statement to handle different types of events
		switch (event.type) {
			// SDL_QUIT event: window is closed
		case SDL_QUIT:
			quitEvent = true;
			break;
			// SDL_KEYUP event: a key is released
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
				// ESCAPE key: quit application
			case KEY_ESCAPE:
				quitEvent = true;
				break;
				// G key: toggle GUI visibility
			case KEY_G:
				labhelper::isGUIvisible() ? labhelper::hideGUI() : labhelper::showGUI();
				break;
				// F key: toggle wireframe mode
			case KEY_F:
				isWireframe = !isWireframe;
				break;
				// Default case: do nothing
			default:
				break;
			}
			break;
			// SDL_MOUSEBUTTONDOWN event: mouse button pressed
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT && (!labhelper::isGUIvisible() || !ImGui::GetIO().WantCaptureMouse)) {
				g_isMouseDragging = true;
				int x;
				int y;
				SDL_GetMouseState(&x, &y);
				g_prevMouseCoords.x = x;
				g_prevMouseCoords.y = y;
			}
			break;
			// SDL_MOUSEMOTION event: mouse moved
		case SDL_MOUSEMOTION:
			if (g_isMouseDragging) {
				int delta_x = event.motion.x - g_prevMouseCoords.x;
				int delta_y = event.motion.y - g_prevMouseCoords.y;
				float rotationSpeed = 0.1f;

				float yaw = rotationSpeed * deltaTime * (float)-delta_x;
				float pitch = rotationSpeed * deltaTime * (float)-delta_y;
				camera.rotate(yaw, pitch);

				g_prevMouseCoords.x = event.motion.x;
				g_prevMouseCoords.y = event.motion.y;
			}
			break;
			// Default case: do nothing for other event types
		default:
			break;
		}


	}
	HandleCameraMovement();
	return quitEvent; // Return whether quit event occurred
}

// Handle camera movement function definition
void ProceduralWorld::HandleCameraMovement() {
	// Get keyboard state
	const uint8_t* state = SDL_GetKeyboardState(nullptr);
	// Handle camera movement based on pressed keys

	if (state[KEY_W]) {
		camera.moveForward(deltaTime);
	}
	if (state[KEY_S]) {
		camera.moveBackward(deltaTime);
	}
	if (state[KEY_A]) {
		camera.moveLeft(deltaTime);
	}
	if (state[KEY_D]) {
		camera.moveRight(deltaTime);
	}
	if (state[KEY_Q]) {
		camera.moveDown(deltaTime);
	}
	if (state[KEY_E]) {
		camera.moveUp(deltaTime);
	}
}

void ProceduralWorld::GuiTexture()
{
	if (ImGui::CollapsingHeader("Texture")) {
		ImGui::Indent();

		ImGui::Text("View Mode:");
		if (ImGui::SliderInt("##View Mode", &viewMode, 0, 1))

			ImGui::Text("Texture Scale:");
		if (ImGui::SliderFloat("##Texture Scale", &worldSettings.textureScale, 0.0f, 0.1f))
			GenerateTerrain();
		/*if (ImGui::CollapsingHeader("Height Thresholds")) {
			ImGui::Indent();
			ImGui::Text("Keep the height values in order. I.e 1 should be lower than 2.");
			ImGui::Text("Height 1:");
			if (ImGui::SliderFloat("##Height 1", &heightThresholds[0], 0.0, 1)) {
				m_terrain.setTerrainHeights(heightThresholds);
				GenerateTerrain();
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
		}*/
		ImGui::Text("Slope");
		if (ImGui::SliderFloat("##Slope", &slopeSettings.slope, 0.0f, 90.0f)) {
			GenerateTerrain();
		}
		ImGui::Text("Slope Range");
		if (ImGui::SliderFloat("##Slope Range", &slopeSettings.slopeRange, 0.0f, 90.0f)) {
			GenerateTerrain();
		}
		ImGui::Unindent();
	}
}

void ProceduralWorld::GuiErosion() {
	if (ImGui::CollapsingHeader("Erosion")) {
		ImGui::Indent();

		ImGui::Text("Change Threshold:");
		ImGui::SliderFloat("##Change Threshold", &maxChangeThreshold, 0, 10);
		ImGui::Text("Erosion Radius:");
		ImGui::SliderInt("##Erosion Radius", &erosion.erosionRadius, 2, 8);

		ImGui::Text("Droplet Innertia:");
		ImGui::SliderFloat("##Droplet Innertia", &erosion.inertia, 0, 1);
		
		ImGui::Text("Droplet Sediment Capacity Factor:");
		ImGui::SliderFloat("##Droplet Sediment Capacity Factor", &erosion.sedimentCapacityFactor, 1, 20);

		ImGui::Text("Droplet Minimum Sediment Capacity:");
		ImGui::SliderFloat("##Droplet Minimum Sediment Capacity", &erosion.minSedimentCapacity, 0, 1);

		ImGui::Text("Erode Speed:");
		ImGui::SliderFloat("##Erode Speed", &erosion.erodeSpeed, 0, 1);

		ImGui::Text("Deposit Speed:");
		ImGui::SliderFloat("##Deposit Speed", &erosion.depositSpeed, 0, 1);

		ImGui::Text("Gravity:");
		ImGui::SliderFloat("##Gravity Gravity", &erosion.gravity, 0, 10);

		ImGui::Text("Evaporation Speed:");
		ImGui::SliderFloat("##Evaporation Evaporation", &erosion.evaporateSpeed, 0, 1);

		ImGui::Text("Water Size:");
		ImGui::SliderFloat("##Water Size", &erosion.initialWaterVolume, 1, 10000);

		ImGui::Text("Max Droplet Life:");
		ImGui::SliderInt("##Max Droplet Life", &erosion.maxDropletLifetime, 15, 45);

		ImGui::Text("Iterations:");
		ImGui::SliderInt("##Erosion Iterations", &erosionIteration, 1, 10000);
		if (ImGui::Button("Erode Terrain")) {
			ErodeHeightMap();
		}

		ImGui::Unindent();
	}
}

void ProceduralWorld::GuiTerrain()
{
	if (ImGui::CollapsingHeader("Terrain Generation")) {
		ImGui::Indent();

		// World Scale
		ImGui::Text("World Scale:");
		regenerateWorld |= ImGui::SliderFloat("##World Scale", &worldSettings.worldScale, 0.0, 20.0);

		// World Size
		ImGui::Text("World Size:");
		regenerateWorld |= ImGui::SliderInt("##World Size", &worldSettings.worldSize, 1, 2048);

		// World Size
		ImGui::Text("Patch Size:");
		regenerateWorld |= ImGui::SliderInt("##Patch Size", &worldSettings.patchSize, 2, 7);

		GuiNoiseSettings();

		if (ImGui::Button("Generate Terrain") || autoUpdate) {
			if (regenerateWorld) {
				RegenerateTerrain();
			}
			else if (updateHeightMap) {
				UpdateHeightMap();
			}
			regenerateWorld = false;
			updateHeightMap = false;
		}

		ImGui::Unindent();
	}
}

void ProceduralWorld::GuiNoiseSettings()
{
	if (ImGui::CollapsingHeader("Noise Settings")) {
		ImGui::Indent();

		NoiseSettings& noiseSettings = heightMapGenerator.getNoiseSettings();
		ImGui::Text("Noise Type:");
		const char* items[] = { "DEFAULT", "FBM", "TURBULENCE", "RIDGE" };
		int currentItem = noiseSettings.noiseType;
		if (ImGui::Combo("##NoiseType", &currentItem, items, IM_ARRAYSIZE(items))) {
			noiseSettings.noiseType = static_cast<Noise>(currentItem);
			updateHeightMap = true;
		}

		ImGui::Text("Lacunarity:");
		updateHeightMap |= ImGui::SliderFloat("##Lacunarity", &noiseSettings.lacunarity, 0.0f, 10.0f);

		ImGui::Text("Gain:");
		updateHeightMap |= ImGui::SliderFloat("##Gain", &noiseSettings.gain, 0.0f, 2.0f);

		ImGui::Text("Octaves:");
		updateHeightMap |= ImGui::SliderInt("##Octaves", &noiseSettings.octaves, 0, 8);

		ImGui::Text("Sample Scale:");
		updateHeightMap |= ImGui::SliderFloat("##SampleScale", &noiseSettings.sampleScale, 0.0f, 0.01f);

		ImGui::Text("Min Height:");
		updateHeightMap |= ImGui::SliderFloat("##MinHeight", &noiseSettings.minHeight, 0.0f, 1000.0f);

		ImGui::Text("Max Height:");
		updateHeightMap |= ImGui::SliderFloat("##MaxHeight", &noiseSettings.maxHeight, 0.0f, 1000.0f);

		ImGui::Text("Ridge Offset:");
		updateHeightMap |= ImGui::SliderFloat("##RidgeOffset", &noiseSettings.ridgeOffset, 0.0f, 10.0f);

		ImGui::Text("Sample Offset:");
		// Adjusting the layout for X, Y, Z components of sampleOffset vector
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(50); // Set the width of the input box
		updateHeightMap |= ImGui::InputFloat("##SampleOffsetX", &noiseSettings.sampleOffset.x);
		ImGui::SameLine(); // Adjust this value to your preference
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(50); // Set the width of the input box
		updateHeightMap |= ImGui::InputFloat("##SampleOffsetY", &noiseSettings.sampleOffset.y);
		ImGui::SameLine(); // Adjust this value to your preference
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(50); // Set the width of the input box
		updateHeightMap |= ImGui::InputFloat("##SampleOffsetZ", &noiseSettings.sampleOffset.z);

		ImGui::Unindent();
	}
}

void ProceduralWorld::RenderGuiOverlay()
{
	// ----------------- Set variables --------------------------
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		ImGui::GetIO().Framerate);
	// ----------------------------------------------------------

			// Create Checkbox
	ImGui::Checkbox("Auto update terrain (on change vs button)", &autoUpdate);

	GuiTexture();

	GuiTerrain();
	GuiErosion();

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	labhelper::perf::drawEventsWindow();

}