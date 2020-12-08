#include "FrustumGizmo.h"
#include "Editor.h"

//class Camera ;

  FrustumGizmo::FrustumGizmo(EditorLayer* edl, const std::string& name)
	: Gizmo(edl, name)
{
}

void FrustumGizmo::Draw()
{
	Renderer& renderer = m_Edl->m_Editor->GetRenderer();
	renderer.SetAlphaBlending(true);
	for (auto entity : m_Edl->m_Registry.view<CameraComponent>())
	{
		auto debugCam = m_Edl->m_Registry.get<CameraComponent>(entity).camera;
		renderer.DrawFrustum(m_Edl->GetEditorCamera(), debugCam, m_Color);
	}

	//Renderer::DrawFrustum(*m_EditorCam, *m_DummyCam, m_Color);
}
