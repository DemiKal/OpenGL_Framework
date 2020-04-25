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

	void Bind()   const;
	static void UnBind();

	inline unsigned int GetCount() const { return m_Count; };
};