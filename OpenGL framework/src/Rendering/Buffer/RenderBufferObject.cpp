#include "precomp.h"
#include "RenderBufferObject.h"

RenderBufferObject::RenderBufferObject() : m_renderID(0)
{
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREENWIDTH, SCREENHEIGHT);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	// now actually attach it
	m_renderID = rbo;
}

void RenderBufferObject::Bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_renderID);
}

unsigned int RenderBufferObject::GetRendererID() const
{
	return m_renderID;
}
