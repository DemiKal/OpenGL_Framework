#define GLM_ENABLE_EXPERIMENTAL 

const int SCREENWIDTH = 1920;
const int SCREENHEIGHT = 1080;
#include <filesystem>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <ostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <map>
#include <numeric>

#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.inl>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <entt/entt.hpp>
#include <fmt/format.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h> 

 #include <imgui/extensions/imGuIZMOquat.h> 
//#//include <imgui/extensions/vGizmo.h> 
//#include <imgui/extensions/vgConfig.h> 
//#include <imgui/extensions/vGizmoMath.h> 
//#include <imgui/extensions/vgMath.h> 
//#include <imgui/extensions/> 
	
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



const glm::vec3 UP_WORLD(0, 1, 0);
const glm::vec3 RIGHT_WORLD(1, 0, 0);
const glm::vec3 FORWARD_WORLD(0, 0, -1);

#define ASSERT(x) if(!(x)) __debugbreak(); 
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

#include "misc/HardwareQuery.h"
