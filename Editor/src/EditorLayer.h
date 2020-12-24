#pragma once
#include "Core/Layer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "Geometry/BVH/TopLevelBVH.h"

namespace meme {
	class Editor;
}

//class Gizmo;
class Camera;

class EditorLayer : public Layer
{

public:
	meme::Editor* m_Editor;
	entt::registry m_Registry;
	entt::entity m_Selected{ entt::null };
	Camera m_EditorCamera;
	ImVec2 m_ImGuiRegionSize;
	ImVec2 m_ViewportClickPos;
	bool m_ClickedOnViewport= false;
	bool m_ViewportHovered = false;
	bool m_TransformGizmoActive = false;
	
	ImGuizmo::OPERATION m_TransformWidgetOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE m_TransformWidgetMode = ImGuizmo::MODE::LOCAL;
	//std::vector<Gizmo*> m_Gizmos;
	std::unique_ptr<TopLevelBVH> m_TopLevelBVH = std::make_unique< TopLevelBVH>();
	EditorLayer(meme::Editor* editor);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;
	void DrawDebugVisuals(float dt);
	void RenderSkybox();
	void RenderScene(float dt);
	void DrawGizmos(float dt);
	void DrawCameraInspector(float dt);
	void OnInput(float dt);
	void EnableDockSpace();
	Camera& GetEditorCamera();

	void DrawGizmoMenu();

	void RenderViewportPanel(float dt);
	void RenderInspectorPanel(float dt);
	void RenderSceneHierarchyPanel(float dt);
};
