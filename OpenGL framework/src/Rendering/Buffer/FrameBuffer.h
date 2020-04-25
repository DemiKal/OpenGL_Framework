#pragma once
class FrameBuffer
{
private:
	unsigned int m_rendererID;

public:
	FrameBuffer() : m_rendererID(0)
	{
		unsigned int fb;
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		m_rendererID = fb;
	}
	

	void Bind() const { GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID)); }
};

