#pragma once
#include "EditorLayer.h"
#include "Rendering/Buffer/FrameBuffer.h"

class RenderLayer final : public Layer
{
public:
	std::shared_ptr<EditorLayer> m_EditorLayer;
	//std::vector<FrameBuffer> m_FrameBuffers;
	FrameBuffer m_FramebufferCamera;
	ImVec2 m_ImGuiRegionSize;
	//Texture2D m_Skybox;
	
	RenderLayer(std::shared_ptr<EditorLayer> edl);
	void OnAttach() override;
	void OnDetach() override;
	//void RenderSkybox();
	void RenderCamera() const;
	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;
};

