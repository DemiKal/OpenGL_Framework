#include "precomp.h"
#include "Renderer.h"
#include "Buffer/IndexBuffer.h"
#include "Buffer/VertexArray.h"
#include "Shader.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Buffer/FrameBuffer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/MeshManager.h"
#include "GameObject/Components/Model.h"

ScreenQuad Renderer::screenQuad;

void Renderer::ShutDown()
{
	glfwDestroyWindow(GetInstance().m_Window);
	glfwTerminate();
}

//render = the one used to render. debug = one used to extract frustum planes from
void Renderer::DrawFrustum(const Camera& renderCam, Camera& debugCam, const glm::vec4& color)
{
	float fov = debugCam.GetFieldOfView();
	float aspect = debugCam.GetAspectRatio();
	float near = debugCam.GetNearPlaneDist();
	float far = debugCam.GetFarPlaneDist();
	glm::vec3  p = debugCam.GetPosition();
	glm::vec3 up = debugCam.GetUpVector();
	glm::vec3 forward = debugCam.GetForwardVector();
	glm::vec3 right = glm::cross(forward, up);

	const float Hnear = 2 * glm::tan(glm::radians(fov) / 2.0f) * near;
	const float Wnear = Hnear * aspect;

	const float Hfar = 2 * glm::tan(glm::radians(fov) / 2.0f) * far;
	const float Wfar = Hfar * aspect;

	glm::vec3 fc = p + far * forward;

	glm::vec3 ftl = fc + (up * Hfar / 2.0f) - (right * Wfar / 2.0f);
	glm::vec3 ftr = fc + (up * Hfar / 2.0f) + (right * Wfar / 2.0f);
	glm::vec3 fbl = fc - (up * Hfar / 2.0f) - (right * Wfar / 2.0f);
	glm::vec3 fbr = fc - (up * Hfar / 2.0f) + (right * Wfar / 2.0f);

	auto nc = p + near * forward;

	glm::vec3 ntl = nc + (up * Hnear / 2.0f) - (right * Wnear / 2.0f);
	glm::vec3 ntr = nc + (up * Hnear / 2.0f) + (right * Wnear / 2.0f);
	glm::vec3 nbl = nc - (up * Hnear / 2.0f) - (right * Wnear / 2.0f);
	glm::vec3 nbr = nc - (up * Hnear / 2.0f) + (right * Wnear / 2.0f);

	DrawLine(glm::mat4(1.0f), renderCam, ntl, ntr, color);
	DrawLine(glm::mat4(1.0f), renderCam, ntr, nbr, color);
	DrawLine(glm::mat4(1.0f), renderCam, nbr, nbl, color);
	DrawLine(glm::mat4(1.0f), renderCam, nbl, ntl, color);

	DrawLine(glm::mat4(1.0f), renderCam, ftl, ftr, color);
	DrawLine(glm::mat4(1.0f), renderCam, ftr, fbr, color);
	DrawLine(glm::mat4(1.0f), renderCam, fbr, fbl, color);
	DrawLine(glm::mat4(1.0f), renderCam, fbl, ftl, color);

	DrawLine(glm::mat4(1.0f), renderCam, ntl, ftl, color);
	DrawLine(glm::mat4(1.0f), renderCam, ntr, ftr, color);
	DrawLine(glm::mat4(1.0f), renderCam, nbl, fbl, color);
	DrawLine(glm::mat4(1.0f), renderCam, nbr, fbr, color);

}


void Renderer::DrawLine(const glm::mat4& model, const Camera& cam, const glm::vec3& a, const glm::vec3& b,
	const glm::vec4& color)
{
	//GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO));
	std::vector<glm::vec3> data = { a, b };
	Shader& shader = ShaderManager::GetShader("lineshader");

	shader.Bind();
	shader.SetUniformMat4f("u_Model", model);
	shader.SetUniformMat4f("u_Projection", cam.GetProjectionMatrix());
	shader.SetUniformMat4f("u_View", cam.GetViewMatrix());
	shader.SetVec4f("u_Color", color);

	//GLfloat lineWidthRange[2] = { 0.0f, 0.0f };
	//glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
	auto [lineVAO, lineVBO] = MeshManager::GetLineBuffer();
	GLCall(glLineWidth(5.0f))
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, lineVBO))
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * 2, &data[0]))

		GLCall(glBindVertexArray(lineVAO))
		GLCall(glDrawArrays(GL_LINES, 0, 2))
		GLCall(glBindVertexArray(0))
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0))
		GLCall(glActiveTexture(GL_TEXTURE0))

		GLCall(glLineWidth(1.0f))
		Shader::Unbind();
}

void Renderer::CreateCubeMesh()
{
	/*cubeVertices = {
		{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f,0.5f, 0.5f},
		{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f,0.5f, -0.5f},
	};

	cubeIndices = {
	   0,1, 1,2, 2,3, 3,0,
	   1,5, 5,6, 6,2,
	   5,4, 6,7,
	   4,7, 0,4, 3,7
	};

	GLCall(glGenVertexArrays(1, &cubeVAO))
		GLCall(glGenBuffers(1, &cubeVBO))

		GLCall(glBindVertexArray(cubeVAO))
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO))
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * cubeVertices.size(), &cubeVertices[0], GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0))
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr))
		GLCall(glVertexAttribDivisor(1, 1))
		GLCall(glGenBuffers(1, &cubeEBO))
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int),
		&cubeIndices[0], GL_STATIC_DRAW))


		GLCall(glBindVertexArray(0))
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0))
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0))*/
}

void Renderer::CreateTriangle()
{

}

void Renderer::CreatePlane()
{

}

void Renderer::DrawInstancedCubes(const GLsizei instanceCount) const
{
	//	GLCall(glBindVertexArray(cubeVAO))
	//		const GLsizei indxSize = static_cast<GLsizei>(cubeIndices.size());
	//	GLCall(glDrawElementsInstanced(GL_LINES, indxSize, GL_UNSIGNED_INT, nullptr, instanceCount))
	//		GLCall(glBindVertexArray(0))
	//		GLCall(glActiveTexture(GL_TEXTURE0))
}


void Renderer::DrawCube(const Camera& cam, const glm::mat4& transform, const glm::vec4 color) const
{
	//TODO: use meshmanager?
	//auto& shader = ShaderManager::GetShader("AABB_single");
	//shader.Bind();
	//
	////shader.SetVec4f("u_color", glm::vec4(1.0f, 0.75f, 0.5f, 1.0f));
	//shader.SetUniformMat4f("u_Model", transform);
	//shader.SetUniformMat4f("u_View", cam.GetViewMatrix());
	//shader.SetUniformMat4f("u_Projection", cam.GetProjectionMatrix());
	//shader.SetVec4f("u_Color", color.x, color.y, color.z, color.w);
	//
	//glBindVertexArray(cubeVAO);
	//
	//GLCall(glDrawElements(GL_LINES,
	//	static_cast<GLsizei>(cubeIndices.size()),
	//	GL_UNSIGNED_INT, nullptr))
	//
	//	GLCall(glBindVertexArray(0));
	//GLCall(glActiveTexture(GL_TEXTURE0))
}

void Renderer::Enable(GLenum type)
{
	GLCall(glEnable(type))
}

//void Renderer::EnableDepth()//TODO: add some cached binding stuff
//{
//	Enable(GL_DEPTH_TEST);
//}

void Renderer::SetDepthFunc(const GLenum depthFunc)
{

	if (depthFunc == m_DepthFunction)
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

	m_DepthFunction = depthFunc;
	GLCall(glDepthFunc(depthFunc))
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
	if (m_CullingMode == cullingMode)
	{
		//fmt::print("Same culling mode is already set\n");
		return;
	}

	if (cullingMode != GL_FRONT && cullingMode != GL_BACK && cullingMode != GL_FRONT_AND_BACK)
	{
		const auto a = PRINTAPI(GL_FRONT)
			const auto b = PRINTAPI(GL_BACK)
			const auto c = PRINTAPI(GL_FRONT_AND_BACK)
			fmt::print("invalid culling mode! Only {0}, {1}, {2} are valid\n", a, b, c);
		return;
	}

	m_CullingMode = cullingMode;
	GLCall(glCullFace(m_CullingMode))
}

void Renderer::_DisableDepth()
{
	if (!m_DepthTest) return;
	m_DepthTest = false;
	GLCall(glDisable(GL_DEPTH_TEST))
}

void Renderer::_EnableDepth()
{
	if (m_DepthTest) return;
	m_DepthTest = true;
	GLCall(glEnable(GL_DEPTH_TEST))
}
void Renderer::DisableDepth()
{
	GetInstance()._DisableDepth();
}

void Renderer::EnableDepth()
{
	GetInstance()._EnableDepth();
}


void Renderer::_Init()
{
	if (!glfwInit()) throw std::exception("Cant init glfw");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE); //GL_FALSE GL_TRUE

	//glfwWindowHint(GLFW_FULLSCREEN, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//glfwGetPrimaryMonitor()
	/* Create a windowed mode window and its OpenGL context */
	//auto xx = glfwGetPrimaryMonitor();
	m_Window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Hello World", nullptr, nullptr);

	if (!m_Window)
	{
		glfwTerminate();
		throw std::exception("ERROR: Could not create GLFW window!");
		//return -1;
	}

	//InputManager::SetWindow(m_Window);
	glfwMakeContextCurrent(m_Window);

#ifdef USEGLEW
	if (glewInit() != GLEW_OK) fmt::print("ERROR!\n");
#else
	const int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!gladStatus)
		fmt::print("Glad not loaded!");
#endif	
	HardwareQuery::Query();
	//m_DepthTest = false;
	//SetAlphaBlending(m_AlphaBlending);
	GLboolean v;
	glGetBooleanv(GL_BLEND, &v);
	m_AlphaBlending = v;

	glGetBooleanv(GL_DEPTH_TEST, &v);
	m_DepthTest = v;

	glGetBooleanv(GL_CULL_FACE, &v);
	m_Cull = v;

	GLint i;
	glGetIntegerv(GL_CULL_FACE_MODE, &i);
	m_CullingMode = i;

}

Renderer::Renderer()
{
	//CreateCubeMesh();
	//CreateTriangle();
	//CreateLine();
	//CreatePlane();
	//screenQuad.Init();
}

void Renderer::UpdateUI(float deltaTime)
{
	//SetAlphaBlending(alphaBlend);
	//SetVSync(m_VSync);
	//
	//const float average = (float)std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.0) / m_frameTimes.size();
	//const float avgFPS = 1000.0f * static_cast<float>(1.0f / average);
	//
	////ImGui::Text("FPS: %f, %f ms", avgFPS, average);
	//
	//
	//if (m_frameTimes.size() >= 100) m_frameTimes.clear();
	//
	//
	////ImGui::Checkbox("Alpha Blend", &alphaBlend);
	////ImGui::Checkbox("VSync", &m_VSync);
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
	GetInstance()._SetAlphaBlending(alphaValue);
}

void Renderer::_SetAlphaBlending(const bool alphaValue)
{
	if (alphaValue == m_AlphaBlending) return;
	//m_AlphaBlending = alphaValue;

	if (alphaValue)
	{
		m_AlphaBlending = true;
		GLCall(glEnable(GL_BLEND))
			GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA))
	}
	else
	{
		GLCall(glDisable(GL_BLEND))
			m_AlphaBlending = false;
	}

}
void Renderer::_SetVSync(const bool cond)
{
	if (cond != m_VSync) m_VSync = cond;

	GLCall(glfwSwapInterval(m_VSync))
}

bool Renderer::GetVsync()
{
	return GetInstance().m_VSync;
}
void Renderer::SetVSync(const bool cond)
{
	GetInstance()._SetVSync(cond);

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
	GLCall(glfwPollEvents())
		GLCall(glfwSwapBuffers(GetWindow()))
}

void Renderer::SwapBuffers(GLFWwindow* window)
{
	GLCall(glfwPollEvents())
		GLCall(glfwSwapBuffers(window))
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

bool Renderer::GetAlphaBlending()
{
	return GetInstance().m_AlphaBlending;
}

GLenum Renderer::GetDepthFunc()
{
	return GetInstance().m_DepthFunction;
}

GLenum Renderer::GetCullingMode()
{
	return GetInstance().m_CullingMode;
}

void Renderer::SetClearColor(const float r, const float g, const float b, const float a)
{
	glClearColor(r, g, b, a);
}

void Renderer::SetClearColor(const glm::vec4& color)
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
	return GetInstance().m_Window;
}

Renderer& Renderer::GetInstance()
{
	static Renderer instance;
	return instance;
}

void Renderer::Init()
{
	GetInstance()._Init();
}