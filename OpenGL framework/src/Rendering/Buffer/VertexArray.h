#pragma once
 
  class VertexArray
{
private:
	unsigned int m_RendererID;


public:
	VertexArray();
	~VertexArray();
	void Bind() const;
	void Unbind() const;

	virtual  void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
};
