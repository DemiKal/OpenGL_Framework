#pragma once
class Gbuffer
{
private:
	unsigned int m_bufferID;
	unsigned int m_gPosition;
	unsigned int m_gNormal;
	unsigned int m_gAlbedoSpec;
	unsigned int m_RBO; //depth buffer

public:
	Gbuffer(unsigned int width = SCREENWIDTH, unsigned int height = SCREENHEIGHT);
	unsigned int GetID() const;
	void Bind() const;
	[[nodiscard]] unsigned int GetPositionID() const;
	[[nodiscard]] unsigned int GetNormalID() const;
	[[nodiscard]] unsigned int GetAlbedoSpecID() const;

	void Enable();
};

