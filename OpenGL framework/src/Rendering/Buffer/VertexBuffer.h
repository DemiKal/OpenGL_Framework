#pragma once

class VertexBuffer
{
private:
	unsigned int m_rendererID{};

public:
	VertexBuffer(const void* data, unsigned int size);
	void BufferSubData(std::vector<float> position, std::vector<float> uvs);
	VertexBuffer();
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;
};