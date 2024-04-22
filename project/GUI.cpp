#include "procedural_world.h" // Include the header file for ProceduralWorld class
#include <imgui.h> // Include ImGui library
#include <perf.h>
#include <glm/gtc/random.hpp>

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
		if (ImGui::SliderInt("##View Mode", &viewMode, 0, 15)) {
			terrainTechnique.SetViewMode(viewMode);
		}

		ImGui::Text("Texture Scale:");
		regenerateWorld |= ImGui::SliderFloat("##Texture Scale", &worldSettings.textureScale, 0.1f, 4.0f);

		ImGui::Text("Color Texture Scale:");
		if (ImGui::SliderFloat("##Color Texture Scale", &colorTextureScale, 1.0f, 256.0f)) {
			terrainTechnique.SetTextureScale(colorTextureScale);
		}
		if (ImGui::CollapsingHeader("Individual Texture Settings")) {
			ImGui::Indent();
			for (int i = 0; i < NUMBER_OF_TEXTURES; ++i) {
				ImGui::Text("Texture %d", i + 1);
				/*if (ImGui::SliderFloat(("Start Height##" + std::to_string(i)).c_str(), &textureStartHeights[i], 0.0f, textureBlends[i])) {
					if (textureStartHeights[i] > textureBlends[i]) {
						textureStartHeights[i] = textureBlends[i];
						terrainTechnique.SetTextureStartHeights(textureStartHeights);
					}
				}
				if (ImGui::SliderFloat(("End Height##" + std::to_string(i)).c_str(), &textureBlends[i], textureStartHeights[i], 1.0f)) {
					if (textureStartHeights[i] > textureBlends[i]) {
						textureBlends[i] = textureStartHeights[i];
						terrainTechnique.SetTextureEndHeights(textureBlends);
					}
				}*/
				if (ImGui::SliderFloat(("Start Height##" + std::to_string(i)).c_str(), &textureStartHeights[i], 0.0f, 1)) {
					terrainTechnique.SetTextureStartHeights(textureStartHeights);
				}
				if (ImGui::SliderFloat(("End Height##" + std::to_string(i)).c_str(), &textureBlends[i], 0, 1.0f)) {
					terrainTechnique.SetTextureEndHeights(textureBlends);
				}
				if (ImGui::SliderFloat(("Start Slope##" + std::to_string(i)).c_str(), &textureStartSlope[i], 0.0f, 90)) {
					terrainTechnique.SetTextureStartSlopes(textureStartSlope);
				}
				if (ImGui::SliderFloat(("End Slope##" + std::to_string(i)).c_str(), &textureEndSlope[i],0,90)) {
					terrainTechnique.SetTextureEndSlopes(textureEndSlope);
				}

				ImGui::Separator();
			}
		}
		ImGui::Unindent();
	}
	ImGui::Unindent();
}

void ProceduralWorld::GuiTerrain()
{
	if (ImGui::CollapsingHeader("Terrain Generation")) {
		ImGui::Indent();

		// World Scale
		ImGui::Text("World Scale:");
		regenerateWorld |= ImGui::SliderFloat("##World Scale", &worldSettings.worldScale, 1.0, 128.0);

		// World Size
		ImGui::Text("World Size:");
		regenerateWorld |= ImGui::SliderInt("##World Size", &worldSettings.worldSize, 512, 4096);

		// World Size
		ImGui::Text("Patch Size:");
		regenerateWorld |= ImGui::SliderInt("##Patch Size", &worldSettings.patchSize, 4, 64);

		ImGui::Text("Max Height:");
		if (ImGui::SliderFloat("##Max Height", &noiseSettings.maxHeight, 1, 250.0f)) {
			terrainTechnique.SetMaxHeight(noiseSettings.maxHeight);
		}

		ImGui::Text("Offset Height:");
		if (ImGui::SliderFloat("##Offset Height", &offSetHeight, -50.0f, 50.0f)) {
			terrainTechnique.SetOffSetHeight(offSetHeight);
		}

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


		ImGui::Text("Seed:");
		if (ImGui::SliderInt("##Seed", &noiseSettings.seed, -1000000, 1000000)) {
			regenerateWorld = true;
			noiseSettings.generateRandomOffsets();
		}
		ImGui::Text("Lacunarity:");
		regenerateWorld |= ImGui::SliderFloat("##Lacunarity", &noiseSettings.lacunarity, 0.1f, 2.0f);

		ImGui::Text("Persistence:");
		regenerateWorld |= ImGui::SliderFloat("##Persistence", &noiseSettings.persistence, 0.01f, 1.0f);

		ImGui::Text("Octaves:");
		regenerateWorld |= ImGui::SliderInt("##Octaves", &noiseSettings.octaves, 1, 16);

		ImGui::Text("Noise Scale:");
		regenerateWorld |= ImGui::SliderFloat("##Noise Scale", &noiseSettings.noiseScale, 0.01f, 10.0f);

		ImGui::Text("Redistribution:");
		regenerateWorld |= ImGui::SliderFloat("##Redistribution", &noiseSettings.exponent, 0.01f, 2.0f);

		if (ImGui::CollapsingHeader("Offsets")) {
			ImGui::Indent();
			// Display sliders for each component of offsets
			for (int i = 0; i < 16; ++i) {
				regenerateWorld |= ImGui::SliderFloat2(("Offset " + std::to_string(i)).c_str(), &noiseSettings.offsets[i].x, -100000, 100000);
			}
			ImGui::Unindent();
		}


		// Button to generate random offsets
		if (ImGui::Button("Generate Random offset with new random seed")) {
			noiseSettings.RandomizeSeed();
			noiseSettings.generateRandomOffsets();
			regenerateWorld = true;
		}

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
	//GuiErosion();

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	labhelper::perf::drawEventsWindow();

}