#version 420
precision highp float;

layout (quads, fractional_odd_spacing, ccw) in;

in vec2 Tex2[];

out vec2 Tex3;
out float Height;

uniform sampler2D u_heightMap;
uniform mat4 u_viewProjectionMatrix;

void main()
{
    // Get the results from the tessellator
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // Interpolate texture coordinates
    vec2 t00 = Tex2[0];
    vec2 t01 = Tex2[1];
    vec2 t10 = Tex2[2];
    vec2 t11 = Tex2[3];

    vec2 t0 = mix(t00, t01, u);
    vec2 t1 = mix(t10, t11, u);
    Tex3 = mix(t0, t1, v);

    // Sample height from the height map
    Height = texture(u_heightMap, Tex3).x;

    // Interpolate positions
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 p0 = mix(p00, p01, u);
    vec4 p1 = mix(p10, p11, u);
    vec4 p = mix(p0, p1, v);

    // Add sampled height to the position
    p.y += Height;

    // Transform from world to clip space
    gl_Position = u_viewProjectionMatrix * p;
}