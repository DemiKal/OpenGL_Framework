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


	unsigned int GetID() const;
	unsigned int GetWidth() const;
	std::string GetName() const;

	void Bind(const unsigned slot) const;

	//void Bind(unsigned int slot = 0) const;
};

