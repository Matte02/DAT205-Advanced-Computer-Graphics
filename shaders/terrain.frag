#version 460 core

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;
layout(location = 0) out vec4 fragmentColor;

layout (binding=3) uniform sampler2DArray u_terrainTextures;

#include incl_noise.glsl

in DATA{
	vec3 worldPos;
	vec2 textureCoords;
	float HeightRatio;
}In;

uniform int u_viewMode;
uniform sampler2D u_heightMap;
uniform sampler2D u_normalMap;
uniform float u_textureScale;

// Textures
#define NUM_TEXTURES 6
uniform float u_startHeights[NUM_TEXTURES];
uniform float u_endHeights[NUM_TEXTURES];
uniform float u_startSlope[NUM_TEXTURES];
uniform float u_endSlope[NUM_TEXTURES];

float InverseLerp(float a, float b, float x) { return (x - a) / (b - a); }
float InverseLerpClamped(float a, float b, float x) { return clamp(InverseLerp(a, b, x), 0, 1); }

float CustomLerp(float a, float b, float c) {
    if (c < a || c > b) {
        return 0.0;
    } else if (c == (a + b) / 2.0) {
        return 1.0;
    } else {
        float distance_from_mid = abs(c - (a + b) / 2.0);
        float max_distance = abs(b - a) / 2.0;
        return 1.0 - (distance_from_mid / max_distance);
    }
}

#define NUM_TEXTURES 6
float[NUM_TEXTURES] TerrainBlending(vec3 worldNormal) {
	float drawStrenghts[NUM_TEXTURES];

	vec3 normal = normalize(worldNormal);
	float slope = degrees(abs(dot(normal, vec3(0.0, 1.0, 0.0))));

	float total = 0;
	for (int i = 0; i < NUM_TEXTURES; i++) {
		float heightValue = CustomLerp(u_startHeights[i], u_endHeights[i], In.HeightRatio);
		float slopeValue = CustomLerp(u_startSlope[i], u_endSlope[i], slope); // IDK WHAT FUNTION IT SHOULD BE
		float value = heightValue * slopeValue;
		total += value;
		drawStrenghts[i] = value;
	}
	for (int i = 0; i < NUM_TEXTURES; i++) {
		drawStrenghts[i] = max(0, drawStrenghts[i]/total);
	}
	return drawStrenghts;
}

vec4 TerrainColoring() {
	vec2 scaledTexCoord = In.textureCoords * u_textureScale;

	vec3 normal = texture(u_normalMap, In.textureCoords).xyz;
	float[NUM_TEXTURES] drawStrenghts = TerrainBlending(normal);

	vec3 color = vec3(0);
	for (int i = 0; i < drawStrenghts.length(); i++) {
	if (drawStrenghts[i] > 0.0001) {
			color += texture(u_terrainTextures, vec3(scaledTexCoord, i)).xyz * drawStrenghts[i];
		}
	}
	return vec4(color, 1.0f);
}

void main()
{
	vec2 scaledTexCoord = In.textureCoords * u_textureScale;
	if (u_viewMode == 1) {
	// Render UV-map
		fragmentColor = vec4(In.textureCoords, 0.0f, 1.0f);
	} else if (u_viewMode == 2) {
	// Render normal map
		vec3 normal = texture(u_normalMap, In.textureCoords).xyz;
		fragmentColor = vec4(normal, 1.0f);
	} else if (u_viewMode >= 3 && u_viewMode < 9) {
	// Render each texture individually
		int index = u_viewMode - 3;
		float t = u_startHeights[index];
		t = u_endHeights[index];
		t = u_startSlope[index];
		t = u_endSlope[index];
		fragmentColor = texture(u_terrainTextures, vec3(scaledTexCoord, index));
	} else if (u_viewMode == 9) {
	// Actual Color
		fragmentColor = TerrainColoring();
	} else if (u_viewMode == 10) {
	// Draw Strenght Debug
		float totalDrawStrength = 0;
		vec3 normal = texture(u_normalMap, In.textureCoords).xyz;
		float[NUM_TEXTURES] drawStrenghts = TerrainBlending(normal);
		vec3 outColor;
		for (int i = 0; i < drawStrenghts.length(); i++) {
			totalDrawStrength += drawStrenghts[i];
		}
		if (totalDrawStrength > 1.0001) {
			outColor = vec3(1.0, 0,0);
		} else if (totalDrawStrength < 0.9999) {
			outColor = vec3(0, 0, 1);
		} else {
			outColor = TerrainColoring().xyz;
		}
		fragmentColor = vec4(vec3(drawStrenghts[0]), 1.0f);
	} else if (u_viewMode == 11) {
	// Actual Color
		vec3 normal = texture(u_normalMap, In.textureCoords).xyz;
		fragmentColor = vec4(u_endSlope[0], u_startSlope[0], u_startSlope[1],1.0f);
	} else {
	// Render Grayscale based on height
		fragmentColor= vec4(vec3(In.HeightRatio), 1.0f);
	}
}
