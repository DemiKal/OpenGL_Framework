#pragma once

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
	unsigned int cubeVAO, cubeVBO, cubeEBO;
	inline static GLenum m_depthFunction = GL_LEQUAL;
	//Mesh cube;

public:
	Renderer();
	static void Clear();
	static void Draw(const VertexArray& va, const IndexBuffer& ib, const  Shader& shader);
	void SetAlphaBlending(bool set);
	static void BlitFrameBuffer(const unsigned int from,
	                            const unsigned int to,
	                            const GLenum type,
	                            glm::ivec2 srcStart = { 0,0 },
	                            glm::ivec2 srcEnd = { SCREENWIDTH, SCREENHEIGHT },
	                            glm::ivec2 destStart = { 0,0 },
	                            glm::ivec2 destEnd = { SCREENWIDTH, SCREENHEIGHT },
	                            const GLenum filterMethod = GL_NEAREST);

	static void ClearColor(float r, float g, float b, float a);
	static void ClearColor(const glm::vec4& color);
	static void Clear(GLenum type);

	[[nodiscard]] bool GetAlphaBlending() const { return m_alphaBlending; }
	void CreateCubeMesh();
	void CreateTriangle();
	void CreatePlane();
	void DrawCube(const Camera& cam, const glm::mat4& transform,  const glm::vec4 color) const;
	static void Enable(GLenum type);
	static void EnableDepth();
	static void SetDepthFunc(GLenum depthFunc);
	static void DisableDepth();
	GLenum GetDepthFunc() const { return m_depthFunction; }
};
