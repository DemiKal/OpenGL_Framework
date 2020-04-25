#include "precomp.h"
#include "UserInterface.h"
#include "GameObject/EntityManager.h"
#include "misc/InputManager.h"

void UserInterface::Update()
{
	//prepare frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	EntityBrowser();
	EntityInspector();
	MainMenu();
}

void UserInterface::Draw()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::MainMenu() const
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ImGui::ShowExampleMenuFile();
			if (ImGui::MenuItem("Open", "CTRL+O")) {}
			if (ImGui::MenuItem("Import", "CTRL+I")) {}
			if (ImGui::MenuItem("Save", "CTRL+S")) {}
			if (ImGui::MenuItem("Exit", "Alt+F4")) {}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void UserInterface::EntityBrowser() const
{
	ImGuiWindowFlags window_flags = 0;

	ImGui::Begin("Entities", nullptr, window_flags);

	//for (auto& ent : EntityManager::GetEntities())
	//{
	//	if (ImGui::BeginMenu(ent->name.c_str()))
	//	{
	//		for (Mesh& mesh : ent->meshes) {
	//			if (ImGui::MenuItem("Vertex count:", std::to_string(mesh.vertices.size()).c_str())) {}
	//			if (ImGui::MenuItem("Has animation: ", mesh.hasAnimation() ? "yes" : "no")) {}
	//
	//
	//		}
	//		ImGui::EndMenu();
	//	}
	//}
	Model* selected = InputManager::GetSelectedModel();

	if (ImGui::TreeNode("Entities"))
	{
		for (Model* ent : EntityManager::GetEntities())
		{
			const std::string ent_name = ent->m_name;

			/*	if (selected == nullptr)
				{
					if (ImGui::Selectable(ent_name.c_str()))
					{
						selected = ent;
						InputManager::SetSelectedModel(ent);
					}
				}
				else {*/
			bool isSelected = ent_name == (selected != nullptr ? selected->m_name : "none selected!");

			if (ImGui::Selectable(ent_name.c_str(), isSelected))
			{
				/*		if (ent_name == "plane") {
							int i = 0;
						}*/
				selected = ent;
				InputManager::SetSelectedModel(ent);
			}
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void UserInterface::EntityInspector()
{
	ImGui::Begin("Inspector");
	Model* selection = InputManager::GetSelectedModel();
	if (selection != nullptr)
	{
		//ImGui::ColorEdit4("clear color", static_cast<float*>(&override_color[0]));
		ImGui::Text("Currently selected: ", selection->m_name.c_str());
		ImGui::SliderFloat3("Position", &selection->m_position[0], -100.0f, 100.0f);
		ImGui::SliderFloat3("Rotation", &selection->m_rotation[0], 0, 360);
		ImGui::SliderFloat3("Scale", &selection->m_scale[0], 0, 10);
	}

	ImGui::End();
}
