#version 420

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 InTex;
layout (location = 2) in vec3 Normal;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float max_height;

out vec4 Color;
out vec2 Tex;
out vec3 Pos;
out vec3 WorldNormal; // Pass normal to fragment shader
out float HeightRatio;

// Function to map height to grayscale color
float mapHeightToColor(float height) {
    // Map height to grayscale color
    float t = (height) / (max_height);
    return t;
}

void main()
{
	gl_Position = projectionMatrix *viewMatrix  * vec4(Position, 1.0);
    float height_ratio = mapHeightToColor(Position.y);
    float c = height_ratio * 0.8 + 0.2;
	Color = vec4(c, c ,c, 1.0);
    Pos = Position;
    WorldNormal = Normal;
    HeightRatio = height_ratio;
}
