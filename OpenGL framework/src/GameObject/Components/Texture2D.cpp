#include "precomp.h"

GLuint Texture2D::TextureFromFile(const std::string& fullpath/*, Texture& texture*/)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(fullpath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		//GLenum format = -1;
		//if (nrComponents == 1)
		//	format = GL_RED;
		//else if (nrComponents == 3)
		//	format = GL_RGB;
		//else if (nrComponents == 4)
		//	format = GL_RGBA;
		//
		//ASSERT(format > 0);
		//
		//glBindTexture(GL_TEXTURE_2D, textureID);
		//glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		////glGenerateMipmap(GL_TEXTURE_2D);
		//
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//
		//stbi_image_free(data);
		GLenum format ;
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
	m_rendererID = TextureFromFile(fullPath );
	type = typeName;
	path = fullPath;
}



