#include "ImGuiManager.h"
#include "Editor.h"


void ImGuiManager::Prepare()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::Init()
{
	//glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	
	io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/OpenSans/OpenSans-Regular.ttf", 18.0f);
	io.Fonts->AddFontFromFileTTF("res/fonts/Nunito/Nunito-Regular.ttf", 18.0f);
	io.Fonts->AddFontFromFileTTF("res/fonts/NunitoSans/NunitoSans-Regular.ttf", 18.0f);
	io.Fonts->AddFontFromFileTTF("res/fonts/NotoSansJP/NotoSansJP-Regular.otf", 18.0f);
	io.Fonts->AddFontFromFileTTF("res/fonts/Montserrat/Montserrat-Regular.ttf", 18.0f);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.33f, 0.40f, 0.48f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.32f, 0.34f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.25f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.57f, 0.25f, 0.28f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.57f, 0.25f, 0.28f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.67f, 0.75f, 0.84f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.47f, 0.64f, 0.85f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.41f, 0.56f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.59f, 0.63f, 0.68f, 0.41f);
	colors[ImGuiCol_Separator] = ImVec4(0.52f, 0.59f, 0.80f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.84f, 0.84f, 0.84f, 0.78f);
	colors[ImGuiCol_Tab] = ImVec4(0.43f, 0.48f, 0.53f, 0.48f);
	colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.47f, 0.55f, 0.95f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.13f, 0.13f, 0.74f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.57f, 0.67f, 0.80f, 0.70f);

	ImGui_ImplGlfw_InitForOpenGL(Renderer::GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 410");


}

void ImGuiManager::ShutDown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::End()
{
	ImGuiIO& io = ImGui::GetIO();
	//io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
