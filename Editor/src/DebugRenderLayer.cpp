#include "DebugRenderLayer.h"

#include "Rendering/Renderer.h"

DebugRenderLayer::DebugRenderLayer(std::shared_ptr<EditorLayer> edl)
	: Layer("Debug Render Layer"),
	m_EditorLayer(edl)
{
}

void DebugRenderLayer::OnAttach()
{
}

void DebugRenderLayer::OnDetach()
{
}

void DebugRenderLayer::OnUpdate(float dt)
{
	m_FrameBuffer.Bind();
	Renderer::SetAlphaBlending(true);
	Renderer::SetClearColor(0.10f, 0.5f, 0.5f, 0.1f);
	Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto entity : m_EditorLayer->m_Registry.view<CameraComponent>())
	{
		auto debugCam = m_EditorLayer->m_Registry.get<CameraComponent>(entity).camera;
		Renderer::DrawFrustum(m_EditorLayer->GetEditorCamera(), debugCam, { 1.0f, 0.5f, 0.3f, 0.99f });
	}
	m_FrameBuffer.Unbind();
}

void DebugRenderLayer::OnImGuiRender(float dt)
{
	ImGui::Begin("Debug Render Layer");
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Camera frustum debug")) 
	{
		auto* const texId = reinterpret_cast<void*>(static_cast<intptr_t>(m_FrameBuffer.GetTexture().GetID()));
		ImGui::Image(texId, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::TreePop();
	}
	ImGui::End();

}
