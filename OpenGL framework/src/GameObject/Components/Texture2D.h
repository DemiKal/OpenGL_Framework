#pragma once
//#include <stb_image.h>

class Texture2D
{
private:
	std::string m_type = "";
	std::string m_path = "";
	unsigned int m_RendererID = 0;
	unsigned int m_Width = 0;
	unsigned int m_Height= 0;

	void GetEmbeddedTexture(const aiTexture* texture);
	void GenerateTexture(unsigned char* data, int nrComponents, int width, int height);

	void TextureFromFile(const std::string& fullPath);

public:
	Texture2D(const std::string& fullPath, const std::string& typeName);
	Texture2D(const aiTexture* aitexture, const std::string& texType);
	Texture2D(
		const GLenum internalFormat,
		const unsigned int p_width,
		const unsigned int p_height,
		const unsigned int border,
		const GLenum format,
		const GLenum type,
		const GLenum minFilter = GL_LINEAR,
		const GLenum magFilter = GL_LINEAR,
		const GLenum wrap_S = GL_CLAMP_TO_BORDER,
		const GLenum wrap_T = GL_CLAMP_TO_BORDER,
		const void* data = nullptr);

	~Texture2D() = default;

	[[nodiscard]] std::string GetType() const;
	[[nodiscard]] std::string GetPath() const;
	[[nodiscard]] unsigned int GetID() const;
	[[nodiscard]] unsigned int GetWidth() const;
	[[nodiscard]] unsigned int GetHeight() const;

	void Bind(const int textureIndex = 0) const;
	void Delete();
};

