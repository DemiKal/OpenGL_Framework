#pragma once
#include "EditorLayer.h"
#include "Rendering/Buffer/FrameBuffer.h"

class EditorLayer;

class RenderLayer final : public Layer
{
public:
	std::shared_ptr<EditorLayer> m_EditorLayer;
	//std::vector<FrameBuffer> m_FrameBuffers;
	FrameBuffer* m_FramebufferCamera;
	ImVec2 m_ImGuiRegionSize;

	RenderLayer(std::shared_ptr<EditorLayer> edl);
	void OnAttach() override;
	void OnDetach() override;
	void OnImGuiRender(float dt) override;
	void OnUpdate(float dt) override;
	void RenderCamera() const;
};