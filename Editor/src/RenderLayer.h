#pragma once
#include "EditorLayer.h"
#include "Rendering/Buffer/FrameBuffer.h"

class RenderLayer : public Layer
{
public:
	std::shared_ptr<EditorLayer> m_EditorLayer;
	std::vector<FrameBuffer> m_FrameBuffers;
	FrameBuffer m_FramebufferCamera;
	ImVec2 m_ImGuiRegionSize;

	RenderLayer(std::shared_ptr<EditorLayer> edl);
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void RenderCamera();
	virtual void OnUpdate(float dt) override;
	virtual void OnImGuiRender(float dt);
};

