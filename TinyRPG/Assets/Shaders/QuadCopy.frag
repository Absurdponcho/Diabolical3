#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D albedoTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

void main()
{
	color = vec4(abs(texture( albedoTexture, UV ).xyz), 1.0);
}