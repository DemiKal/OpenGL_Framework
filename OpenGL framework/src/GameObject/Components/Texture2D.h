#pragma once
class Texture2D
{
private:
	std::string m_type = "";
	std::string m_path = "";
	unsigned int m_rendererID = 0;
	unsigned int m_width = 0;
	unsigned int m_height= 0;

	GLuint TextureFromFile(const std::string& fullPath);

public:
	Texture2D(const std::string& fullPath, const std::string& typeName);
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
};

