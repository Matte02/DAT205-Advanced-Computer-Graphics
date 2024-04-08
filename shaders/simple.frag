#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(binding = 5) uniform sampler2D SandTexture;
layout(binding = 6) uniform sampler2D GrassTexture;
layout(binding = 7) uniform sampler2D MountainTexture;
layout(binding = 8) uniform sampler2D SnowTexture;
layout(binding = 9) uniform sampler2D CliffTexture;
layout(binding = 10) uniform sampler2D SnowCliffTexture;

// Light
uniform vec3 reversedLightDir;

uniform float TextureScale;

// Height Thresholds
uniform float height0;
uniform float height1;
uniform float height2;
uniform float height3;

// Slope Threshold
uniform float slopeThreshold;
uniform float slopeMixRange;


layout(location = 0) out vec4 fragmentColor;

in vec4 Color;
void main()
{
	fragmentColor = Color;
}
