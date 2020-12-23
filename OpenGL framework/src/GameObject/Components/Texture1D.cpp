#include "precomp.h"
#include "Texture1D.h"



Texture1D::Texture1D()
	: m_rendererID(0), m_width(0)
{

}

unsigned Texture1D::GetID() const
{
	return m_rendererID;
}

unsigned Texture1D::GetWidth() const
{
	return m_width;
}

std::string Texture1D::GetName() const
{
	return m_name;
}

void Texture1D::Bind(const unsigned slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_1D, m_rendererID);
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
	return {};
}

Texture1D::Texture1D(const uint32_t width, const uint32_t channels,
	dataType dataType, void* data, bool normalized = false)
	:
	m_rendererID(0), m_width(width)
{
	//	GLenum type1 = datatype == dataType::INT32 ? GL_RGBA32I : GL_RGBA32F;
	//	GLenum type2 = datatype == dataType::INT32 ? GL_RGBA_INTEGER : GL_RGB;
	//	GLenum type3 = datatype == dataType::INT32 ? GL_INT :					 GL_FLOAT;

	auto [type1, type2, type3] = GetType(dataType, channels);

	const uint32_t maxsize = 2u << 13u;
	const uint32_t maxsize2 = 2u >> 14u;
	const uint32_t actualWidth = std::min(width, maxsize);

	unsigned int renderer_id = 0;
	glGenTextures(1, &renderer_id);
	glBindTexture(GL_TEXTURE_1D, renderer_id);
	glTexImage1D(GL_TEXTURE_1D, 0, type1, actualWidth, 0,
		type2, type3, data); //TODO: fix non-normalized ints!

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	m_rendererID = renderer_id;
	//type = typeName;
	//path = fullPath;


}



