#pragma once

class Texture
{
public:
	enum TextureType {
		DIFFUSE,
		SPECULAR,
		NORMALMAP,
		HEIGHTMAP
	};

 
public:
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	TextureType type;

	std::string m_FilePath;
	unsigned int m_RendererID;




	Texture();
	Texture(const std::string& path, const TextureType  texType);
	~Texture();
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline unsigned int GetWidth() const { return m_Width; }
	inline unsigned int GetHeight() const { return m_Height; }
	inline std::string GetTypeName()  const
	{
		switch (type) {
		case  DIFFUSE:	return "texture_diffuse";
		case  SPECULAR: return "texture_specular";
		case  NORMALMAP: return "texture_normal";
		case  HEIGHTMAP: return "texture_height";
			return  "ERROR";
		}
		return {};
	}

	static TextureType GetType(const std::string & name)
	{
		if (name == "texture_diffuse") return DIFFUSE;
		else if (name == "texture_specular")return  SPECULAR;
		else if (name == "texture_normal")return NORMALMAP;
		else if (name == "texture_height")return HEIGHTMAP;

		ASSERT(false);
	}


	inline unsigned int GetID()  const { return m_RendererID; }
};

