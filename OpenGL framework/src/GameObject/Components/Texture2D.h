#pragma once
class Texture2D
{
private:
	unsigned int m_rendererID;
	std::string type;
	std::string path;
	unsigned int width;
	unsigned int height;
	GLuint TextureFromFile(const std::string& fullpath);

public:
	Texture2D(const std::string& fullPath, const std::string& typeName);
	Texture2D(const GLenum internalformat, 
		const int p_width,  //TODO: unsigned?
		const int p_height,
		const int border,
		const GLenum format, 
		const GLenum type, 
		const GLenum minFilter,
		const GLenum magFilter,
		const GLenum wrap_S,
		const GLenum wrap_T,
		const void* data);
	
	~Texture2D() = default;

	inline unsigned int GetID()const { return m_rendererID; }
	inline  std::string GetType() const { return type; }
	inline  std::string GetPath() const { return path; }
	inline unsigned int GetWidth()  const { return width; }
	inline unsigned int GetHeight()  const { return height; }
	inline void Bind() { throw new std::exception(); }
};

