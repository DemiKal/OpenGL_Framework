/*
#include "precomp.h"
#include "UserInterface.h"
#include "GameObject/EntityManager.h"
#include "misc/InputManager.h"
#include "Light/LightManager.h"
#include "GameObject/Camera.h"

void UserInterface::Update(float dt)
{
	//prepare frame
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();

	EntityBrowser();
	EntityInspector();
	MainMenu(dt);
}


void UserInterface::Draw()
{
//	ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::MainMenu(float deltaTime) const
{
	////if (ImGui::BeginMainMenuBar())
	//{
	////	if (ImGui::BeginMenu("File"))
	//	{
	//		//ImGui::ShowExampleMenuFile();
	//		if (ImGui::MenuItem("Open", "CTRL+O")) {}
	//		if (ImGui::MenuItem("Import", "CTRL+I")) {}
	//		if (ImGui::MenuItem("Save", "CTRL+S")) {}
	//		if (ImGui::MenuItem("Exit", "Alt+F4")) {}
	//
	//		ImGui::EndMenu();
	//	}
	//	if (ImGui::BeginMenu("Edit"))
	//	{
	//		if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
	//		if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
	//		ImGui::Separator();
	//		if (ImGui::MenuItem("Cut", "CTRL+X")) {}
	//		if (ImGui::MenuItem("Copy", "CTRL+C")) {}
	//		if (ImGui::MenuItem("Paste", "CTRL+V")) {}
	//		ImGui::EndMenu();
	//	}
	//
	//	ImGui::EndMainMenuBar();
	//}
	//glm::vec3& lightDir = LightManager::GetDirectionalLight();
	//static float rotation = 0;
	//
	//static bool pauseLight = true;
	//ImGui::Checkbox("Pause Light", &pauseLight);
	//
	//Camera* camera = Camera::GetMain();
	//glm::vec3 lightCopy = camera->GetViewMatrix() * glm::vec4(-lightDir, 0.0f);
	//if (ImGui::gizmo3D("##Dir1", lightCopy))
	//{
	//	lightDir = glm::inverse(camera->GetViewMatrix()) * glm::vec4(-lightCopy, 0.0f);
	//	rotation = atan2f(sinf(lightDir.z), cosf(lightDir.x));
	//}
	//else if (!pauseLight)
	//{
	//	lightDir.x = sinf(rotation);
	//	lightDir.z = cosf(rotation);
	//
	//	rotation += 1.f * deltaTime;
	//}
	//float& ambientLight = LightManager::GetAmbientLight();
	//ImGui::SliderFloat("ambient", &ambientLight, 0, 1);

}

void UserInterface::EntityBrowser() const
{
	//ImGuiWindowFlags window_flags = 0;
	//
	//ImGui::Begin("Entities", nullptr, window_flags);

	//for (auto& ent : EntityManager::GetEntities())
	//{
	//	if (ImGui::BeginMenu(ent->name.c_str()))
	//	{
	//		for (Mesh& mesh : ent->meshes) {
	//			if (ImGui::MenuItem("Vertex count:", std::to_string(mesh.m_Vertices.size()).c_str())) {}
	//			if (ImGui::MenuItem("Has animation: ", mesh.HasAnimation() ? "yes" : "no")) {}
	//
	//
	//		}
	//		ImGui::EndMenu();
	//	}
	//}
	Model* selected = InputManager::GetSelectedModel();

	//if (ImGui::TreeNode("Entities"))
	{
		for (auto  ent : EntityManager::GetEntities())
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
				else {
			bool isSelected = ent_name == (selected != nullptr ? selected->m_name : "none selected!");

	//		if (ImGui::Selectable(ent_name.c_str(), isSelected))
			{
				/*		if (ent_name == "plane") {
							int i = 0;
						}
				selected = ent.get();
				InputManager::SetSelectedModel(ent.get());
			}
		}
	//	ImGui::TreePop();
	}

	//ImGui::End();
}

void UserInterface::EntityInspector()
{
	//ImGui::Begin("Inspector");
	Model* selection = InputManager::GetSelectedModel();
	if (selection != nullptr)
	{
		//ImGui::ColorEdit4("clear color", static_cast<float*>(&override_color[0]));
	//	ImGui::Text("Currently selected: ", selection->m_Name.c_str());
	//	ImGui::SliderFloat3("Position", &selection->m_position[0], -100.0f, 100.0f);
	//	ImGui::SliderFloat3("Rotation", &selection->m_rotation[0], 0, 360);
	//	ImGui::SliderFloat3("Scale", &selection->m_scale[0], 0, 10);
	}

	//ImGui::End();
}

*/