#include "precomp.h"
#include "Renderer.h"
#include "Buffer/IndexBuffer.h"
#include "Buffer/VertexArray.h"
#include "Shader.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Buffer/FrameBuffer.h"
#include "GameObject/Camera.h"

ScreenQuad Renderer::screenQuad;

void Renderer::CreateCubeMesh()
{
	cubeVertices = {
		{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f,0.5f, 0.5f},
		{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f,0.5f, -0.5f},
	};

	cubeIndices = {
	   0,1, 1,2, 2,3, 3,0,
	   1,5, 5,6, 6,2,
	   5,4, 6,7,
	   4,7, 0,4, 3,7
	};

	GLCall(glGenVertexArrays(1, &cubeVAO));
	GLCall(glGenBuffers(1, &cubeVBO));

	GLCall(glBindVertexArray(cubeVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * cubeVertices.size(), &cubeVertices[0], GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr));
	GLCall(glVertexAttribDivisor(1, 1));
	GLCall(glGenBuffers(1, &cubeEBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int),
		&cubeIndices[0], GL_STATIC_DRAW));


	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Renderer::CreateTriangle()
{

}

void Renderer::CreatePlane()
{

}

void Renderer::DrawInstancedCubes(const GLsizei instanceCount) const
{
	GLCall(glBindVertexArray(cubeVAO));
	const GLsizei indxSize = static_cast<GLsizei>(cubeIndices.size());
	GLCall(glDrawElementsInstanced(GL_LINES, indxSize, GL_UNSIGNED_INT, nullptr, instanceCount));
	GLCall(glBindVertexArray(0));
	GLCall(glActiveTexture(GL_TEXTURE0));
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

	GLCall(glBindVertexArray(0));
	GLCall(glActiveTexture(GL_TEXTURE0));
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

void Renderer::DrawScreenQuad()
{
	screenQuad.Bind(); //TODO use some binding management!
	screenQuad.Draw();
}

void Renderer::SetCullingMode(const GLenum cullingMode)
{
	if (m_cullingMode == cullingMode)
	{
		//fmt::print("Same culling mode is already set\n");
		return;
	}

	if (cullingMode != GL_FRONT && cullingMode != GL_BACK && cullingMode != GL_FRONT_AND_BACK)
	{
		const auto a = PRINTAPI(GL_FRONT);
		const auto b = PRINTAPI(GL_BACK);
		const auto c = PRINTAPI(GL_FRONT_AND_BACK);
		fmt::print("invalid culling mode! Only {0}, {1}, {2} are valid\n", a, b, c);
		return;
	}

	m_cullingMode = cullingMode;
	GLCall(glCullFace(m_cullingMode));
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
	screenQuad.Init();
}

void Renderer::CalcFrameTime(float deltaTime)
{
	m_prevFrameTime = m_currentFrameTime;
	const double endtime = glfwGetTime();
	const double diffms = 1000 * (endtime - m_currentFrameTime);
	m_frameTimes.push_back(static_cast<float>(diffms));
	m_totalTime += deltaTime;

}
void Renderer::UpdateUI(float deltaTime)
{
	SetAlphaBlending( alphaBlend);
	SetVSync( vsync);

	const float average = (float)std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.0) / m_frameTimes.size();
	const float avgFPS = 1000.0f * static_cast<float>(1.0f / average);

	ImGui::Text("FPS: %f, %f ms", avgFPS, average);


	if (m_frameTimes.size() >= 100) m_frameTimes.clear();


	ImGui::Checkbox("Alpha Blend", &alphaBlend);
	ImGui::Checkbox("VSync", &vsync);

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
}

void Renderer::SetAlphaBlending(const bool alphaValue)
{
	m_alphaBlending = alphaValue;

	if (alphaValue)
	{
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}
	else { GLCall(glDisable(GL_BLEND)); }


}

void Renderer::SetVSync(const bool cond)
{
	if (cond != m_VSync) m_VSync = cond;

	GLCall(glfwSwapInterval(m_VSync));
}

void Renderer::BlitTexture(FrameBuffer& frameBuffer, std::optional<FrameBuffer> target)
{
	const GLuint targetID = target ? target->GetID() : 0;
	glBindFramebuffer(GL_FRAMEBUFFER, targetID);

	ShaderManager::GetShader("framebuffer_screen").Bind();
	frameBuffer.GetTexture().Bind();
	DrawScreenQuad();
}

void Renderer::SwapBuffers(GLFWwindow* window)
{
	GLCall(glfwSwapBuffers(window));
	GLCall(glfwPollEvents());
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