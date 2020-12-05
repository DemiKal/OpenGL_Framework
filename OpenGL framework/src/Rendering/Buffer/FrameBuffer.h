#pragma once
#include "GameObject/Components/Texture2D.h"

class FrameBuffer
{
private:
	uint32_t m_RendererID;
	uint32_t m_RBO;
	uint32_t m_Width;
	uint32_t m_Height;
	Texture2D m_RenderTarget;
	Texture2D m_DepthTexture;

public: //TODO ADD COPY CONSTRUCTOR!
	void Init(uint32_t width, uint32_t height);
	FrameBuffer(unsigned int width = SCREENWIDTH, unsigned int height = SCREENHEIGHT);
	bool Resize(uint32_t width, uint32_t height);
	~FrameBuffer();
	Texture2D& GetTexture();
	Texture2D& GetDepthTexture();
	unsigned int GetID() const;
	static void Unbind();
	void Bind() const;
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	[[nodiscard]] std::tuple<uint32_t, uint32_t> GetSize() const;
	[[nodiscard]] float GetAspect() const;

};


