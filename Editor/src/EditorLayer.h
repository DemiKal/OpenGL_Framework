#pragma once
#include "Core/layer.h"
#include "GameObject/Camera.h"

namespace meme {
	class Editor;
}

class EditorLayer : public Layer
{
public:

	entt::registry m_Registry;
	meme::Editor* m_Editor;
	Camera m_EditorCamera;

	EditorLayer(meme::Editor* editor) : Layer("EditorLayer"), m_Editor(editor)
	{

	}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	void OnImGuiRender() override;
	void OnInput();
	void EnableDockSpace();
	//void DrawMenuBar();
	void DrawEntityPanel();
	void DrawInspectorPanel();
	//void DrawEntityComponent(TransformComponent& tfc);
	
};