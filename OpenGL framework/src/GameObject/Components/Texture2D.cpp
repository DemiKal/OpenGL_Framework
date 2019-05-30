#include "precomp.h"

GLuint TextureFromFile(const std::string& fullpath/*, Texture& texture*/)
{
	////Generate texture ID and load texture data 
	//std::string filename = std::string(path);
	//filename = directory + '/' + filename;
	//GLuint textureID;
	////glGenTextures(1, &textureID);
	//int width, height, mBPP;
	////unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	//unsigned char* image = stbi_load(path, &width, &height, &mBPP, 4);
	//stbi_set_flip_vertically_on_load(true);
	//
	//ASSERT(width > 0 && height > 0);
	//GLCall(glGenTextures(1, &textureID));
	//GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
	//GLCall(glPixelStorei(GL_PACK_ALIGNMENT, 1));
	//GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	//
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	//
	//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, image));
	//
	//GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	//
	//// Assign texture to ID
	////glBindTexture(GL_TEXTURE_2D, textureID);
	////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	////glGenerateMipmap(GL_TEXTURE_2D);
	//
	//// Parameters
	////GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	////GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	/////GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	////GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	////GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	////GLCall(glBindTexture(GL_TEXTURE_2D, 0));
   /////
	//
	//
	////SOIL_free_image_data(image);
	//if (image)
	//	stbi_image_free(image);
	//
	//texture.m_LocalBuffer = image;
	//return textureID;
	//std::string filename = std::string(path);
	//filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(fullpath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = -1;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		ASSERT(format > 0);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
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


Texture2D::~Texture2D()
{
}
