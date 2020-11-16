#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h" 

class ImGuiManager //possibly convert to layer if needed?
{
public:
	static void Prepare();
	static void End();
	static void Init();
	static void ShutDown();
};

