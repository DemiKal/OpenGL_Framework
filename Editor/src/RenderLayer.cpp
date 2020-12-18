#include "RenderLayer.h"

#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"
#include "Editor.h"



RenderLayer::RenderLayer(std::shared_ptr<EditorLayer> edl)
	:
	Layer("RenderLayer"),
	m_EditorLayer(std::move(edl))
{

}

void RenderLayer::OnAttach()
{
	FrameBufferSpecs specs =
	{
		specs.Name = "Camera Framebuffer",
		specs.Width = 1920,
		specs.Height = 1080,
	};

	auto* ed = m_Editor;	//this needs to be set 
	CommandBuffer& cb = ed->GetCommandBuffer();
	m_FramebufferCamera = &cb.GenerateFrameBuffer(specs);

}

void RenderLayer::OnDetach()
{
}


void RenderLayer::RenderCamera() const
{
	Renderer& renderer = m_Editor->GetRenderer();

	auto view2 = m_EditorLayer->m_Registry.view<CameraComponent>();
	auto meshes = m_EditorLayer->m_Registry.view<MeshComponent, TransformComponent>();

	m_FramebufferCamera->Bind();

	auto [fbWidth, fbHeight] = m_FramebufferCamera->GetSize2();
	const auto prevViewport = renderer.GetViewPort();
	renderer.SetViewPort(0, 0, fbWidth, fbHeight);

	for (auto entity : view2)
	{
		renderer.SetClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		renderer.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.EnableDepth();

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

	m_FramebufferCamera->Unbind();
	renderer.SetViewPort(prevViewport);

}

void RenderLayer::OnUpdate(float dt)
{
	RenderCamera();

	//ComputeShader& compute = ShaderManager::GetComputeShader("raytrace2");
	//compute.Bind();
	//const Texture2D& computeTexture = compute.GetComputeTexture();
	//const uint32_t texW = computeTexture.GetWidth();
	//const uint32_t texH = computeTexture.GetHeight();
	//computeTexture.Bind();
	static bool ya = false;
	//	if (!ya)
	{
		//	ya = true;
		//	glDispatchCompute(static_cast<GLuint>(texW), static_cast<GLuint>(texH), 1);
		//	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//	compute.Unbind();
		//	compute.GetComputeTexture().Unbind();
	}
}

void RenderLayer::OnImGuiRender(float dt)
{
	//ComputeShader& compute = ShaderManager::GetComputeShader("raytrace");
	Renderer& renderer = m_Editor->GetRenderer();
	CommandBuffer& cb = m_Editor->GetCommandBuffer();

	ImGui::Begin("Renderer Debug", 0, ImGuiWindowFlags_HorizontalScrollbar);
	bool vsync = renderer.GetVSync();
	ImGui::Checkbox("Vsync", &vsync);
	renderer.SetVSync(vsync);

	//const auto size = ImGui::GetContentRegionAvail();
	//auto* texPtr = reinterpret_cast<void*>(static_cast<intptr_t>(compute.GetComputeTexture().GetID()));
	//ImGui::ImageButton(texPtr, size, ImVec2(0, 1), ImVec2(1, 0), 0);

	ImGui::Text("Framerate: %.3f ms / %.3f fps", 1000.0f * dt, 1.0f / dt);
	ImGui::Text("Framebuffers");

	ImGui::BeginChild("Frame buffers", { 0,0 }, true, ImGuiWindowFlags_HorizontalScrollbar); // 0.0f:left, 0.5f:center, 1.0f:right

	auto& fbrs = cb.GetFrameBuffers();
	uint32_t count = static_cast<uint32_t>(fbrs.size());
	ImVec2 avail = ImGui::GetContentRegionAvail();

	//ImGui::Scrollbar(ImGuiAxis::ImGuiAxis_X);
//	ImGui::SetScrollHereX(i * 0.25f)
//	ImGui::BeginGroup();
	for (const FrameBuffer& fb : fbrs)
	{
		ImGui::BeginGroup();
		auto fbSz = fb.GetSize(); float aspect = fbSz.x / static_cast<float>(fbSz.y);
		ImGui::Text("Framebuffer \"%s\", id: %i, size: (%i, %i)", fb.GetName().data(), fb.GetID(), fbSz.x, fbSz.y);
		const ImVec2 size = { 250 * aspect, 250 };
		void* texPtr = reinterpret_cast<void*>(static_cast<intptr_t>(fb.GetTexture().GetID()));
		void* texPtrDepth = reinterpret_cast<void*>(static_cast<intptr_t>(fb.GetDepthTexture().GetID()));
		ImGui::Image(texPtr, size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Image(texPtrDepth, size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndGroup();
		ImGui::SameLine();
	}
	//	ImGui::EndGroup();
		//ImGui::EndChild();
	ImGui::Separator();

	ImGui::Spacing();
	auto& meshes = MeshManager::GetMeshes();
	ImGui::BeginGroup();
	ImGui::Text("Meshes");
	//ImGui::BeginChild(ImGuiID("Meshes"), {0,0}, true, ImGuiWindowFlags_HorizontalScrollbar);
	int i = 0;
	for (const Mesh& mesh : meshes)
	{
		//	ImGui::BeginChild("MeshesChild" + i++, { 0,0 }, true, 0);
		ImGui::BeginGroup();
		//const std::string ebo = mesh.HasFaceIndices() ? fmt::format("EBO: {}", mesh.GetEBO()) : "";
		ImGui::Text("Mesh: \"%s\", VAO: %i, VBO: %i, EBO: %i", mesh.GetFilename().data(), mesh.GetVAO(), mesh.GetVBO(), mesh.GetEBO());
		ImGui::Text("Directory: %s", mesh.GetDirectory().data());
		ImGui::Text("Triangle count: %i, Indices count: %i, Draw Mode: %i", mesh.GetTriangleCount(), mesh.GetIndicesCount(), mesh.GetElemDrawType());
		ImGui::EndGroup();
		//	ImGui::EndChild();

		const ImVec2 rectMin = ImGui::GetItemRectMin() - ImVec2{ 3, 3 };// IM_COL32(145, 199, 180, 200)
		const ImVec2 rectMax = ImGui::GetItemRectMax() + ImVec2{ 3, 3 };
		ImGui::GetWindowDrawList()->AddRect(rectMin, rectMax, IM_COL32(145, 199, 180, 200));
		ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(rectMin, rectMax, IM_COL32_BLACK, IM_COL32_BLACK, IM_COL32_WHITE, IM_COL32_WHITE);
		ImGui::SameLine();
	}

	ImGui::EndGroup();
	ImGui::EndChild();
	ImGui::End();
}