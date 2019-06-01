#include "precomp.h"


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

void Renderer::Clear()
{
	GLCall(glClearColor(0.2f, 0.3f, 0.6f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const GPUShader& shader)
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	//glDrawElementsBaseVertex(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0, 0);
}

void Renderer::SetAlphaBlending(bool set)
{
	alphaBlending = set;

	if (set)
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}
	else { GLCall(glDisable(GL_BLEND)); }


}
