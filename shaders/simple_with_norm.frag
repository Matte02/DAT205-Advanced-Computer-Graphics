#version 460 core

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;
layout(location = 0) out vec4 fragmentColor;


// Textures
layout(binding = 5) uniform sampler2D SandTexture;
layout(binding = 6) uniform sampler2D GrassTexture;
layout(binding = 7) uniform sampler2D MountainTexture;
layout(binding = 8) uniform sampler2D SnowTexture;
layout(binding = 9) uniform sampler2D CliffTexture;
layout(binding = 10) uniform sampler2D SnowCliffTexture;
// Texture Normals
layout(binding = 11) uniform sampler2D SandNormal;
layout(binding = 12) uniform sampler2D GrassNormal;
layout(binding = 13) uniform sampler2D MountainNormal;
layout(binding = 14) uniform sampler2D SnowNormal;
layout(binding = 15) uniform sampler2D CliffNormal;
layout(binding = 16) uniform sampler2D SnowCliffNormal;

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

in vec4 Color;
in vec3 Pos;
in vec3 WorldNormal;
in float HeightRatio;


vec4 getColor(sampler2D textureToUse) {
        return texture2D(textureToUse, Pos.xz*TextureScale);
}



vec4 calculateHeight(sampler2D t0,sampler2D t1,sampler2D t2,sampler2D t3) {
    float height = HeightRatio;
    vec4 heightColor;
    if (height < height0) {
        heightColor = getColor(t0);
    } else if (height < height1) {
        heightColor = mix(getColor(t0),
                         getColor(t1),
                         smoothstep(height0, height1, height));
    } else if (height < height2) {
        heightColor = mix(getColor(t1),
                         getColor(t2),
                         smoothstep(height1, height2, height));
    } else if (height < height3) {
        heightColor = mix(getColor(t2),
                         getColor(t3),
                         smoothstep(height2, height3, height));
    } else {
        heightColor = getColor(t3);
    }
    return heightColor;
}

vec4 calculateSlope(sampler2D t1,sampler2D t2) {
    float height = HeightRatio;
    vec4 slopeColor;
    if (height < height2) {
        slopeColor = getColor(t1);
    } else if (height > height3) {
        slopeColor = getColor(t2);
    } else {
        slopeColor = mix(getColor(t1),
                         getColor(t2),
                         smoothstep(height2, height3, height));
    }
    return slopeColor;
}

void main()
{
    vec3 Normal = normalize(WorldNormal);
    vec4 finalColor;
    float slope = degrees(dot(Normal, vec3(0.0, 1.0, 0.0)));

    vec4 heightColor = calculateHeight(SandTexture, GrassTexture, MountainTexture, SnowTexture);
    vec4 slopeColor = calculateSlope(CliffTexture, SnowCliffTexture);

    vec4 heightTextureNormal = calculateHeight(SandNormal, GrassNormal, MountainNormal, SnowNormal);
    vec4 slopeTextureNormal = calculateSlope(CliffNormal, SnowCliffNormal);

    float blendFactor = smoothstep(slopeThreshold, slopeThreshold+slopeMixRange, slope);
    finalColor = mix(slopeColor, heightColor, blendFactor);

    vec3 finalTextureNormal = vec3(normalize(mix(slopeTextureNormal, heightTextureNormal, blendFactor)));

    vec3 totalNormal = normalize(Normal + finalTextureNormal);
    //Light
    float Diffuse = dot(totalNormal, -reversedLightDir);

    Diffuse = max(0.3, Diffuse);

    fragmentColor = Color * finalColor * Diffuse;
}