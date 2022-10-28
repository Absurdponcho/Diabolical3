#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 Pos;
layout(location = 2) out vec4 Normal;

in vec3 LocalPos;
in vec3 WorldPos;
in vec3 Normals;
in vec3 UVs;

void main()
{
    vec3 Color = vec3(.8, .8, .8);
    float total = floor(LocalPos.x) +
                  floor(LocalPos.y) +
                  floor(LocalPos.z);
    vec3 col = vec3(.3, .3, .3);
    FragColor = vec4(col * mod(total, 2.0) + vec3(.5, .5, .5),1);
	Pos = vec4(WorldPos, 1.0);
	Normal = vec4(Normals, 1);
}