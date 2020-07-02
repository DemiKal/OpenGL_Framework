#pragma once

class IndexBuffer
{
private:
	unsigned int m_rendererID{};
	unsigned int m_Count{};


public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	IndexBuffer();
	~IndexBuffer();

	void Bind() const;
	static void UnBind();

	[[nodiscard]] unsigned int GetCount() const;;
};