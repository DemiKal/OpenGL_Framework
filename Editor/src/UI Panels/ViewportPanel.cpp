#include "ViewportPanel.h"
#include "RenderLayer.h"

ViewportPanel::ViewportPanel(EditorLayer* edl) : Panel(edl)
{

}
void ViewportPanel::OnImGuiRender(float dt)
{
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
		for (auto& gizmo : m_Edl->m_Gizmos)
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
		m_Edl->m_ImGuiRegionSize = ImGui::GetContentRegionAvail();

		const auto texId = m_Edl->m_SceneFrame.GetTexture().GetID();	//todo fix 0 indexing!
		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texId)),
			m_Edl->m_ImGuiRegionSize, ImVec2(0, 1), ImVec2(1, 0));
		auto xy = ImGui::GetItemRectSize();
		//fmt::print("region: {} {}\n", m_ImGuiRegionSize.x, m_ImGuiRegionSize.y);
		m_Edl->DrawGizmos(dt);

		if (m_Edl->m_Selected != entt::null && m_Edl->m_Registry.has<CameraComponent>(m_Edl->m_Selected))
		{
			RenderLayer* rl = m_Edl->m_Editor->GetLayer<RenderLayer>();
			if (rl)
			{
				FrameBuffer& fb = rl->m_FramebufferCamera;
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
		m_Edl->m_ImGuiRegionSize = ImGui::GetContentRegionAvail();
		RenderLayer* rl = m_Edl->m_Editor->GetLayer<RenderLayer>();	//TODO: use a render manager to get framebuffer!
		if (rl)
		{
			auto* texid = reinterpret_cast<void*>(static_cast<intptr_t>(rl->m_FramebufferCamera.GetTexture().GetID()));
			ImGui::Image(texid, m_Edl->m_ImGuiRegionSize, ImVec2(0, 1), ImVec2(1, 0));

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