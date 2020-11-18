#pragma once
#include "Core/layer.h"

class EditorLayer : public Layer
{
private:
	entt::registry m_Registry;


public:
	EditorLayer() : Layer("EditorLayer")
	{

	}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnImGuiRender();


};