#pragma once

class VertexBuffer
{
private:
	unsigned int m_rendererID{};
	void* bufferData;
public:
	VertexBuffer(const void* data, unsigned int size);
	void InterleaveVertexData(const std::vector<float>& position, const std::vector<float>& uvs) const;
	void BufferSubData(const std::vector<float>& position, const std::vector<float>& uvs) const;
	VertexBuffer();
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;
	void ClearData();
};


class InterleavedVB : VertexBuffer
{
	InterleavedVB() {}

	 

};