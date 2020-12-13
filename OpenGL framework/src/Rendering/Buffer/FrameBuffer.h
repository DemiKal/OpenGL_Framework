#pragma once
#include "GameObject/Components/Texture2D.h"
struct FrameBufferSpecs
{
	entt::hashed_string Name;
	uint32_t Width{1024};
	uint32_t Height{768};
	GLenum Type = GL_RGBA;
	GLenum Depth = GL_DEPTH_COMPONENT24;
};

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
	FrameBuffer(uint32_t width = SCREENWIDTH, uint32_t height = SCREENHEIGHT);
	FrameBuffer(const FrameBufferSpecs& framebufferSpecs);

	void Init(uint32_t width, uint32_t height);

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


