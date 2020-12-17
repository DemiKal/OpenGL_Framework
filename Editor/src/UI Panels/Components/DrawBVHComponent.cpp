#include "UI Panels/UI includes.h"
#include "EditorLayer.h"

class EditorLayer;

void DrawUIComponent(BVHComponent& mc, EditorLayer* edl,  const float dt)
{
	ImGui::Text("BVH %i", mc.BVHidx);
	BVH& bvh = edl->m_UpperLvlBVH->GetBVH(mc.BVHidx);
	ImGui::Text("Size %i", bvh.m_Count);
	ImGui::Text("Offset Range: [%i, %i]", bvh.StartOffset, bvh.EndOffset);
	ImGui::Checkbox("Draw BVH", &mc.Draw);
}