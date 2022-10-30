#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D albedoTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D materialProperties;
uniform sampler2D currentAlbedo;

uniform vec3 lightDir;
uniform vec3 lightColor;

void main()
{
	// emissivity from red channel of materialProperties in the GBuffer
	float emissivity = clamp(texture( materialProperties, UV ).x, 0.0, 1.0);

	// Light calculations
	vec3 position = texture( positionTexture, UV ).xyz;	
	vec3 normal = texture( normalTexture, UV ).xyz;	
	float dirDot = clamp(-dot(normal, lightDir), 0.0, 1.0);

	// Previous light pass buffer
	vec3 currentColor = texture( currentAlbedo, UV ).xyz;
	
	// Color from the GBuffer
	vec3 albedoColor = texture( albedoTexture, UV ).xyz;	
	
	vec3 finalNonEmissiveColor = ((albedoColor * lightColor) * dirDot) + currentColor;
	
	color = vec4((finalNonEmissiveColor * (1.0 - emissivity)) + (albedoColor * emissivity), 1.0);
}

