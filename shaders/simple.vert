#version 460 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 InTex;
layout (location = 2) in vec3 Normal;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float max_height;

out vec4 Color;

// Function to map height to grayscale color
vec4 mapHeightToColor(float height) {
    // Map height to grayscale color
    float t = (height) / (max_height);
    return vec4(t, t, t, 1.0);
}

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(Position, 1.0);
	Color = mapHeightToColor(Position.y);
}
