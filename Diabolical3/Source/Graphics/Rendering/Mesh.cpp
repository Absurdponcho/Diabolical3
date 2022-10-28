#include "Mesh.h"
#include "Logging/Logging.h"

DMesh::~DMesh()
{
	if (VertexArray)
	{
		glDeleteVertexArrays(1, &VertexArray);
		Check (!VertexArray);
	}
	if (VertexBuffer)
	{
		glDeleteBuffers(1, &VertexBuffer);
		Check(!VertexBuffer);
	}
	if (ElementBuffer)
	{
		glDeleteBuffers(1, &ElementBuffer);
		Check(!ElementBuffer);
	}
}

const GLuint DMesh::GetVertexArray()
{
	if (!VertexArray)
	{
		GenerateBuffers();
	}

	Check (VertexArray);
	return VertexArray;
}

void DMesh::GenerateBuffers()
{
	// ensure vector is made of floats
	Check(sizeof(Vertices[0]) == sizeof(float) * 3);

	Check (!VertexArray);
	Check (!VertexBuffer);
	Check (!ElementBuffer);
	if (VertexArray || VertexBuffer || ElementBuffer) 
	{
		LOG_ERR("Failed to generate buffers");
		return;
	}

	{
		// Generate and bind VAO, which stores the VBO and EBO
		glGenVertexArrays(1, &VertexArray);
		glBindVertexArray(VertexArray);
	}

	{
		// Generate and bind VBO, which stores vertex information
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

		// Create the buffer
		glBufferData(GL_ARRAY_BUFFER, Vertices.SizeBytes() + UVs.SizeBytes() + Normals.SizeBytes(), NULL, GL_STATIC_DRAW);

		// Put Vertices, UVs and normals in
		glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.SizeBytes(), Vertices.GetData());
		glBufferSubData(GL_ARRAY_BUFFER, Vertices.SizeBytes(), UVs.SizeBytes(), UVs.GetData());
		glBufferSubData(GL_ARRAY_BUFFER, Vertices.SizeBytes() + UVs.SizeBytes(), Normals.SizeBytes(), Normals.GetData());
	}

	{
		// Generate and bind EBO, which stores element information
		glGenBuffers(1, &ElementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.SizeBytes(), Indices.GetData(), GL_STATIC_DRAW);
	}

	// vertices attribute pointer. location is 0, size is 3, stride is 3, offset is 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// uvs attribute pointer. location is 1, size is 2, stride is 2, offset is size of vertices
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)Vertices.SizeBytes());
	glEnableVertexAttribArray(1);

	// normals attribute pointer. location is 1, size is 3, stride is 3, offset is size of vertices
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(Vertices.SizeBytes() + UVs.SizeBytes()));
	glEnableVertexAttribArray(2);
}

