//#include "ViewportPanel.h"
//#include "RenderLayer.h"
#include "EditorLayer.h"
#include "RenderLayer.h"
//#include "RenderLayer.h"
#include "Editor.h"
#include "Gizmos/Gizmo.h"

void EditorLayer::RenderViewportPanel(float dt)
{

	ComputeShader& comp = ShaderManager::GetComputeShader("raytrace");
	m_UpperLvlBVH->Bind();
	static bool dispatch = false;

	ImGui::Begin("Ray trace test");
	ImGui::Checkbox("render", &dispatch);
	Texture2D* spyrotex = nullptr;
	glm::mat4 spyromat;
	for(auto entity : m_Registry.view<TagComponent, MeshComponent>())
	{
		const auto& [tagc, mc, tc] = m_Registry.get<TagComponent, MeshComponent, TransformComponent>(entity);
		if (tagc.Name.data() == "Spyro")
		{
			Mesh& m = MeshManager::GetMesh(mc.MeshIdx);
			//.Bind
			spyrotex = &m.m_Textures[0];
			spyromat = tc.CalcMatrix();
		}
	}

	if (dispatch) comp.Dispatch([&]()
		{
			spyrotex->Bind(1);

			comp.SetFloat("u_aspectRatio", m_EditorCamera.GetAspectRatio());
			comp.SetFloat("u_screenWidth", 1920);
			comp.SetFloat("u_screenHeight", 1080);
			comp.SetFloat("u_nearPlane", m_EditorCamera.GetNearPlaneDist());
			//comp.SetFloat("u_ambientLight", 
			comp.SetFloat("u_epsilon", 0.001f);
			
			comp.SetVec3f("u_cameraPos", m_EditorCamera.GetPosition());
			comp.SetVec3f("u_viewDir", m_EditorCamera.GetForwardVector());
			comp.SetVec3f("u_cameraUp", m_EditorCamera.GetUpVector());
			
			comp.SetUniformMat4f("u_inv_viewMatrix", glm::inverse(m_EditorCamera.GetViewMatrix()));
			comp.SetUniformMat4f("u_inv_projMatrix", glm::inverse(m_EditorCamera.GetProjectionMatrix()));
			comp.SetUniformMat4f("u_InvTransform", glm::inverse(spyromat));
		});

	spyrotex->Unbind();

	auto sz = ImGui::GetContentRegionAvail();
	auto id1 = comp.GetComputeTexture().GetID();
	auto* id = reinterpret_cast<void*>(static_cast<intptr_t>(id1));
	ImGui::Image(id, sz, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();



	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin("##SceneView", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration);

	ImGui::BeginTabBar("Scene");
	const float windowWidth = ImGui::GetContentRegionAvail().x;
	const float widgetWidth = 150.0f;

	//std::string comboName = "Debug Render Options";
	const char* name = "Gizmos..";
	const auto buttonWidth = ImGui::CalcTextSize(name).x;
	ImGui::SameLine(windowWidth - 1.1f * buttonWidth);

	ImGui::PopStyleVar();

	if (ImGui::Button(name))
		ImGui::OpenPopup("MyGizmoPopup");

	if (ImGui::BeginPopup("MyGizmoPopup"))
	{
		ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
		for (auto& gizmo : m_Gizmos)
		{
			if (&gizmo->m_Enabled)
			{
				if (ImGui::BeginMenu(gizmo->m_Name.c_str()))
				{
					ImGui::MenuItem("Enable", "", &gizmo->m_Enabled);
					ImGui::ColorEdit4("##MyColor", &gizmo->m_Color[0], ImGuiColorEditFlags_NoInputs);

					ImGui::EndMenu();
				}
			}


			ImGui::PopItemFlag();
			ImGui::EndPopup();


		}
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	if (ImGui::BeginTabItem("View"))
	{
		ImGui::BeginChild("GameChild");
		m_ImGuiRegionSize = ImGui::GetContentRegionAvail();

		FrameBuffer& sceneFrame = m_Editor->GetCommandBuffer().GetFrameBuffer("Editor Scene Frame");
		const auto texId = sceneFrame.GetTexture().GetID();	//todo fix 0 indexing!


		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texId)),
			m_ImGuiRegionSize, ImVec2(0, 1), ImVec2(1, 0));
		auto xy = ImGui::GetItemRectSize();

		DrawGizmos(dt);

		if (m_Selected != entt::null && m_Registry.has<CameraComponent>(m_Selected))
		{
			RenderLayer* rl = m_Editor->GetLayer<RenderLayer>();
			if (rl)
			{
				FrameBuffer& fb = *rl->m_FramebufferCamera;
				const float aspect = fb.GetAspect();
				const ImVec2 sizeOG(250, 250);
				const ImVec2 size(sizeOG.x, sizeOG.y / aspect);

				//ImVec2 = avail ImGui::GetContentRegionAvail();
				auto* texid = reinterpret_cast<void*>(static_cast<intptr_t>(fb.GetTexture().GetID()));

				const ImVec2 wSize = ImGui::GetWindowSize();
				const ImVec2 wPos = ImGui::GetWindowPos();
				const ImVec2 anchorPos(wPos + wSize);
				const ImVec2 bottomRight = wPos + wSize;

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
				ImGui::SetNextWindowPos(bottomRight - size - ImVec2(10, 10), ImGuiCond_::ImGuiCond_Always/*, ImVec2(0.5,0.5)*/);
				ImGui::SetNextWindowSize(size, ImGuiCond_::ImGuiCond_Always);

				const auto flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar;

				if (ImGui::Begin("Selected camera view"), nullptr, flags)
				{
					ImVec2 newSize = ImGui::GetContentRegionAvail();
					ImGui::Image(texid, newSize, ImVec2(0, 1), ImVec2(1, 0));

					//ImVec2 collapsedSize = ImGui::GetWindowSize();
					//ImGui::SetWindowPos(bottomRight - collapsedSize, ImGuiCond_::ImGuiCond_Always);
					ImGui::End();
				}

				ImGui::PopStyleVar();
			}
		}
		ImGui::EndChild();

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Game"))
	{
		ImGui::BeginChild("GameSubWindow");
		m_ImGuiRegionSize = ImGui::GetContentRegionAvail();
		RenderLayer* rl = m_Editor->GetLayer<RenderLayer>();	//TODO: use a render manager to get framebuffer!
		if (rl)
		{
			auto* texid = reinterpret_cast<void*>(static_cast<intptr_t>((*rl->m_FramebufferCamera).GetTexture().GetID()));
			ImGui::Image(texid, m_ImGuiRegionSize, ImVec2(0, 1), ImVec2(1, 0));

		}
		//ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
		ImGui::EndChild();
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
	//ImGui::EndColumns();
	ImGui::PopStyleVar();
	ImGui::End();


}