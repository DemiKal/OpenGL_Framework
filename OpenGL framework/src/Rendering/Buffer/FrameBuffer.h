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
	FrameBuffer(unsigned int width = SCREENWIDTH, unsigned int height = SCREENHEIGHT);
	~FrameBuffer();
	Texture2D& GetTexture();
	Texture2D& GetDepthTexture();
	unsigned int GetID() const;
	void Bind() const;
	static void Unbind();
};


