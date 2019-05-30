#include "precomp.h"


VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	auto* rec = (float*)data;

		GLCall(glGenBuffers(1, &m_rendererID));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}


void VertexBuffer::InterleaveVertexData(
	const  std::vector<float>& position,
	const std::vector<float>&  uvs) const
{
	//const unsigned int posSize = position.size() * sizeof(float);
	//GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, posSize, &position[0]));
	//
	//if (!uvs.empty())
	//{
	//	GLCall(glBufferSubData(GL_ARRAY_BUFFER, posSize, 0 * uvs.size() * sizeof(float), &uvs[0]));
	//}

}

void VertexBuffer::BufferSubData(
	const std::vector<float>& position,
	const std::vector<float>&  uvs) const
{
	 	//glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	//glEnableVertexAttribArray(0);    // We like submitting vertices on stream 0 for no special reason
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)0 );      // The starting point of the VBO, for the vertices
	//glEnableVertexAttribArray(1);    // We like submitting normals on stream 1 for no special reason
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*) 12 );     // The starting point of normals, 12 bytes away
	//glEnableVertexAttribArray(2);    // We like submitting texcoords on stream 2 for no special reason
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(24));     // The starting point of texcoords, 24 bytes away

	//const unsigned int posSize = position.size() * sizeof(float);
	//GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, posSize, &position[0]));
	//
	//if (!uvs.empty())
	//{
	// 	GLCall(glBufferSubData(GL_ARRAY_BUFFER, posSize, 0*uvs.size() * sizeof(float), &uvs[0]));
	//}
}
VertexBuffer::VertexBuffer() : m_rendererID(0)
{
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_rendererID));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
}

void VertexBuffer::UnBind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
