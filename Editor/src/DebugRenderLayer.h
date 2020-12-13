#pragma once
#include "EditorLayer.h"
#include "Core/Layer.h"
#include "Rendering/Buffer/FrameBuffer.h"

class DebugRenderLayer : public Layer
{
public:
	DebugRenderLayer(std::shared_ptr<EditorLayer>& edl);
	void OnAttach() override;
	void OnDetach() override;
	void DrawCameraFrustum();
	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;

private:
	FrameBuffer* m_FrameBuffer;
	std::shared_ptr<EditorLayer> m_EditorLayer;
};

