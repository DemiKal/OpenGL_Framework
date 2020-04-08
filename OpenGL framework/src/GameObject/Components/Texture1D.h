#pragma once
enum class dataType
{
	INT32,
	UINT32,
	FLOAT32
};

class Texture1D
{
private:
	unsigned int m_rendererID;
	std::string m_name;
	//std::string path;
	unsigned int m_width;

public:
	//Texture1D(void* data, const int width, GLenum type, bool normalized);
	Texture1D(const unsigned int width, const unsigned int channels, dataType datatype, void* data, bool normalized);
	Texture1D();
	~Texture1D() = default;


	inline unsigned int GetID()const { return m_rendererID; }
	inline unsigned int GetWidth()  const { return m_width; }
	inline std::string GetName() const { return m_name; }

	inline void  Bind(const unsigned int slot = 0)
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		GLCall(glBindTexture(GL_TEXTURE_1D, m_rendererID));
	}


	//Texture1D(const int width, const std::string& typeName, void* data, bool normalized);
};

