#include "Renderer.h"
#include <ostream>
#include <GL/glew.h>
#include <iostream>


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
	GLCall(glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}	
