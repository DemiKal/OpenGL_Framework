#include "precomp.h"
#include "Renderer.h"
#include "Buffer/IndexBuffer.h"
#include "Buffer/VertexArray.h"
#include "Shader.h"
#include "Rendering/ShaderManager.h"
#include "GameObject/Camera.h"

void Renderer::CreateCubeMesh()
{
	cubeVertices = {
		{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f,0.5f, 0.5f},
		{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f,0.5f, -0.5f},
	};

	// cubeIndices = {
	//	 0,1,2,
	//	3,0,4,
	//	1,5,6,
	//	6,2,1,
	//	5,4,7,
	//	7,6,4,
	//	0,4,7,
	//	7,3,0,
	//	3,4,6,
	//	6,7,3,
	//	0,1,5,
	//	3,4,5 
	// };

	cubeIndices = {
	   0,1, 1,2, 2,3, 3,0,
	   1,5, 5,6, 6,2,
	   5,4, 6,7,
	   4,7, 0,4, 3,7
	};
	
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO); 
	
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * cubeVertices.size(), &cubeVertices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);

	glGenBuffers(1, &cubeEBO);	
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int),
		&cubeIndices[0], GL_STATIC_DRAW));
	
	
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::CreateTriangle()
{

}

void Renderer::CreatePlane()
{

}
 

void Renderer::DrawCube(const Camera& cam, const glm::mat4& transform,  const glm::vec4 color)
{
	auto& shader = ShaderManager::GetShader("AABB_single");
	shader.Bind();
 
 	//shader.SetVec4f("u_color", glm::vec4(1.0f, 0.75f, 0.5f, 1.0f));
	shader.SetUniformMat4f("model", transform);
	shader.SetUniformMat4f("view", cam.GetViewMatrix());
	shader.SetUniformMat4f("projection", cam.GetProjectionMatrix());
	shader.SetUniform4f("u_color", color.x, color.y, color.z, color.w);

	glBindVertexArray(cubeVAO);

	GLCall(glDrawElements(GL_LINES, 
		static_cast<GLsizei>(cubeIndices.size()), 
		GL_UNSIGNED_INT, nullptr));

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

Renderer::Renderer()
{
	CreateCubeMesh();
	CreateTriangle();
	CreatePlane();
}

void Renderer::Clear()
{
	GLCall(glClearColor(0.2f, 0.3f, 0.6f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_POINTS, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	//glDrawElementsBaseVertex(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0, 0);
}

void Renderer::SetAlphaBlending(bool set)
{
	m_alphaBlending = set;

	if (set)
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}
	else { GLCall(glDisable(GL_BLEND)); }


}
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum err = glGetError())
	{
		std::cout << "OpenGL Error: " << err << "\n" <<
			function << "\n" << "line: " << line << "\n" <<
			"file:" << file << std::endl;
		return false;
	}

	return true;
}