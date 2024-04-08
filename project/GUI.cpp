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

void ProceduralWorld::RenderGuiOverlay()
{
    // ----------------- Set variables --------------------------
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);
    // ----------------------------------------------------------



    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    labhelper::perf::drawEventsWindow();

}