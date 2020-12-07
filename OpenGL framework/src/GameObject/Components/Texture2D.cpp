#include "precomp.h"
#include "Texture2D.h"

void Texture2D::GetEmbeddedTexture(const aiTexture* texture)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	unsigned char* data = stbi_load_from_memory(
		reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &nrComponents, 0);

	uint32_t texId = 0;
	if (data)
		GenerateTexture(data, nrComponents, width, height);
	else
	{
		fmt::print("could not generate texture from memory!");
	}

}

//from learnopengl
void Texture2D::LoadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;	//TODO: use components?
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			fmt::print("Cubemap texture failed to load at path: {}\n", faces[i]);
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	m_Width = width;
	m_Height= height;
	m_RendererID = textureID;
	m_type = "texture_diffuse";
	m_GLType = GL_TEXTURE_CUBE_MAP;
	//return textureID;
}

GLenum Texture2D::GetGLType() const
{
	return m_GLType;
}

void Texture2D::GenerateTexture(unsigned char* data, int nrComponents, int width, int height)
{

	GLenum format;
	unsigned int textureID = 0;
	if (nrComponents == 1)
		format = GL_RED;
	else if (nrComponents == 3)
		format = GL_RGB;
	else if (nrComponents == 4)
		format = GL_RGBA;

	ASSERT(nrComponents > 0);

	glGenTextures(1, &textureID);
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D));

	m_Width = width;
	m_Height = height;
	m_RendererID = textureID;	//TODO: careful this wont work when calling this func with Cubemap. Refactor?

	// for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. 
	//Due to interpolation it takes texels from next repeat 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

void Texture2D::TextureFromFile(const std::string& fullPath/*, Texture& texture*/)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GenerateTexture(data, nrComponents, width, height);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << fullPath << std::endl;
		stbi_image_free(data);

	}
}

Texture2D::Texture2D(const std::string& fullPath, const std::string& typeName)
{
	TextureFromFile(fullPath);
	m_type = typeName;
	m_path = fullPath;
}

Texture2D::Texture2D(const aiTexture* aitexture, const std::string& texType)
{
	GetEmbeddedTexture(aitexture); //set m_RendererID
	m_type = texType;
	m_path = "Initialized from memory";
}

//Create a texture directly, not from a file
Texture2D::Texture2D(
	const GLenum internalFormat,
	const unsigned int p_width,
	const unsigned int p_height,
	const unsigned int border,
	const GLenum format,
	const GLenum type,
	const GLenum minFilter,
	const GLenum magFilter,
	const GLenum wrap_S,
	const GLenum wrap_T,
	const void* data)
	:
	m_Width(p_width), m_Height(p_height)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glTexImage2D(GL_TEXTURE_2D, 0,
		internalFormat,
		SCREENWIDTH,
		SCREENHEIGHT,
		0,
		format,
		type,		//GL_Unsigned byte, gl_float etc.
		data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_T);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindTexture(GL_TEXTURE_2D, 0);

}

unsigned Texture2D::GetID() const
{
	return m_RendererID;
}

std::string Texture2D::GetType() const
{
	return m_type;
}

std::string Texture2D::GetPath() const
{
	return m_path;
}

unsigned Texture2D::GetWidth() const
{
	return m_Width;
}

unsigned Texture2D::GetHeight() const
{
	return m_Height;
}

void Texture2D::Bind(const int textureIndex) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + textureIndex));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture2D::Delete()
{
	glDeleteTextures(1, &m_RendererID);
}



