#pragma once
class Texture2D
{
 private:
	unsigned int m_rendererID;
	std::string type;
	std::string path;

	GLuint TextureFromFile(const std::string& fullpath);

public:
	Texture2D(const std::string& fullPath, const std::string& typeName);
	~Texture2D() = default;
	 

	inline unsigned int GetID()const { return m_rendererID; }
	inline  std::string GetType() const { return type; }
	inline  std::string GetPath() const { return path; }
};

