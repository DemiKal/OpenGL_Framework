#pragma once
class Texture2D
{
public:
 
	unsigned int m_rendererID;
	std::string type;
	std::string path;


	Texture2D(const std::string& fullPath, const std::string& typeName);
	~Texture2D();

	inline unsigned int GetID()const { return m_rendererID; }
};

