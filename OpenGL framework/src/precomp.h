#define GLM_ENABLE_EXPERIMENTAL 
#define GLEW_STATIC

#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "glm/gtx/intersect.inl"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image/stb_image.h"


#define ASSERT(x) if(!(x)) __debugbreak(); 
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);


#include "util.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"

#include "IndexBuffer.h"

#include "tests/Test.h"
#include "tests/TestClearColor.h"

#include "Shader.h"

#include "Renderer.h"
#include "Texture.h"
#include "Camera.h"

#include "Transform.h"

#include "GameObject.h"
#include "Cube.h"



#include "obj_loader.h"

#include "mesh.h"


