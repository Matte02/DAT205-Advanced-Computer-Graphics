#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;
layout(location = 0) out vec4 fragmentColor;


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

in vec4 Color;
in vec3 Pos;
in vec3 WorldNormal;
in float HeightRatio;


vec3 blendNormal(vec3 normal){
	// in wNorm is the world-space normal of the fragment 
    vec3 blending = abs( normal );
    blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0 
    float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);
    return blending;
}


vec4 triplanarMapping (sampler2D textureToUse, vec3 normal, vec3 position) {
    vec3 normalBlend = blendNormal(normal);
	vec4 xColor = texture2D(textureToUse, position.yz * TextureScale);
	vec4 yColor = texture2D(textureToUse, position.xz * TextureScale);
	vec4 zColor = texture2D(textureToUse, position.xy * TextureScale);

  return (xColor * normalBlend.x + yColor * normalBlend.y + zColor * normalBlend.z);
}


vec4 getColor(sampler2D textureToUse) {
    return triplanarMapping(textureToUse, WorldNormal, Pos);
}


vec4 calculateHeightColor() {
    float height = HeightRatio;
    vec4 heightColor;
    if (height < height0) {
        heightColor = getColor(SandTexture);
    } else if (height < height1) {
        heightColor = mix(getColor(SandTexture),
                         getColor(GrassTexture),
                         smoothstep(height0, height1, height));
    } else if (height < height2) {
        heightColor = mix(getColor(GrassTexture),
                         getColor(MountainTexture),
                         smoothstep(height1, height2, height));
    } else if (height < height3) {
        heightColor = mix(getColor(MountainTexture),
                         getColor(SnowTexture),
                         smoothstep(height2, height3, height));
    } else {
        heightColor = getColor(SnowTexture);
    }
    return heightColor;
}

vec4 calculateSlopeColor() {
    float height = HeightRatio;
    vec4 slopeColor;
    if (height < height2) {
        slopeColor = getColor(CliffTexture);
    } else if (height > height3) {
        slopeColor = getColor(SnowCliffTexture);
    } else {
        slopeColor = mix(getColor(CliffTexture),
                         getColor(SnowCliffTexture),
                         smoothstep(height2, height3, height));
    }
    return slopeColor;
}

void main()
{
    vec3 Normal = normalize(WorldNormal);
    vec4 finalColor;
    float slope = degrees(dot(Normal, vec3(0.0, 1.0, 0.0)));
    vec4 heightColor = calculateHeightColor();
    vec4 slopeColor = calculateSlopeColor();
    float blendFactor = smoothstep(slopeThreshold, slopeThreshold+slopeMixRange, slope);
    finalColor = mix(slopeColor, heightColor, blendFactor);

    //Light
    float Diffuse = dot(Normal, -reversedLightDir);

    Diffuse = max(0.3, Diffuse);

    fragmentColor = Color * finalColor * Diffuse;
}