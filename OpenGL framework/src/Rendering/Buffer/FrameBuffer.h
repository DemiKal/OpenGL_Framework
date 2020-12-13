#pragma once
#include "GameObject/Components/Texture2D.h"
struct FrameBufferSpecs
{
	entt::hashed_string Name;
	uint32_t Width{ SCREENWIDTH };
	uint32_t Height{ SCREENHEIGHT };
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
	entt::hashed_string m_Name;

public: //TODO ADD COPY CONSTRUCTOR!
	//FrameBuffer(uint32_t width = SCREENWIDTH, uint32_t height = SCREENHEIGHT);
	FrameBuffer(const FrameBufferSpecs& framebufferSpecs);

	void Init(uint32_t width, uint32_t height);
	entt::hashed_string GetName() const;
	bool Resize(uint32_t width, uint32_t height);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer& fb) = delete;
	FrameBuffer& operator= (const FrameBuffer& other) = delete;
	FrameBuffer(FrameBuffer&& a) = default;// {/*code*/ }

	//{
	//	m_RendererID = other.m_RendererID;
	//	m_RBO = other.m_RBO;
	//	m_Width = other.m_Width;
	//	m_Height = other.m_Height;
	//	m_RenderTarget = other.m_RenderTarget;
	//	m_DepthTexture = other.m_DepthTexture;
	//	m_Name = other.m_Name;
	//	return {};
	//}



	const Texture2D& GetTexture() const;
	const Texture2D& GetDepthTexture() const;
	unsigned int GetID() const;
	static void Unbind();
	void Bind() const;
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	//operator glm::vec2() const { return glm::vec2(x, y); }                               \


	[[nodiscard]] glm::ivec2 GetSize() const;
	[[nodiscard]] std::tuple<uint32_t, uint32_t> GetSize2() const;
	[[nodiscard]] float GetAspect() const;

};


