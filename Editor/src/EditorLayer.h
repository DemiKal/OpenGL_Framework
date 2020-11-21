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

	EditorLayer(meme::Editor* editor) :
		Layer("EditorLayer"),
		m_Editor(editor),
		m_EditorCamera(glm::vec3(0, 3, 16), 70, float(SCREENWIDTH) / float(SCREENHEIGHT), 0.1f, 700.0f)
	{

	}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	void OnImGuiRender() override;
	void DrawCameraInspector();
	void OnInput(const float dt);
	void EnableDockSpace();
	//void DrawMenuBar();
	void DrawEntityPanel();
	void DrawInspectorPanel();
	Camera& GetEditorCamera();
	//void DrawEntityComponent(TransformComponent& tfc);

};
