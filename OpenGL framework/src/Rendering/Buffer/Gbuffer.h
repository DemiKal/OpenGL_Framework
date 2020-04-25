#pragma once
class Gbuffer
{
private:
	unsigned int m_bufferID;
	unsigned int m_gPosition;
	unsigned int m_gNormal;
	unsigned int m_gAlbedoSpec;

public:
	Gbuffer();
	unsigned int GetID();
	void Bind();
	unsigned int GetPositionID();
	unsigned int GetNormalID();
	unsigned int GetAlbedoSpecID();
};

