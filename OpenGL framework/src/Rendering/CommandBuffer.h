#pragma once
#include "Buffer/FrameBuffer.h"
struct RenderCommand
{
	entt::hashed_string FrameBufferIndex;
	entt::hashed_string ShaderIndex; //or entt::string?
	 
};

class CommandBuffer
{
	//TODO: use a unordered map?
	std::vector<FrameBuffer> m_FrameBuffers;

	std::vector<RenderCommand> m_DeferredRenderCommands;
	std::vector<RenderCommand> m_ForwardRenderCommands;
	std::vector<RenderCommand> m_AlphaRenderCommands;
	std::vector<RenderCommand> m_PostProcessingRenderCommands;
	std::vector<RenderCommand> m_CustomRenderCommands; //possibly for compute shaders n stuff
public:
	CommandBuffer(){};
	FrameBuffer& GenerateFrameBuffer(const FrameBufferSpecs& framebufferSpecs);
	FrameBuffer& GetFrameBuffer(uint32_t index);
};

