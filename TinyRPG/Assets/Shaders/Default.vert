#version 330 core
layout (location = 0) in vec3 InPos;
layout (location = 1) in vec2 InUVs;
layout (location = 2) in vec3 InNormals;
  
uniform mat4 MVP;
uniform mat4 ModelMat;
out vec3 LocalPos;
out vec3 WorldPos;
out vec3 Normals;
out vec2 UVs;

void main()
{
    gl_Position = MVP * vec4(InPos, 1.0);
	WorldPos = (ModelMat * vec4(InPos, 1.0)).xyz;
	LocalPos = InPos;
	Normals = InNormals;
	UVs = InUVs;	
}