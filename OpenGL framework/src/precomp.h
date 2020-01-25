#define GLM_ENABLE_EXPERIMENTAL 
#define GLEW_STATIC

const int SCREENWIDTH = 1280;
const int SCREENHEIGHT = 720;

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
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
#define ASSERT(x) if(!(x)) __debugbreak(); 
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

#include "misc/util.h"

#include "misc/obj_loader.h"


#include "Rendering/Buffer/VertexBuffer.h"
#include "Rendering/Buffer/VertexBufferLayout.h"
#include "Rendering/Buffer/VertexArray.h"
#include "Rendering/Buffer/VertexArraySub.h"
#include "Rendering/Buffer/RenderBufferObject.h"
#include "Rendering/Buffer/FrameBuffer.h"

#include "Rendering/Buffer/IndexBuffer.h"

#include "tests/Test.h"
#include "tests/TestClearColor.h"

#include "GameObject/Components/Texture2D.h"
#include "Rendering/GPUShader.h"
#include "Rendering/ShaderManager.h"

#include "Rendering/Renderer.h"
#include "Rendering/Texture.h"
#include "GameObject/Camera.h"
#include "GameObject/VertexNew.h"

#include "GameObject/Components/Transform.h"

#include "GameObject/GameObject.h"
#include "GameObject/Cube.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "GameObject/Components/Mesh.h"
#include "GameObject/Components/MeshNew.h"
#include "GameObject/Components/Model.h"



