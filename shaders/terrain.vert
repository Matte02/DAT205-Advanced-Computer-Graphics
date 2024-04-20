#version 420

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 Tex0;

out vec2 Tex1;


void main()
{
    gl_Position = vec4(Position, 1.0);

    Tex1 = Tex0;
}