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
	Texture1D(uint32_t width, uint32_t channels, dataType dataType, void* data, bool normalized);
	Texture1D();
	~Texture1D() = default;


	unsigned int GetID()const { return m_rendererID; }
	unsigned int GetWidth()  const { return m_width; }
	std::string GetName() const { return m_name; }

	void  Bind(const unsigned int slot = 0) const
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		GLCall(glBindTexture(GL_TEXTURE_1D, m_rendererID));
	}

};

