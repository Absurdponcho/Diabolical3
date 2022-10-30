#version 330 core

in vec2 UV;

out vec4 color;

layout(location = 3) out vec4 MaterialProperties;
in vec3 WorldPos;

uniform vec3 UpColor;
uniform vec3 DownColor;
uniform vec3 HorizonColor;

void main()
{
	vec3 WorldPosNormalized = normalize(WorldPos);
	float UpColorMult = clamp(WorldPosNormalized.y, 0.0, 1.0);
	float DownColorMult = clamp(-WorldPosNormalized.y, 0.0, 1.0);	
	float HorizonColorMult = 1.0 - (UpColorMult + DownColorMult);
	color = vec4((UpColor * UpColorMult) + (DownColor * DownColorMult) + (HorizonColor * HorizonColorMult), 1.0);
	
	MaterialProperties = vec4(1, 0, 0, 0);
}

