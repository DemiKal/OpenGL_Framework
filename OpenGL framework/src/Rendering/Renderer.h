#pragma once
#define PRINTAPI(x)  std::string(#x) ;
//constexpr std::string printname(GLenum x) { return std::string(x); }

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
	unsigned int cubeVAO{}, cubeVBO{}, cubeEBO{};
	GLenum m_depthFunction = GL_LEQUAL;
	GLenum m_cullingMode = GL_BACK;
	bool m_VSync;


public:
	Renderer();

	static void Clear();
	static void Draw(const VertexArray& va, const IndexBuffer& ib, const  Shader& shader);
	void SetAlphaBlending(bool alphaValue);
	void SetVSync(bool cond);
	static void SwapBuffers(GLFWwindow* window);
	static void BlitFrameBuffer(unsigned int from,
	                            unsigned int to,
	                            GLenum type,
	                            glm::ivec2 srcStart = { 0,0 },
	                            glm::ivec2 srcEnd = { SCREENWIDTH, SCREENHEIGHT },
	                            glm::ivec2 destStart = { 0,0 },
	                            glm::ivec2 destEnd = { SCREENWIDTH, SCREENHEIGHT },
	                            GLenum filterMethod = GL_NEAREST);

	static void ClearColor(float r, float g, float b, float a);
	static void ClearColor(const glm::vec4& color);
	static void Clear(GLenum type);

	[[nodiscard]] bool GetAlphaBlending() const { return m_alphaBlending; }
	void CreateCubeMesh();
	void CreateTriangle();
	void CreatePlane();
	void DrawInstancedCubes(GLsizei instanceCount) const;
	void DrawCube(const Camera& cam, const glm::mat4& transform,  glm::vec4 color) const;
	uint32_t GetCubeVAO() const { return cubeVAO; }

	static void Enable(GLenum type);
	static void EnableDepth();
	static void DisableDepth();
	void SetDepthFunc(GLenum depthFunc);
	GLenum GetDepthFunc() const { return m_depthFunction; }

	GLenum GetCullingMode() const { return m_cullingMode; }
	void SetCullingMode(GLenum cullingMode);
};
