#include "FrustumGizmo.h"

//class Camera ;

  FrustumGizmo::FrustumGizmo(EditorLayer* edl, const std::string& name)
	: Gizmo(edl, name)
{
}

void FrustumGizmo::Draw()
{
	Renderer::SetAlphaBlending(true);
	for (auto entity : m_Edl->m_Registry.view<CameraComponent>())
	{
		auto debugCam = m_Edl->m_Registry.get<CameraComponent>(entity).camera;
		Renderer::DrawFrustum(m_Edl->GetEditorCamera(), debugCam, m_Color);
	}

	//Renderer::DrawFrustum(*m_EditorCam, *m_DummyCam, m_Color);
}
