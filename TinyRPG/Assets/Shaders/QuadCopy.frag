#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;

void main()
{
    color = vec4(texture( renderedTexture, UV ).xyz, 1.0) + vec4(UV.x, UV.y, 0.0, 1.0);
}