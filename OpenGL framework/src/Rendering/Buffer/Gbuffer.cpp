#include "precomp.h"
#include "Gbuffer.h"

#include "GameObject/Components/Texture2D.h"

Gbuffer::Gbuffer(const unsigned int width, const unsigned int height)
	:
	m_bufferID(0),
	m_gPosition(0),
	m_gNormal(0),
	m_gAlbedoSpec(0),
	m_RBO(0)	//depth 
{
	GLCall(glGenFramebuffers(1, &m_bufferID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID));

	GLCall(glGenTextures(1, &m_gPosition));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_gPosition));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0));
	// normal color buffer
	GLCall(glGenTextures(1, &m_gNormal));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_gNormal));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0));
	// color + specular color buffer
	GLCall(glGenTextures(1, &m_gAlbedoSpec));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0));
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	GLCall(glDrawBuffers(3, attachments));

	GLCall(glGenRenderbuffers(1, &m_RBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RBO));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO));
	// finally check if framebuffer is complete
	GLCall(if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE))
		fmt::print("Framebuffer not complete!\n");

	const Texture2D zBufferTexPosition(
		GL_DEPTH_COMPONENT24,
		width,
		height,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT);

	m_zBufferTexID = zBufferTexPosition.GetID();
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBufferTexPosition.GetID(), 0));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

uint32_t Gbuffer::GetZBufferTexID() const
{
	return m_zBufferTexID;
}
unsigned int Gbuffer::GetID() const
{
	return m_bufferID;
}
unsigned int Gbuffer::GetPositionID() const
{
	return m_gPosition;
}
unsigned int Gbuffer::GetNormalID() const
{
	return m_gNormal;
}
unsigned int Gbuffer::GetAlbedoSpecID() const
{
	return m_gAlbedoSpec;
}

unsigned Gbuffer::GetRBO_ID() const
{
	return m_RBO;
}

inline void Gbuffer::Enable()
{
	
}

void Gbuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);

}
