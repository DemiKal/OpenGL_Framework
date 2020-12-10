#pragma once
#include "Core/Layer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/Buffer/FrameBuffer.h"
#include "Gizmos/Gizmo.h"

namespace meme {
	class Editor;
}
class Gizmo;

class EditorLayer : public Layer
{

public:
	meme::Editor* m_Editor;
	entt::registry m_Registry;
	entt::entity m_Selected{ entt::null };
	Camera m_EditorCamera;
	FrameBuffer m_SceneFrame;
	ImVec2 m_ImGuiRegionSize;
	ImGuizmo::OPERATION m_TransformWidgetOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE m_TransformWidgetMode = ImGuizmo::MODE::LOCAL;
	std::vector<Gizmo*> m_Gizmos;
	Texture2D m_Skybox;

	EditorLayer(meme::Editor* editor);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;
	void DrawDebugVisuals(float dt);
	void RenderSkybox();
	void DrawScene(const float dt);
	void DrawGizmos(const float dt);
	void DrawCameraInspector(float dt);
	void OnInput(const float dt);
	void EnableDockSpace();
	//void DrawMenuBar();
	Camera& GetEditorCamera();

	void DrawGizmoMenu();




	void RenderViewportPanel(float dt);
	void RenderInspectorPanel(float dt);
	void RenderSceneHierarchyPanel(float dt);
	
	template<typename T>
	void DrawUIComponent(T t);
	void DrawVec3Component(const std::string& label, glm::vec3& vec, float resetVal);
	void DrawUIComponent(TransformComponent& t, const std::string& label, const float dt);
	void DrawUIComponent(const glm::mat4& m, const std::string& label, const float dt);
	void DrawUIComponent(CameraComponent& cc, const std::string& label, entt::entity entity, const float dt);
	void DrawUIComponent(MeshComponent& mc, const std::string& label, const float dt);
};
