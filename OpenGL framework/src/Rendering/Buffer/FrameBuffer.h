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

	Texture2D& GetTexture();
	unsigned int GetID() const;
	void Bind() const;
	static void Unbind();
};


