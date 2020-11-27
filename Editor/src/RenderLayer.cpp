#include "RenderLayer.h"

#include <utility>
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"
#include "ImGuiManager.h"
#include "Rendering/Renderer.h"

RenderLayer::RenderLayer(std::shared_ptr<EditorLayer> edl)
	:
	Layer("RenderLayer"),
	m_EditorLayer(std::move(edl))// /*std::make_shared<EditorLayer>(*edl ))
{
	m_FrameBuffers.emplace_back();
}

void RenderLayer::OnAttach()
{

}

void RenderLayer::OnDetach()
{
}

void RenderLayer::RenderCamera()
{
	auto view2 = m_EditorLayer->m_Registry.view<CameraComponent>();
	auto meshes = m_EditorLayer->m_Registry.view<MeshComponent, TransformComponent>();

	m_FramebufferCamera.Bind(); //TODO: should each cam have its own framebuffer?

	for (auto entity : view2)
	{
		Renderer::SetClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Renderer::EnableDepth();

		const auto& cc = m_EditorLayer->m_Registry.get<CameraComponent>(entity);

		for (auto otherEntity : meshes)	//render entities other than itself
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

	m_FramebufferCamera.Unbind();
}

void RenderLayer::OnUpdate(float dt)
{
	GLFWvidmode return_struct;
	auto sas = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//glfwGetDesktopMode(&return_struct);
	//
	//int height = return_struct.Height;

	Camera& camera = m_EditorLayer->GetEditorCamera();
	m_FrameBuffers[0].Bind();

	auto [fbWidth, fbHeight] = m_FrameBuffers[0].GetSize();

	if ((fbWidth != static_cast<uint32_t>(m_ImGuiRegionSize.x) ||
		fbHeight != static_cast<uint32_t>(m_ImGuiRegionSize.y))
		&& m_ImGuiRegionSize.x > 0 && m_ImGuiRegionSize.y > 0)
	{
		const float aspect = m_ImGuiRegionSize.x / m_ImGuiRegionSize.y;
		camera.SetAspectRatio(aspect);
	}

	//m_FrameBuffers[0].Bind();
	//ImVec2 fbSize2 = { 0,1 };
	Renderer::SetClearColor(0.5f, 0.4f, 0.4f, 1.0f);
	Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Renderer::EnableDepth();
	bool a = Renderer::GetAlphaBlending();

	auto view = m_EditorLayer->m_Registry.view<MeshComponent, TransformComponent>();
	for (auto entity : view)
	{
		auto [meshC, transform] = m_EditorLayer->m_Registry.get<MeshComponent, TransformComponent>(entity);
		const uint32_t idx = meshC.MeshIdx;
		Mesh& mesh = MeshManager::GetMesh(idx);
		Shader& shader = ShaderManager::GetShader(meshC.ShaderIdx);
		const glm::mat4 mat = transform.CalcMatrix();
		mesh.Draw(camera, mat, shader);
	}



	m_FrameBuffers[0].Unbind();

	RenderCamera();

}

void RenderLayer::OnImGuiRender(float dt)
{
	const auto texId = m_FrameBuffers[0].GetTexture().GetID();	//todo fix 0 indexing!

	ImGui::Begin("Scene", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration);
	m_ImGuiRegionSize = ImGui::GetContentRegionAvail();
	ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texId)),
		m_ImGuiRegionSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
}

