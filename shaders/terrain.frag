#version 460 core

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;
layout(location = 0) out vec4 fragmentColor;

#include incl_noise.glsl

in vec2 Tex3;
in float Height;

uniform float u_maxHeight;
uniform int u_viewMode;

// Function to map height to grayscale color
float mapHeightToColor(float height) {
    return height / u_maxHeight;
}

void main()
{
	if (u_viewMode == 1) {
		fragmentColor = vec4(Tex3.x, Tex3.y, 0.0f, 1.0f);
	} else if (u_viewMode == 2) {
		float v = snoise(Tex3);
		fragmentColor = vec4(vec3(v), 1.0f);
	} else {
		fragmentColor= vec4(vec3(mapHeightToColor(Height)), 1.0f);
	}
}
