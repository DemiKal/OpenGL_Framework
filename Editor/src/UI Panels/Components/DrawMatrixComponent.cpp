#include "UI Panels/UI includes.h"

void DrawUIComponent(const glm::mat4& m, const std::string& label, const float dt)
{
	// ImGui::TreeNodeEx("matrix",ImGuiTreeNodeFlags_::)
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(label.c_str()))
	{
		ImGui::Text("%.2f", m[0].x); ImGui::SameLine();
		ImGui::Text("%.2f", m[0].y); ImGui::SameLine();
		ImGui::Text("%.2f", m[0].z); ImGui::SameLine();
		ImGui::Text("%.2f", m[0].w);

		ImGui::Text("%.2f", m[1].x); ImGui::SameLine();
		ImGui::Text("%.2f", m[1].y); ImGui::SameLine();
		ImGui::Text("%.2f", m[1].z); ImGui::SameLine();
		ImGui::Text("%.2f", m[1].w);

		ImGui::Text("%.2f", m[2].x); ImGui::SameLine();
		ImGui::Text("%.2f", m[2].y); ImGui::SameLine();
		ImGui::Text("%.2f", m[2].z); ImGui::SameLine();
		ImGui::Text("%.2f", m[2].w);

		ImGui::Text("%.2f", m[3].x); ImGui::SameLine();
		ImGui::Text("%.2f", m[3].y); ImGui::SameLine();
		ImGui::Text("%.2f", m[3].z); ImGui::SameLine();
		ImGui::Text("%.2f", m[3].w);
		ImGui::TreePop();
	}
}