#include "precomp.h"
#include "Texture1D.h"



Texture1D::Texture1D()
	: m_rendererID(0), m_width(0)
{

}

std::tuple<GLenum, GLenum, GLenum> GetType(dataType datatype, const unsigned int channels)
{
	if (datatype == dataType::INT32)
	{
		if (channels == 1) return std::make_tuple(GL_R32I, GL_RED_INTEGER, GL_INT);
		if (channels == 2) return std::make_tuple(GL_RG32I, GL_RG_INTEGER, GL_INT);
		if (channels == 3) return std::make_tuple(GL_RGB32I, GL_RGB_INTEGER, GL_INT);
		if (channels == 4) return std::make_tuple(GL_RGBA32I, GL_RGBA_INTEGER, GL_INT);
	}
	if (datatype == dataType::UINT32)
	{
		if (channels == 1) return std::make_tuple(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT);
		if (channels == 2) return std::make_tuple(GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT);
		if (channels == 3) return std::make_tuple(GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
		if (channels == 4) return std::make_tuple(GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
	}

	if (datatype == dataType::FLOAT32)
	{
		if (channels == 1) return std::make_tuple(GL_R32F, GL_RED, GL_FLOAT);
		if (channels == 2) return std::make_tuple(GL_RG32F, GL_RG, GL_FLOAT);
		if (channels == 3) return std::make_tuple(GL_RGB32F, GL_RGB, GL_FLOAT);
		if (channels == 4) return std::make_tuple(GL_RGBA32F, GL_RGBA, GL_FLOAT);
	}

	ASSERT(false);

}

Texture1D::Texture1D(const unsigned int width, const unsigned int channels,
	dataType datatype, void* data, bool normalized = false)
	:
	m_rendererID(0), m_width(width)
{
	//	GLenum type1 = datatype == dataType::INT32 ? GL_RGBA32I : GL_RGBA32F;
	//	GLenum type2 = datatype == dataType::INT32 ? GL_RGBA_INTEGER : GL_RGB;
	//	GLenum type3 = datatype == dataType::INT32 ? GL_INT :					 GL_FLOAT;

	auto [type1, type2, type3] = GetType(datatype, channels);

	unsigned int maxsize = 2u << 13u;
	unsigned int maxsize2 = 2u >> 14u;
	 const unsigned int actualWidth = std::min(width, maxsize);

	unsigned int renderer_id = 0;
	GLCall(glGenTextures(1, &renderer_id));
	GLCall(glBindTexture(GL_TEXTURE_1D, renderer_id));
	GLCall(glTexImage1D(GL_TEXTURE_1D, 0, type1, actualWidth, 0,
		type2, type3, data)); //fix nonnormalized ints!

	GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP));
	GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP));
	GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));


	m_rendererID = renderer_id;
	//type = typeName;
	//path = fullPath;


}



