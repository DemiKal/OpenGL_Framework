#pragma once
#define PRINTAPI(x)  std::string(#x) ;

#include "ScreenQuad.h"
#include "GameObject/Components/AABB.h"

class FrameBuffer;
class IndexBuffer;
class VertexArray;
class Shader;
class Camera;

class Renderer
{
private:
	bool m_alphaBlending = true;
	std::vector<glm::vec3> cubeVertices;
	std::vector<unsigned int> cubeIndices;
	unsigned int cubeVAO = 0, cubeVBO = 0, cubeEBO = 0;
	//GLuint lineVAO = 0;
	//GLuint lineVBO = 0;
	GLenum m_depthFunction = GL_LEQUAL;
	GLenum m_cullingMode = GL_BACK;
	bool m_VSync = true;
	static ScreenQuad screenQuad;
	std::vector<float> m_frameTimes;
	double m_totalTime{};
	//std::vector<Gbuffer> m_gBuffers;
	GLFWwindow* m_Window;

public:
	float m_prevFrameTime{}, m_currentFrameTime{};
	static void ShutDown();

	static void DrawFrustum(const Camera& renderCam, Camera& debugCam, const glm::vec4& color);
	static void DrawLine(const glm::mat4& model, const  Camera& cam, const glm::vec3& a, const glm::vec3& b, const glm::vec4& color);
	void CreateLine();
	Renderer();
	bool alphaBlend{};
	void CalcFrameTime(float deltaTime);
	void UpdateUI(float deltaTime);
	static void Clear();
	static void Draw(const VertexArray& va, const IndexBuffer& ib, const  Shader& shader);
	void SetAlphaBlending(bool alphaValue);
	void SetVSync(bool cond);
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

	static void ClearColor(float r, float g, float b, float a);
	static void ClearColor(const glm::vec4& color);
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
