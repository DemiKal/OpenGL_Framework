#include "precomp.h"
#include "FrameBuffer.h"


float FrameBuffer::GetAspect() const
{
	return static_cast<float>(m_Width) / static_cast<float>(m_Height);
}

void FrameBuffer::Init(const uint32_t width, const uint32_t height)
{
	GLCall(glGenFramebuffers(1, &m_RendererID))
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID))
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RenderTarget.GetID(), 0))

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLCall(glGenRenderbuffers(1, &m_RBO))
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RBO))
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height))
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO))

		//// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Render buffer object (RBO) not complete!" << std::endl;

	////bind color and depth to framebuffer
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture.GetID(), 0))

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	m_Width = width;
	m_Height = height;
}
FrameBuffer::FrameBuffer(const unsigned width, const unsigned height)
	:
	m_RendererID(0),
	m_RBO(0),
	m_RenderTarget(Texture2D(GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE)),
	m_DepthTexture(Texture2D(GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER))
{
	Init(width, height);

	//unsigned int framebuffer;
	//glGenFramebuffers(1, &framebuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	//m_RendererID = framebuffer;
	//
	//// create a color attachment texture
	//unsigned int textureColorbuffer;
	//glGenTextures(1, &textureColorbuffer);
	//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0  , GL_TEXTURE_2D, textureColorbuffer, 0);
	//m_colorTextureID = textureColorbuffer;
	//
	//// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	//// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//
	//m_RBO = rbo;
}

bool FrameBuffer::Resize(uint32_t width, uint32_t height)
{
	if ((width <= 0 || height <= 0) && m_RendererID) return false;
	//if (m_RendererID)

	glDeleteFramebuffers(1, &m_RendererID);

	m_RenderTarget.Delete();
	m_DepthTexture.Delete();

	m_RenderTarget = Texture2D(GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE);
	m_DepthTexture = Texture2D(GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

	Init(width, height);
	return true;
}

FrameBuffer::~FrameBuffer()
{

}

Texture2D& FrameBuffer::GetTexture()
{
	return m_RenderTarget;
}

Texture2D& FrameBuffer::GetDepthTexture()
{
	return m_DepthTexture;
}

unsigned FrameBuffer::GetID() const
{
	return m_RendererID;
}

void FrameBuffer::Bind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID))
}

uint32_t FrameBuffer::GetWidth() const
{
	return m_Width;
}

uint32_t FrameBuffer::GetHeight() const
{
	return m_Height;
}

std::tuple<uint32_t, uint32_t>  FrameBuffer::GetSize() const
{
	return { m_Width, m_Height };
}

void FrameBuffer::Unbind()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0))
}
