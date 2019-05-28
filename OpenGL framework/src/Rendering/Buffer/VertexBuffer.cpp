#include "precomp.h"


VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	auto* rec = (float*)data;

	GLCall(glGenBuffers(1, &m_rendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::BufferSubData(const std::vector<float>& position,
	const std::vector<float>&  uvs) const
{
	const unsigned int posSize = position.size() * sizeof(float);
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, posSize, &position[0]));

	if (!uvs.empty())
	{
	 	GLCall(glBufferSubData(GL_ARRAY_BUFFER, posSize, 0*uvs.size() * sizeof(float), &uvs[0]));
	}
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
