#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;
layout(location = 0) out vec4 fragmentColor;


in vec2 Tex3;
in float Color;
void main()
{
	fragmentColor = vec4(Color, Color, Color, 1.0f);
}
