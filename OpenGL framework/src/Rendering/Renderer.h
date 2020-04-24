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
	//Mesh cube;

public:
	Renderer();
	void Clear();
	void Draw(const VertexArray& va, const IndexBuffer& ib, const  Shader& shader);
	void SetAlphaBlending(bool set);
	bool GetAlphaBlending() const { return m_alphaBlending; }
	void CreateCubeMesh();
	void CreateTriangle();
	void CreatePlane();
	void DrawCube(const Camera& cam, const glm::mat4& transform,  const glm::vec4 color);

};
