#include "RenderLayer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"
#include <ImGuiManager.h>
#include <Rendering/Renderer.h>

RenderLayer::RenderLayer(const std::shared_ptr<EditorLayer> edl)
	:
	Layer("RenderLayer"),
	m_EditorLayer(edl.get())
{
	m_FrameBuffers.emplace_back();
}

void RenderLayer::OnAttach()
{

}

void RenderLayer::OnDetach()
{
}

void RenderLayer::OnUpdate(float dt)
{
	auto& reg = m_EditorLayer->m_Registry;

	Camera& camera = m_EditorLayer->GetEditorCamera();
	m_FrameBuffers[0].Bind();

	Renderer::ClearColor(0.5f, 0.4f, 0.4f, 1.0f);
	Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Renderer::EnableDepth();
	bool a = Renderer::GetAlphaBlending();
	
	auto view = m_EditorLayer->m_Registry.view<MeshComponent, TransformComponent>();
	for (auto entity : view)
	{
		auto [meshC, transf] = m_EditorLayer->m_Registry.get<MeshComponent, TransformComponent>(entity);
		const uint32_t idx = meshC.MeshIdx;
		Mesh& mesh = MeshManager::GetMesh(idx);
		Shader& shader = ShaderManager::GetShader(meshC.ShaderIdx);
		const glm::mat4 mat = transf.CalcMatrix();
		mesh.Draw(camera, mat, shader);
	}
	//Renderer::SetAlphaBlending(true);
	//TODO: move to debug layer
	for (auto entity : reg.view<CameraComponent>())
	{
		auto debugCam = reg.get<CameraComponent>(entity).camera;
		Renderer::DrawFrustum(camera, debugCam, { 1.0f, 0.5f, 0.3f, 0.4f });
	}

	m_FrameBuffers[0].Unbind();

	auto view2 = m_EditorLayer->m_Registry.view<CameraComponent>();
	for (auto entity : view2)
	{
		fbcam.Bind();
		Renderer::ClearColor(0.0F, 0.0f, 1.0f, 1.0f);
		Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Renderer::EnableDepth();

		const auto& cc = m_EditorLayer->m_Registry.get<CameraComponent>(entity);

		for (auto otherEntity : view)
		{
			if (otherEntity != entity)
			{
				auto [mc, t] = m_EditorLayer->m_Registry.get <MeshComponent, TransformComponent>(otherEntity);
				Shader& otherShader = ShaderManager::GetShader(mc.ShaderIdx);
				Mesh& otherMesh = MeshManager::GetMesh(mc.MeshIdx);
				otherMesh.Draw(cc.camera, t.CalcMatrix(), otherShader);
			}
		}
	}

	fbcam.Unbind();
}

void RenderLayer::OnImGuiRender(float dt)
{
	const auto texId = m_FrameBuffers[0].GetTexture().GetID();

	ImGui::Begin("Scene");
	const ImVec2 avail_size = ImGui::GetContentRegionAvail();
	ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texId)),
		avail_size, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	;
	ImGui::Begin("framebuffer");
	ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(fbcam.GetTexture().GetID())),
		ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));


	ImGui::End();



}

