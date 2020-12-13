#include "DebugRenderLayer.h"
#include "Editor.h"
#include "RenderLayer.h"
#include "Rendering/Renderer.h"



DebugRenderLayer::DebugRenderLayer(std::shared_ptr<EditorLayer>& edl)
	:
	Layer("Debug Render Layer"),
	m_EditorLayer(edl)
{

}

void DebugRenderLayer::OnAttach()
{
}

void DebugRenderLayer::OnDetach()
{
}

void DebugRenderLayer::DrawCameraFrustum()
{
	//auto* rl = m_Editor->GetLayer<RenderLayer>();
	//m_EditorLayer->m_SceneFrame.Bind();
	//rl->m_FrameBuffers[0].Bind();

	//Renderer::SetAlphaBlending(true);
	//Renderer::DisableDepth();
	//Renderer::SetClearColor(0.10f, 0.5f, 0.5f, 0.0f);
	//Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//rl->m_FrameBuffers[0].Unbind();
	//m_EditorLayer->m_SceneFrame.Unbind();
}

void DebugRenderLayer::OnUpdate(float dt)
{
	DrawCameraFrustum();

}

void DebugRenderLayer::OnImGuiRender(float dt)
{
	ImGui::Begin("Debug Render Layer");
	Renderer& renderer = m_Editor->GetRenderer();
	  bool vsync = renderer.GetVSync();
	ImGui::Checkbox("Vsync", &vsync);
	renderer.SetVSync(vsync);
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	//if (ImGui::TreeNode("Camera frustum debug"))
	//{
	//	auto* const texId = reinterpret_cast<void*>(static_cast<intptr_t>(m_FrameBuffer->GetTexture().GetID()));
	//	ImGui::Image(texId, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	//	ImGui::TreePop();
	//}
	ImGui::End();


	//ImGui::Begin("Scene", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration);

	//ImGui::End();

}
