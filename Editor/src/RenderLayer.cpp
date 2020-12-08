#include "RenderLayer.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"
#include "ImGuiManager.h"
#include "Rendering/Renderer.h"

RenderLayer::RenderLayer(std::shared_ptr<EditorLayer> edl)
	:
	Layer("RenderLayer"),
	m_EditorLayer(std::move(edl))// /*std::make_shared<EditorLayer>(*edl ))
{

}

void RenderLayer::OnAttach()
{

}

void RenderLayer::OnDetach()
{
}


void RenderLayer::RenderCamera() const
{
	Renderer& renderer = m_Editor->GetRenderer();
	
	auto view2 = m_EditorLayer->m_Registry.view<CameraComponent>();
	auto meshes = m_EditorLayer->m_Registry.view<MeshComponent, TransformComponent>();
	
	m_FramebufferCamera.Bind();  
	
	auto [fbWidth, fbHeight] = m_FramebufferCamera.GetSize();
	auto prevViewport = renderer.GetViewPort();
	renderer.SetViewPort(0, 0, fbWidth, fbHeight);

	for (auto entity : view2)
	{
		renderer.SetClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		renderer .Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer .EnableDepth();

		const auto& cc = m_EditorLayer->m_Registry.get<CameraComponent>(entity);

		for (auto otherEntity : meshes)	 
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
	renderer.SetViewPort(prevViewport);

}

void RenderLayer::OnUpdate(float dt)
{
	RenderCamera();
}

void RenderLayer::OnImGuiRender(float dt)
{
	 
}