#pragma once

template<class T>
class SSBO
{
public:
	//uint32_t m_Size = 0;
	uint32_t m_RendererID = 0;
	uint32_t m_BindIdx = 0;
	std::vector<T> m_Buffer;
	uint32_t m_ElemSize = sizeof(T);
	
	SSBO()
	{

	}
	void Bind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_BindIdx, m_RendererID);
	};

	//uint32_t Size() { return 0; }
	//void SetSize(uint32_t s)
	//{
	//	m_Size = s;
	//}

	std::vector<T>& GetBuffer() { return m_Buffer; }
	T& operator[](int idx)
	{
		return m_Buffer[idx];
	}
	void Init(const uint32_t bindIdx)
	{
		auto totalSize = m_ElemSize * m_Buffer.size();
		//m_Size = size;
		m_BindIdx = bindIdx;
		if (!m_RendererID)
		{
			glGenBuffers(1, &m_RendererID);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
			glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(m_ElemSize) * m_Buffer.size(), m_Buffer.data(), GL_STATIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_BindIdx, m_RendererID);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
			
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(m_ElemSize) * m_Buffer.size(), m_Buffer.data(), GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_BindIdx, m_RendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
};

