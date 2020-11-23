#pragma once
#define PRINTAPI2(x)  std::cout << std::string(#x) << "\n";
#define PRINTAPI(x)   std::string(#x) ;

//#define MACRO2(x, ...) std::cout << #x << std::endl; //domore(__VA_ARGS__)
//#define MACRO(...) MACRO2(...)

#include "ScreenQuad.h"
#include "GameObject/Components/AABB.h"

class FrameBuffer;
class IndexBuffer;
class VertexArray;
class Shader;
class Camera;

//template<size_t n>

class Renderer
{
private:

	GLenum m_CullingMode;
	GLenum m_DepthFunction;

	bool m_AlphaBlending;
	bool m_Cull;
	bool m_DepthTest;
	bool m_VSync;
	
	GLFWwindow* m_Window;

	static ScreenQuad screenQuad; //TODO delete

	void _DisableDepth();
	void _EnableDepth();
	void _SetAlphaBlending(bool alphaValue);
	void _SetVSync(bool cond);
public:
	Renderer();
	static void ShutDown();
	static void DrawFrustum(const Camera& renderCam, Camera& debugCam, const glm::vec4& color);
	static void DrawLine(const glm::mat4& model, const  Camera& cam, const glm::vec3& a, const glm::vec3& b, const glm::vec4& color);
	void CreateLine();
	void CalcFrameTime(float deltaTime);
	void UpdateUI(float deltaTime);
	static void Clear();
	static void Draw(const VertexArray& va, const IndexBuffer& ib, const  Shader& shader);
	static void SetAlphaBlending(bool alphaValue);
	static void SetVSync(bool cond);
	static void BlitTexture(FrameBuffer& frameBuffer, std::optional<FrameBuffer> target);
	static void SwapBuffers(GLFWwindow* window);
	static void BlitFrameBuffer(unsigned int from,
		unsigned int to,
		GLenum type,
		glm::ivec2 srcStart = { 0,0 },
		glm::ivec2 srcEnd = { SCREENWIDTH, SCREENHEIGHT },
		glm::ivec2 destStart = { 0,0 },
		glm::ivec2 destEnd = { SCREENWIDTH, SCREENHEIGHT },
		GLenum filterMethod = GL_NEAREST);

	[[nodiscard]] static bool GetAlphaBlending();
	[[nodiscard]] uint32_t GetCubeVAO() const;
	[[nodiscard]] static GLenum GetDepthFunc();
	[[nodiscard]] static GLenum GetCullingMode();

	//use this before calling Clear for a specific color
	static void SetClearColor(float r, float g, float b, float a);
	static void SetClearColor(const glm::vec4& color);
	static void Clear(GLenum type);
	static void Enable(GLenum type);
	static void DrawScreenQuad();
	static void EnableDepth();
	static void DisableDepth();
	void _Init();
	static void Init();
	static void SwapBuffers();
	static GLFWwindow* GetWindow();
	static Renderer& GetInstance();
	void CreateCubeMesh();
	void CreateTriangle();
	void CreatePlane();
	void DrawInstancedCubes(GLsizei instanceCount) const;
	void DrawCube(const Camera& cam, const glm::mat4& transform, glm::vec4 color) const;
	void SetDepthFunc(GLenum depthFunc);
	void SetCullingMode(GLenum cullingMode);

};
