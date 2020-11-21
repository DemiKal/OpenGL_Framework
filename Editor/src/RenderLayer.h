#pragma once
#include "EditorLayer.h"
#include "Core/Layer.h"
#include "Rendering/Buffer/FrameBuffer.h"

class RenderLayer : public Layer
{
public:
	EditorLayer* m_EditorLayer;
	FrameBuffer* fb;

	RenderLayer(std::shared_ptr<EditorLayer> edl);
	virtual void OnAttach() override;
	virtual void OnDetach() override; 
	virtual void OnUpdate(float dt) override;
	virtual void OnImGuiRender(float dt);
	 
};

