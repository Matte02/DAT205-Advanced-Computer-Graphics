#version 420

layout(location = 0) in vec3 position;
uniform mat4 viewProjectionMatrix;
uniform float min_height;
uniform float max_height;

out vec4 Color;

// Function to map height to grayscale color
vec4 mapHeightToColor(float height) {
    // Map height to grayscale color
    float t = (height - min_height) / (max_height - min_height);
    return vec4(t, t, t, 1.0);
}

void main()
{
	gl_Position = viewProjectionMatrix * vec4(position, 1.0);
	Color = mapHeightToColor(position.y);
}
