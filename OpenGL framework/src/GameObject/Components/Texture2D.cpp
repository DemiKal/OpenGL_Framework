#include "precomp.h"
#include "Texture2D.h"

GLuint Texture2D::TextureFromFile(const std::string& fullpath/*, Texture& texture*/)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(fullpath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		ASSERT(nrComponents > 0);
		GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D));

		this->m_width = width;
		this->m_height = height;
		// for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. 
		//Due to interpolation it takes texels from next repeat 
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	else
	{
		std::cout << "Texture failed to load at path: " << fullpath << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

Texture2D::Texture2D(const std::string& fullPath, const std::string& typeName)
{
	m_rendererID = TextureFromFile(fullPath);
	m_type = typeName;
	m_path = fullPath;
}

//create a texture directly not from a file
Texture2D::Texture2D(
	const GLenum internalFormat, 
	const unsigned int p_width, 
	const unsigned int p_height,
	const unsigned int border, 
	const GLenum format, 
	const GLenum type,
	const void* data,
	const GLenum minFilter,
	const GLenum magFilter, 
	const GLenum wrap_S,
	const GLenum wrap_T)
	:
	m_width(p_width), m_height(p_height)
{ 
	glGenTextures(1, &m_rendererID);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);
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
	
	glBindTexture(GL_TEXTURE_2D, 0);

}



