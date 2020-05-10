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


void Renderer::DrawCube(const Camera& cam, const glm::mat4& transform, const glm::vec4 color) const
{
	auto& shader = ShaderManager::GetShader("AABB_single");
	shader.Bind();

	//shader.SetVec4f("u_color", glm::vec4(1.0f, 0.75f, 0.5f, 1.0f));
	shader.SetUniformMat4f("model", transform);
	shader.SetUniformMat4f("view", cam.GetViewMatrix());
	shader.SetUniformMat4f("projection", cam.GetProjectionMatrix());
	shader.SetVec4f("u_color", color.x, color.y, color.z, color.w);

	glBindVertexArray(cubeVAO);

	GLCall(glDrawElements(GL_LINES,
		static_cast<GLsizei>(cubeIndices.size()),
		GL_UNSIGNED_INT, nullptr));

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Renderer::Enable(GLenum type)
{
	GLCall(glEnable(type));
}

void Renderer::EnableDepth()
{
	Enable(GL_DEPTH_TEST);
}

void Renderer::SetDepthFunc(const GLenum depthFunc)
{
	//static const std::map<GLenum,int> validEnums = {
	//	{GL_NEVER,0}, {GL_LESS,1}, {GL_EQUAL,2}, {GL_LEQUAL,3},
	//	{GL_GREATER,4}, {GL_NOTEQUAL,5}, {GL_GEQUAL,6},
	//};
	//if(validEnums.find(depthFunc) == std::end(validEnums))	//probably slower lel
	//{
	//	std::cout << "not a viable depth func enum! \n";
	//	return;;
	//}
	//
	if (depthFunc == m_depthFunction)
	{
		//std::cout << "same func!\n";
		return;
	}

	//prob faster than std::map
	switch (depthFunc)
	{
		case GL_NEVER:	 	break;
		case GL_LESS:	 	break;
		case GL_EQUAL:	 	break;
		case GL_LEQUAL:	 	break;
		case GL_GREATER: 	break;
		case GL_NOTEQUAL:	break;
		case GL_GEQUAL:	 	break;
		default: std::cout << "not a viable depth func enum! \n"; return;;
	}

	
	m_depthFunction = depthFunc;
	GLCall(glDepthFunc(depthFunc));
}

void Renderer::DisableDepth()
{
	GLCall(glDisable(GL_DEPTH_TEST));
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

void Renderer::BlitFrameBuffer(const unsigned int from,
	const unsigned int to,
	const GLenum type,
	glm::ivec2 srcStart,
	glm::ivec2 srcEnd,
	glm::ivec2 destStart,
	glm::ivec2 destEnd,
	const GLenum filterMethod)
{

	glBindFramebuffer(GL_READ_FRAMEBUFFER, from);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to);
	glBlitFramebuffer(0, 0, SCREENWIDTH, SCREENHEIGHT, 0, 0, SCREENWIDTH, SCREENHEIGHT,
		type, filterMethod);
}

void Renderer::ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}
void Renderer::ClearColor(const glm::vec4& color)
{
	glClearColor(color.x, color.y, color.z, color.w);
}
void Renderer::Clear(GLenum type)
{
	glClear(type);
}