#pragma once
class Texture2D
{
private:
	unsigned int m_rendererID{};
	std::string m_type;
	std::string m_path;
	unsigned int m_width{};
	unsigned int m_height{};
	GLuint TextureFromFile(const std::string& fullpath);

public:
	Texture2D(const std::string& fullPath, const std::string& typeName);
	Texture2D(
		const GLenum internalformat,
		const unsigned int p_width,  
		const unsigned int p_height,
		const unsigned int border,
		const GLenum format,
		const GLenum type,
		const void* data = nullptr,
		const GLenum minFilter = GL_LINEAR,
		const GLenum magFilter = GL_LINEAR,
		const GLenum wrap_S = GL_CLAMP_TO_EDGE,
		const GLenum wrap_T = GL_CLAMP_TO_EDGE);

	~Texture2D() = default;

	inline unsigned int GetID()const { return m_rendererID; }
	inline std::string GetType() const { return m_type; }
	inline std::string GetPath() const { return m_path; }
	inline unsigned int GetWidth()  const { return m_width; }
	inline unsigned int GetHeight()  const { return m_height; }
	inline void Bind(const int textureIndex = 0) const
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + textureIndex));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_rendererID));
	}
};

