#pragma once
#include "GameObject/Components/Texture2D.h"

class FrameBuffer
{
private:
	unsigned int m_rendererID;
	unsigned int m_RBO;
	Texture2D m_renderTarget;
	Texture2D m_depthTexture;
	

public:
	explicit FrameBuffer(const unsigned int width = SCREENWIDTH, const unsigned int height = SCREENHEIGHT);

	Texture2D& GetTexture() { return m_renderTarget; }
	unsigned int GetID() { return m_rendererID; }
	void Bind() const { GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID)); }
	static void Unbind() { GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0)); }
};


