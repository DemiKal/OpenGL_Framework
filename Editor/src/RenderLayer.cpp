#include "RenderLayer.h"
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
	//m_FrameBuffers.emplace_back();
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
 	RenderCamera();
}

void RenderLayer::OnImGuiRender(float dt)
{
	
}