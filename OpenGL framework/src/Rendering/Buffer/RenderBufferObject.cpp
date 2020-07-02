#include "precomp.h"
#include "RenderBufferObject.h"

RenderBufferObject::RenderBufferObject() : m_renderID(0)
{
	unsigned int rbo;
	GLCall(glGenRenderbuffers(1, &rbo));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH, SCREENHEIGHT));

	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo));
	// now actually attach it
	m_renderID = rbo;
}

void RenderBufferObject::Bind() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_renderID));
}

unsigned int RenderBufferObject::GetRendererID() const
{
	return m_renderID;
}
