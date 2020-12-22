#pragma once

template<class T>
class SSBO
{
public:
	uint32_t m_RendererID = 0;
	uint32_t m_Size = 0;
	uint32_t m_BindIdx = 0;
	std::vector<T> m_Buffer;
	uint32_t byteSizeElem = sizeof(T);
	
	SSBO()
	{

	}
	void Bind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_BindIdx, m_RendererID);

	};
	uint32_t Size() { return 0; }
	void SetSize(uint32_t s)
	{
		m_Size = s;
	}

	void Init(void* dataPtr, const uint32_t size, const uint32_t elemCount, const uint32_t bindIdx)
	{
		if (!m_RendererID)
		{
			GLCall(glGenBuffers(1, &m_RendererID));
			GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID));
			GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size) * elemCount, dataPtr, GL_STATIC_COPY));
			GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_BindIdx, m_RendererID));
			GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
		}

		m_Size = size;
		m_BindIdx = bindIdx;
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID));
		GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size) * elemCount, dataPtr, GL_STATIC_COPY));
		GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_BindIdx, m_RendererID));
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
	}
};

