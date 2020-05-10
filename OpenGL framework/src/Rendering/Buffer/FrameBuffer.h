#pragma once
#include "GameObject/Components/Texture2D.h"

class FrameBuffer
{
private:
	unsigned int m_rendererID;
	unsigned int m_RBO;
	//unsigned int m_colorTextureID;
	Texture2D m_renderTarget;
	Texture2D m_depthTexture;
	

public:
	explicit FrameBuffer(const unsigned int width = SCREENWIDTH, const unsigned int height = SCREENHEIGHT);

	//nsigned int GetColorTextureID() { return m_colorTextureID; }
	Texture2D& GetTexture() { return m_renderTarget; }

	void Bind() const { GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID)); }
};


