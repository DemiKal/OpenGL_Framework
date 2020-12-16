#include "UI Panels/UI includes.h"


void DrawVec3Component(const std::string& label, glm::vec3& vec, float resetVal = 0);

void DrawUIComponent(TransformComponent& t)
{
	DrawVec3Component("Translation", t.Position);

	auto rotInDeg = glm::degrees(t.Rotation);
	DrawVec3Component("Rotation", rotInDeg);
	t.Rotation = glm::radians(rotInDeg);
	DrawVec3Component("Scale", t.Scale, 1.0f);

	ImGui::Separator();
}