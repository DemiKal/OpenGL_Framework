#include "UI Panels/UI includes.h"

void DrawUIComponent(MeshComponent& mc, const float dt)
{
	ImGui::SameLine(0, 30);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { -1.0f, -1.0f });
	ImGui::Checkbox("##Enabled", &mc.Enabled);
	ImGui::PopStyleVar();
	ImGui::RadioButton("Initialized", mc.Initialized);
	const std::string  meshStr = fmt::format("Mesh index: {0}", std::to_string(mc.MeshIdx));
	ImGui::Text(meshStr.c_str());
	//ImGui::SameLine();

	ImGui::Checkbox("Draw AABB", &mc.DrawAABB);
	if (mc.DrawAABB)
	{
		ImGui::SameLine();
		ImGui::ColorEdit4("##aabbcolor", glm::value_ptr(mc.AABBcolor), ImGuiColorEditFlags_NoInputs);
	}

	ImGui::Checkbox("Visualize wireframe", &mc.DrawWireFrame);
	if (mc.DrawWireFrame)
	{
		ImGui::SameLine();
		ImGui::ColorEdit4("##wireFrameCol", glm::value_ptr(mc.WireFrameColor), ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::DragFloat("Thickness", &mc.WireframeThickness, 0.0001  , 0, 1, NULL);
	}

	ImGui::Checkbox("Visualize normals", &mc.DrawNormals);
	if (mc.DrawNormals)
	{
		ImGui::SameLine();
		ImGui::ColorEdit4("##normalsCol", glm::value_ptr(mc.NormalsColor), ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();

		ImVec2 avail = ImGui::GetContentRegionAvail();
		ImGui::PushItemWidth(avail.x / 3.0f);
		//ImGui::DragFloat("Magnitude normals", &mc.NormalsMagnitude,0.05, 5, "%.3f");
		const ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat;
		ImGui::DragFloat("Magnitude", &mc.NormalsMagnitude, 0.01f, -1, 5, "%.3f", sliderFlags);
		//ImGui::DragInt("Offset X", &offset_x, 1.0f, -1000, 1000);
		//ImGui::ColorButton("Color", mc.WireFrameColor);
	}

	Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
	const uint32_t nrTextures = static_cast<uint32_t>(mesh.m_Textures.size());

	if (mesh.HasAnimation())
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Animation"))
		{

			static bool playAnimation = false;
			if (ImGui::ArrowButton("##PlayAnimation", ImGuiDir_Right))
			{
				playAnimation = !playAnimation;
			}
			const std::string txt = playAnimation ? "Stop animation" : "Play animation";
			ImGui::SameLine(); ImGui::Text(txt.c_str());
			static float animSpeed = 1.0f;
			ImGui::DragFloat("Animation speed", &animSpeed, 0.001f, 0, 5, "%.3f");

			static int currentPose = 0;
			const int channelSize = mesh.m_animator.current.m_AnimationChannels[0].GetLength() - 1;
			if (playAnimation)
			{
				mesh.m_animator.UpdateAnimation(dt, animSpeed);
				float perc = mesh.m_animator.animTime / mesh.m_animator.m_Duration;
				currentPose = perc * channelSize;
			}

			if (ImGui::SliderInt("Pose", &currentPose, 0, channelSize))
			{
				playAnimation = false;
				mesh.m_animator.SetPose(currentPose);
			}

			if (ImGui::Button("Set T-Pose"))
			{
				playAnimation = false;
				currentPose = 0;
				mesh.m_animator.SetTPose();
			}
			ImGui::TreePop();
		}
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Textures", "Textures (%i)", nrTextures))
	{
		for (uint32_t i = 0; i < nrTextures; i++)
		{
			ImGui::PushID(i);
			const Texture2D& tex = mesh.m_Textures[i];
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			//ImGui::Text("Texture type: %s ", tex.GetType().c_str());

			if (ImGui::TreeNode("Texture", "Type: %s", tex.GetType().c_str(), ImGuiTreeNodeFlags_None))
			{
				ImGui::Text("Width: %i. Height: %i. Render ID: %i", tex.GetWidth(), tex.GetHeight(), tex.GetID());
				const uint32_t id = tex.GetID();

				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				const ImVec2 availSize = ImGui::GetContentRegionAvail();
				const float aspect = static_cast<float>(tex.GetHeight()) / static_cast<float>(tex.GetWidth());

				ImVec2 thumbSize(0.95f * availSize.x, 0.95f * availSize.x * aspect);
				static bool overlayUVs = false;
				ImGui::SameLine();
				ImGui::Columns();
				ImGui::Checkbox("Overlay UVs", &overlayUVs);
				auto* texPtr = reinterpret_cast<void*>(static_cast<intptr_t>(id));
				if (ImGui::ImageButton(texPtr, thumbSize, ImVec2(0, 1), ImVec2(1, 0), 0))
				{
					ImGui::OpenPopup("texture_popup");
				}
				if (ImGui::BeginPopup("texture_popup"))
				{
					ImGui::Text(("File path: /" + tex.GetPath()).c_str());
					ImGui::EndPopup();
				}
				const auto start = ImGui::GetCursorScreenPos();
				auto sp = ImGui::GetCursorStartPos();
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				const ImVec2 p0 = ImGui::GetItemRectMin();
				const ImVec2 p1 = ImGui::GetItemRectMax();
				const glm::vec2 p0v(p0.x, p0.y);
				const glm::vec2 p1v(p1.x, p1.y);
				ImU32 col_a = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
				ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
				//drawList->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);

				const glm::vec2 scale = { p1.x - p0.x, p1.y - p0.y };
				//if (!mesh.m_Indices.empty())

				//draw UVs
				if (!mesh.m_UVs.empty() && overlayUVs && i == 0)
				{
					for (auto& face : mesh.m_UVs)
					{
						auto uv1 = glm::mod(face[0], 1.0f);	 uv1.y = 1.0f - uv1.y;
						auto uv2 = glm::mod(face[1], 1.0f);	 uv2.y = 1.0f - uv2.y;
						auto uv3 = glm::mod(face[2], 1.0f);	 uv3.y = 1.0f - uv3.y;

						const auto uv1Scaled = p0v + uv1 * scale;
						const auto uv2Scaled = p0v + uv2 * scale;
						const auto uv3Scaled = p0v + uv3 * scale;

						const ImVec2 v1(uv1Scaled.x, uv1Scaled.y);
						const ImVec2 v2(uv2Scaled.x, uv2Scaled.y);
						const ImVec2 v3(uv3Scaled.x, uv3Scaled.y);

						drawList->AddTriangle(v1, v2, v3, IM_COL32(255, 255, 255, 200), 1);
					}
				}



				ImGui::TreePop();
			}


			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	ImGui::Separator();
	//ImGui::PopStyleColor();
}
