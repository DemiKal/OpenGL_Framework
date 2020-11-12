#pragma once
#include "Core/layer.h"

class EditorLayer : public Layer
{
public:
	EditorLayer() : Layer("EditorLayer") 
	{

	}
	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(float dt) override;
	

	virtual void OnImGuiRender() {}
};

