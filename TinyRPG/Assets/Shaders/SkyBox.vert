#version 330 core
layout (location = 0) in vec3 InPos;
  
uniform mat4 TransformMatrix;
out vec3 WorldPos;

void main()
{
	// Multiply InPos by 2 because our cube is -0.5 to 0.5. We need -1 to 1
    gl_Position = vec4(InPos.xyz * 2, 1.0);
	
	// Use our transform matrix from the camera to ensure we can get the correct normal in the frag shader.
	WorldPos = (TransformMatrix * gl_Position).xyz;	
}