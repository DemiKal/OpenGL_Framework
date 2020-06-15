#pragma once
#include "GameObject/Components/Texture2D.h"

class Gbuffer
{
private:
	unsigned int m_bufferID;
	unsigned int m_gPosition;
	unsigned int m_gNormal;
	unsigned int m_gAlbedoSpec;
	unsigned int m_RBO; //depth buffer
	uint32_t m_zBufferTexID;

public:
	Gbuffer(unsigned int width = SCREENWIDTH, unsigned int height = SCREENHEIGHT);
	unsigned int GetID() const;
	void Bind() const;
	[[nodiscard]] unsigned int GetPositionID() const;
	[[nodiscard]] unsigned int GetNormalID() const;
	[[nodiscard]] unsigned int GetAlbedoSpecID() const;
	[[nodiscard]] unsigned int GetRBO_ID() const;
	[[nodiscard]] uint32_t GetZBufferTexID() const;

	void Enable();
};
