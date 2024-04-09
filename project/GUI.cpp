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

                float yaw = rotationSpeed * deltaTime * (float) - delta_x;
                float pitch = rotationSpeed * deltaTime * (float) - delta_y;
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
        // World Scale
        /*ImGui::Text("Move sun:");
        if (ImGui::Checkbox("##Move Sun", &moveSun))
        */
            ImGui::Text("Shader Program:");
        if (ImGui::SliderInt("##Shader Program", &terrrainShaderProgramIndex, 0, (int)TerrainShaders.size()-1))

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

void ProceduralWorld::GuiTerrain()
{
    if (ImGui::CollapsingHeader("Terrain Generation")) {
        ImGui::Indent();

        // World Scale
        ImGui::Text("World Scale:");
        if (ImGui::SliderFloat("##World Scale", &worldSettings.worldScale, 0.0, 20.0))
            ; // Do nothing here, as we'll generate terrain only when the button is pressed

        // World Size
        ImGui::Text("World Size:");
        if (ImGui::SliderInt("##World Size", &worldSettings.worldSize, 1, 2048))
            ; // Do nothing here, as we'll generate terrain only when the button is pressed

        // World Size
        ImGui::Text("Patch Size:");
        if (ImGui::SliderInt("##Patch Size", &worldSettings.patchSize, 2, 7))
            ;

       /* // Perlin Noise Parameters
        ImGui::Text("Offset:");
        if (ImGui::SliderFloat("##Offset", &terrainNoiseSettings.offset, 0, 100000.0))
            ; // Do nothing here, as we'll generate terrain only when the button is pressed
            
        ImGui::Text("Octaves:");
        if (ImGui::SliderInt("##Octaves", &terrainNoiseSettings.octaves, 0, 8))
            ; // Do nothing here, as we'll generate terrain only when the button is pressed

        ImGui::Text("Gain:");
        if (ImGui::SliderFloat("##Gain", &terrainNoiseSettings.gain, 0.0, 2.0))
            ; // Do nothing here, as we'll generate terrain only when the button is pressed

        ImGui::Text("Lacunarity:");
        if (ImGui::SliderFloat("##Lacunarity", &terrainNoiseSettings.lacunarity, 0.0, 10.0))
            ; // Do nothing here, as we'll generate terrain only when the button is pressed

        ImGui::Text("Sample Scale:");
        if (ImGui::SliderFloat("##SampleScale", &terrainNoiseSettings.sampleScale, 0.0, 0.01))
            ; // Do nothing here, as we'll generate terrain only when the button is pressed

        ImGui::Text("Height Scale:");
        if (ImGui::SliderInt("##Height Scale", &terrainNoiseSettings.maxHeight, 0, 256))
            ; // Do nothing here, as we'll generate terrain only when the button is pressed

        */
        if (ImGui::Button("Generate Terrain")) {
            GenerateTerrain(); // Call generateTerrain function only when this button is pressed
        }

        /*if (ImGui::Button("Save Settings")) {
            saveSettings();
        }

        ImGui::SameLine();
        if (ImGui::Button("Load Settings")) {
            loadSettings();
            generateTerrain(); // Regenerate terrain after loading settings
        }*/

        ImGui::Unindent();
    }
}

void ProceduralWorld::RenderGuiOverlay()
{
    // ----------------- Set variables --------------------------
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);
    // ----------------------------------------------------------

    GuiTexture();

    GuiTerrain();

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    labhelper::perf::drawEventsWindow();

}