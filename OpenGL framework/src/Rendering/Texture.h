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

private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	TextureType type;

public:




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
		case  DIFFUSE: return "texture_diffuse";
		case  SPECULAR: return "texture_specular";
		case  NORMALMAP: return "texture_normal";
		case  HEIGHTMAP: return "texture_height";
			return  "ERROR";
		}
		return {};
	}

	inline unsigned int GetID()  const { return m_RendererID; }
};

