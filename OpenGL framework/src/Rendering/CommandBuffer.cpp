#include "precomp.h"
#include "CommandBuffer.h"


CommandBuffer::CommandBuffer()
{
}

FrameBuffer& CommandBuffer::GenerateFrameBuffer(const FrameBufferSpecs& framebufferSpecs)
{
	
	 m_FrameBuffers.emplace_back(framebufferSpecs);
	return m_FrameBuffers.back();
}

FrameBuffer& CommandBuffer::GetFrameBuffer(uint32_t index)
{
	return m_FrameBuffers[index];
}

FrameBuffer& CommandBuffer::GetFrameBuffer(entt::hashed_string name)
{
	for(FrameBuffer& fb : m_FrameBuffers)
	{
		auto nm = fb.GetName();
		//auto comp = ;
		//auto comp2 = strcmp(nm, name);

		if (nm == name)
			return fb;
		
	}
}

const std::vector<FrameBuffer>& CommandBuffer::GetFrameBuffers() const
{
	return m_FrameBuffers;
}
