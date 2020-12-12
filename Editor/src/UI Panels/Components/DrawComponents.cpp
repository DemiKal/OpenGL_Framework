#include "EditorLayer.h"

void EditorLayer::DrawVec3Component(const std::string& label, glm::vec3& vec, float resetVal = 0)
{
	ImGui::PushID(label.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.0f);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 1, 1 });

	if (ImGui::Button("X")) vec.x = resetVal;

	ImGui::SameLine();
	ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if (ImGui::Button("Y")) vec.y = resetVal;

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if (ImGui::Button("Z")) vec.z = resetVal;

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::Columns(1);

	ImGui::PopStyleVar();
	ImGui::PopID();
}

void EditorLayer::DrawUIComponent(TransformComponent& t)
{
	DrawVec3Component("Translation", t.Position);

	auto rotInDeg = glm::degrees(t.Rotation);
	DrawVec3Component("Rotation", rotInDeg);
	t.Rotation = glm::radians(rotInDeg);
	DrawVec3Component("Scale", t.Scale, 1.0f);

	ImGui::Separator();
}

void EditorLayer::DrawUIComponent(const glm::mat4& m, const  std::string& label, const float dt)
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


