#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D albedoTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D currentAlbedo;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
	vec3 position = texture( positionTexture, UV ).xyz;
	float lightDistSquared = 1.0 / (distance (position, lightPos) * distance (position, lightPos));
	vec4 currentColor = texture( currentAlbedo, UV );
	color = vec4((texture( albedoTexture, UV ).xyz * lightColor) * lightDistSquared, 1.0) + currentColor;
}

