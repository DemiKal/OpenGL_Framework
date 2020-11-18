#pragma once
#include "Core/layer.h"

class EditorLayer : public Layer
{
public:
	EditorLayer() : Layer("EditorLayer")
	{

	}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;

	virtual void OnImGuiRender();
};