#version 460 core

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;
layout(location = 0) out vec4 fragmentColor;

layout (binding=3) uniform sampler2DArray u_terrainTextures;

#include incl_noise.glsl

in vec2 Tex3;
in float Height;

uniform int u_viewMode;
uniform sampler2D u_heightMap;
uniform sampler2D u_normalMap;
uniform float u_textureScale;
void main()
{
	vec2 scaledTexCoord = Tex3 * u_textureScale; // TODO Add scale as uniform
	if (u_viewMode == 1) {
		fragmentColor = vec4(Tex3, 0.0f, 1.0f);
	} else if (u_viewMode == 2) {
		vec3 normal = texture(u_normalMap, Tex3).xyz;
		fragmentColor = vec4(normal, 1.0f);
	} else if (u_viewMode >= 3 && u_viewMode < 9) {
		int index = u_viewMode - 3;
		vec4 color = texture(u_terrainTextures,vec3(scaledTexCoord, index));
		fragmentColor = color;
	} else {
		fragmentColor= vec4(vec3(Height), 1.0f);
	}
}
