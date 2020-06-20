#ifndef PRECOMP_H
#define PRECOMP_H
#define GLM_ENABLE_EXPERIMENTAL 

#pragma warning(push, 0)	//disable warnings on external header files
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <numeric>
#include <exception>
#include <algorithm>
//datastructures
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <map>

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

#include <magic_enum.hpp>
#include <entt/entt.hpp>
#include <fmt/format.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h> 
#include <imgui/extensions/imGuIZMOquat.h> 
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)

constexpr uint32_t SCREENWIDTH = 1920;
constexpr uint32_t SCREENHEIGHT = 1080;

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
#endif PRECOMP_H
