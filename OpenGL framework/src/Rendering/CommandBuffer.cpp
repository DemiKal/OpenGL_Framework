#include "precomp.h"
#include "CommandBuffer.h"
#include <vector>


FrameBuffer& CommandBuffer::GenerateFrameBuffer(const FrameBufferSpecs& framebufferSpecs)
{
	FrameBuffer fb(framebufferSpecs);
	m_FrameBuffers.emplace_back(fb);
	//return this or back of vector?
	return m_FrameBuffers.back();
}

FrameBuffer& CommandBuffer::GetFrameBuffer(uint32_t index)
{
	return m_FrameBuffers[index];
}
