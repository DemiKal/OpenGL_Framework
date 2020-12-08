#pragma once
//#include "Editor.h"


//#include "vendor/imgui/imgui.h"
//#include "vendor/imgui/imgui_impl_glfw.h"
//#include "vendor/imgui/imgui_impl_opengl3.h" 
//#include "vendor/imgui/imgui_internal.h" 

//#include "vendor/imgui/imgui_impl_glfw" 

class Renderer;

class ImGuiManager //possibly convert to layer if needed?
{
public:
	static void Prepare();
	static void End();
	static void Init(Renderer& renderer);
	static void ShutDown();
	
};

