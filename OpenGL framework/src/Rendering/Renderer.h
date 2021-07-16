#pragma once
#define PRINTAPI2(x)  std::cout << std::string(#x) << "\n";
#define PRINTAPI(x)   std::string(#x) ;

//#define MACRO2(x, ...) std::cout << #x << std::endl; //domore(__VA_ARGS__)
//#define MACRO(...) MACRO2(...)

#include "ScreenQuad.h"
#include "GameObject/Components/AABB.h"
#include <optional>

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

	glm::vec4 m_ClearColor;
	glm::ivec4 m_ViewPort;
	static ScreenQuad screenQuad; //TODO delete

	bool m_Initialized = false;
public:
	Renderer();

	void Init(std::vector<std::tuple<int, int>> windowHints);
	void ShutDown();
	void DisableDepth();
	void EnableDepth();

	void SetAlphaBlending(bool alphaValue);
	void SetCullingMode(GLenum cullingMode);
	void SetDepthFunc(GLenum depthFunc);
	void SetVSync(bool cond);

	void Clear(); //TODO: check color clear! 
	void Clear(GLenum type);	//use this before calling Clear for a specific color
	void DrawCube(const Camera& cam, const glm::mat4& transform, glm::vec4 color) const;
	void DrawFrustum(const Camera& renderCam, Camera& debugCam, const glm::vec4& color);
	void DrawInstancedCubes(GLsizei instanceCount) const;
	void DrawLine(const glm::mat4& model, const  Camera& cam, const glm::vec3& a, const glm::vec3& b, const glm::vec4& color);
	void Enable(GLenum type);
	void Draw(const VertexArray& va, const IndexBuffer& ib, const  Shader& shader);
	void SetClearColor(float r, float g, float b, float a);
	void SetClearColor(const glm::vec4& color);
	void SwapBuffers() const;

	static void DrawScreenQuad();
	static void BlitTexture(FrameBuffer& frameBuffer, std::optional<FrameBuffer> target); //TODO just use nullptr check
	static void SwapBuffers(GLFWwindow* window);
	//TODO: use some specification struct for blitting!
	static void BlitFrameBuffer(unsigned int from, unsigned int to, GLenum type, glm::ivec2 srcStart = { 0,0 }, glm::ivec2 srcEnd = { SCREENWIDTH, SCREENHEIGHT }, glm::ivec2 destStart = { 0,0 }, glm::ivec2 destEnd = { SCREENWIDTH, SCREENHEIGHT }, GLenum filterMethod = GL_NEAREST);

	[[nodiscard]] bool GetAlphaBlending() const;
	[[nodiscard]] GLenum GetDepthFunc() const;
	[[nodiscard]] GLenum GetCullingMode() const;
	[[nodiscard]] bool GetVSync() const;
	[[nodiscard]] GLFWwindow* GetWindow();

	glm::ivec4 GetViewPort() const;
	glm::ivec4 QueryGLViewPort();
	void SetViewPort(const int startX, const int startY, const int width, const int height);
	void SetViewPort(const glm::ivec4& viewport);
};
