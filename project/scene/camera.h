#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

struct Camera {
    vec3 position;
    vec3 direction;
    float speed;
    vec3 worldUp = vec3(0.0f, 0.1f, 0.0f);

    Camera() : position(vec3(0.0f)), direction(vec3(0.0f, 0.0f, -1.0f)), speed(1.0f) {}

    Camera(const vec3& pos, const vec3& dir, float spd)
        : position(pos), direction(glm::normalize(dir)), speed(spd) {}

    mat4 getViewMatrix() const {
        return lookAt(position, position + direction, worldUp);
    }

    void moveForward(float deltaTime) {
        position += speed * deltaTime * direction;
    }

    void moveBackward(float deltaTime) {
        position -= speed * deltaTime * direction;
    }

    void moveLeft(float deltaTime) {
        vec3 right = cross(direction, worldUp);
        position -= speed * deltaTime * right;
    }

    void moveRight(float deltaTime) {
        vec3 right = cross(direction, worldUp);
        position += speed * deltaTime * right;
    }

    void moveUp(float deltaTime) {
        position += speed * deltaTime * worldUp;
    }

    void moveDown(float deltaTime) {
        position -= speed * deltaTime * worldUp;
    }

    void rotate(float yaw, float pitch) {
        mat4 yawMat = glm::rotate(yaw, worldUp);
        mat4 pitchMat = glm::rotate(pitch, normalize(cross(direction, worldUp)));

        direction = vec3(normalize(pitchMat * yawMat * vec4(direction, 0.0)));
        worldUp = vec3(normalize(pitchMat * vec4(worldUp, 0.0)));
    }
};
