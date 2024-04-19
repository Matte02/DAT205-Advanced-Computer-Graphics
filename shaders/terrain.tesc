#version 420
precision highp float;
layout (vertices = 4) out;

in vec2 Tex1[];
out vec2 Tex2[];

uniform mat4 u_viewMatrix;
uniform float u_minDistance;
uniform float u_maxDistance;
uniform int u_maxTessLevel;
uniform int u_minTessLevel;

// Function to map a value to [0, 1]
float mapTo01(float value, float minVal, float maxVal) {
    return clamp((value - minVal) / (maxVal - minVal), 0.0, 1.0);
}

// Function to calculate tessellation level based on distance
float calculateTessLevel(float dist1, float dist2) {
    return pow(2.0, mix(float(u_maxTessLevel), float(u_minTessLevel), min(dist1, dist2)));
}

void main() {
    // Step 1: Pass through vertex position and texture coordinates
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    Tex2[gl_InvocationID] = Tex1[gl_InvocationID];

    // Step 2: Transform vertices to view space
    vec4 viewSpacePos[4];
    for (int i = 0; i < 4; ++i) {
        viewSpacePos[i] = u_viewMatrix * gl_in[i].gl_Position;
    }

    // Step 3: Calculate distances from the origin in view space
    float distances[4];
    for (int i = 0; i < 4; ++i) {
        distances[i] = length(viewSpacePos[i]);
    }

    // Step 4: Map distances to [0, 1]
    float mappedDistances[4];
    for (int i = 0; i < 4; ++i) {
        mappedDistances[i] = mapTo01(distances[i], u_minDistance, u_maxDistance);
    }

    // Step 5: Calculate tessellation levels for edges
    float tessLevels[4];
    tessLevels[0] = calculateTessLevel(mappedDistances[2], mappedDistances[0]);
    tessLevels[1] = calculateTessLevel(mappedDistances[0], mappedDistances[1]);
    tessLevels[2] = calculateTessLevel(mappedDistances[1], mappedDistances[3]);
    tessLevels[3] = calculateTessLevel(mappedDistances[3], mappedDistances[2]);

    // Step 6: Set outer tessellation levels
    gl_TessLevelOuter[0] = tessLevels[0];
    gl_TessLevelOuter[1] = tessLevels[1];
    gl_TessLevelOuter[2] = tessLevels[2];
    gl_TessLevelOuter[3] = tessLevels[3];

    // Step 7: Set inner tessellation levels
    gl_TessLevelInner[0] = max(tessLevels[0], tessLevels[2]);
    gl_TessLevelInner[1] = max(tessLevels[1], tessLevels[3]);
}
