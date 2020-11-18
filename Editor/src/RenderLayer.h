#pragma once
#include "EditorLayer.h"
#include "Core/Layer.h"

class RenderLayer : public Layer
{
public:
	EditorLayer* m_Editor;

	RenderLayer(std::shared_ptr<EditorLayer> edl);
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnImGuiRender();
};

