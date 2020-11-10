#include "test1.h"
////#include "Application.h"
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <fmt/format.h>
////#include "precomp.h"
//#include <vendor/imgui/imgui.h>
//#include <vendor/imgui/imgui_impl_glfw.h>
//#include <vendor/imgui/imgui_impl_opengl3.h> 
////#include <vendor/imgui/extensions/imGuIZMOquat.h> 
// 
//int SCREENWIDTH = 1024;
//int SCREENHEIGHT = 1024;
//
int main(void)
{

}
//	if (!glfwInit()) return -1;
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
//	glfwWindowHint(GLFW_DECORATED, GL_TRUE); //GL_FALSE GL_TRUE
//
//	//glfwWindowHint(GLFW_FULLSCREEN, GL_TRUE);
//	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//	// glfwGetPrimaryMonitor() ;
//
//	/* Create a windowed mode window and its OpenGL context */
//	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Test window", nullptr, nullptr);
//
//	if (!window)
//	{
//		glfwTerminate();
//		throw std::exception("ERROR: Could not create GLFW window!");
//		return -1;
//	}
//
//	glfwMakeContextCurrent(window);
//
//	if (glewInit() != GLEW_OK) fmt::print("ERROR! CAN'T INIT GLEW\n");
//
//	glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
//
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO(); (void)io;
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//
//	// Setup Dear ImGui style
//	ImGui::StyleColorsDark();
//	//ImGui::StyleColorsClassic();
//	const char* glsl_version = "#version 130";
//	// Setup Platform/Renderer bindings
//	ImGui_ImplGlfw_InitForOpenGL(window, true);
//	ImGui_ImplOpenGL3_Init(glsl_version);
//
//	while (!glfwWindowShouldClose(window))
//	{
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//		ImGui::ShowDemoWindow();
//		glClearColor(1, 1, 0.5f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//			glfwSetWindowShouldClose(window, true);
//
//		ImGui::Render();
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//		
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
//
//	glfwDestroyWindow(window);
//	glfwTerminate();
//	return 0;
//}
