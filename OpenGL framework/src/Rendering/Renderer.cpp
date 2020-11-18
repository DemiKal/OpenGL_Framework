#include "precomp.h"
#include "Renderer.h"

#include <glm/common.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/compatibility.hpp>

#include "Buffer/IndexBuffer.h"
#include "Buffer/VertexArray.h"
#include "Shader.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Buffer/FrameBuffer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/Model.h"

ScreenQuad Renderer::screenQuad;
GLFWwindow* Renderer::m_Window;

void Renderer::ShutDown()
{
	glfwDestroyWindow(m_Window);

	glfwTerminate();
}

void Renderer::DrawLine(const glm::mat4& model, const Camera& cam, const glm::vec3& a, const glm::vec3& b)
{
	//GLCall(glBindBuffer(GL_ARRAY_BUFFER, lineVBO));
	std::vector<glm::vec3> data = { a, b };
	Shader& shader = ShaderManager::GetShader("single_line");
	shader.Bind();
	shader.SetUniformMat4f("u_projection", cam.GetProjectionMatrix());
	shader.SetUniformMat4f("u_view", cam.GetViewMatrix());

	static float rot = 0;
	//rot += 0.01f;
	//mat4 rotMat = rotate(mat4(1.0f), rot, { 0,1,0 });
	//const mat4& mat = model.GetModelMatrix();
	shader.SetUniformMat4f("u_model", model);

	//GLfloat lineWidthRange[2] = { 0.0f, 0.0f };
	//glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
	GLCall(glLineWidth(5.0f));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, lineVBO));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * 2, &data[0]));

	GLCall(glBindVertexArray(lineVAO));
	GLCall(glDrawArrays(GL_LINES, 0, 2));  
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
	
	GLCall(glLineWidth(1.0f));
	Shader::Unbind();
}

void Renderer::CreateLine()
{
	//auto lineVerts = { glm::vec3(0,0,0), glm::vec3(1,0,0) };
	//cubeIndices = { 0,1 };

	GLCall(glGenVertexArrays(1, &lineVAO));
	GLCall(glGenBuffers(1, &lineVBO));

	GLCall(glBindVertexArray(lineVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, lineVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, nullptr, GL_DYNAMIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr));
	GLCall(glVertexAttribDivisor(1, 1));
	//GLCall(glGenBuffers(1, &cubeEBO));
	//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO));
	//GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), &cubeIndices[0], GL_STATIC_DRAW));


	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
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

// 
/**
 * \brief Set culling mode for triangles
 * \param cullingMode: GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
 */
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

void Renderer::Init()
{
	if (!glfwInit()) throw std::exception("Cant init glfw");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE); //GL_FALSE GL_TRUE

	//glfwWindowHint(GLFW_FULLSCREEN, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	 
	//glfwGetPrimaryMonitor()
	/* Create a windowed mode window and its OpenGL context */
	//auto xx = glfwGetPrimaryMonitor();
	m_Window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Hello World", nullptr, nullptr) ;

	if (!m_Window)
	{
		glfwTerminate();
		throw std::exception("ERROR: Could not create GLFW window!");
		//return -1;
	}

	//InputManager::SetWindow(m_Window);
	glfwMakeContextCurrent( m_Window );

	if (glewInit() != GLEW_OK) fmt::print("ERROR!\n");

	HardwareQuery::Query();
}



Renderer::Renderer()
{
	CreateCubeMesh();
	CreateTriangle();
	CreateLine();
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
	SetAlphaBlending(alphaBlend);
	SetVSync(m_VSync);

	const float average = (float)std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.0) / m_frameTimes.size();
	const float avgFPS = 1000.0f * static_cast<float>(1.0f / average);

	//ImGui::Text("FPS: %f, %f ms", avgFPS, average);


	if (m_frameTimes.size() >= 100) m_frameTimes.clear();


	//ImGui::Checkbox("Alpha Blend", &alphaBlend);
	//ImGui::Checkbox("VSync", &m_VSync);

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

void Renderer::SwapBuffers()
{
	GLCall(glfwPollEvents());
	GLCall(glfwSwapBuffers(GetWindow()));
}
void Renderer::SwapBuffers(GLFWwindow* window)
{
	GLCall(glfwPollEvents());
	GLCall(glfwSwapBuffers(window));
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

bool Renderer::GetAlphaBlending() const
{
	return m_alphaBlending;
}

uint32_t Renderer::GetCubeVAO() const
{
	return cubeVAO;
}

GLenum Renderer::GetDepthFunc() const
{
	return m_depthFunction;
}

GLenum Renderer::GetCullingMode() const
{
	return m_cullingMode;
}

void Renderer::ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}
void Renderer::ClearColor(const glm::vec4& color)
{
	glClearColor(color.x, color.y, color.z, color.w);
}


/**
 * \brief clear bits of the buffer
 * \param type OR-mask:  GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_ACCUM_BUFFER_BIT, and GL_STENCIL_BUFFER_BIT
 */
void Renderer::Clear(GLenum type)
{
	glClear(type);
}

GLFWwindow* Renderer::GetWindow()
{
	return m_Window;
}