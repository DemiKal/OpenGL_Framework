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
	glfwDestroyWindow(m_Window);
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
	Shader& shader = ShaderManager::GetShader("AABB_single");
	shader.Bind();
	
	//shader.SetVec4f("u_color", glm::vec4(1.0f, 0.75f, 0.5f, 1.0f));
	shader.SetUniformMat4f("u_Model", transform);
	shader.SetUniformMat4f("u_View", cam.GetViewMatrix());
	shader.SetUniformMat4f("u_Projection", cam.GetProjectionMatrix());
	shader.SetVec4f("u_Color", color.x, color.y, color.z, color.w);
	
	Mesh& cube = MeshManager::GetMesh(1);//wireframe cube!
	cube.Draw(shader);
	//glBindVertexArray(cubeVAO);
	//
	//GLCall(glDrawElements(GL_LINES, static_cast<GLsizei>(cubeIndices.size()), GL_UNSIGNED_INT, nullptr))
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

void Renderer::DisableDepth()
{
	if (!m_DepthTest) return;
	m_DepthTest = false;
	GLCall(glDisable(GL_DEPTH_TEST))
}

void Renderer::EnableDepth()
{
	if (m_DepthTest) return;
	m_DepthTest = true;
	GLCall(glEnable(GL_DEPTH_TEST))
}

void Renderer::Init(std::vector<std::tuple<int, int>> windowHints)
{
	if (m_Initialized) throw std::exception("Renderer already initialized!");

	if (!glfwInit()) throw std::exception("Cant init glfw!");

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GL_TRUE); //GL_FALSE GL_TRUE

	for (auto [key, val] : windowHints) glfwWindowHint(key, val);





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

	GLint icc;
	glGetIntegerv(GL_CULL_FACE_MODE, &icc);
	m_CullingMode = icc;

	GLint sa;
	glGetIntegerv(GL_NUM_EXTENSIONS, &sa);


	GLint viewpDim[4];
	glGetIntegerv(GL_VIEWPORT, &viewpDim[0]);

	m_ViewPort = QueryGLViewPort();

	fmt::print("OpenGL extensions loaded:\n");
	for (GLuint i = 0; i < sa; i++)
	{
		const GLubyte* ext = glGetStringi(GL_EXTENSIONS, i);
		fmt::print("{0} {1}\n", i, static_cast<const unsigned char*>(ext));
	}

	m_Initialized = true;
}

glm::ivec4 Renderer::GetViewPort() const
{
	return m_ViewPort;
}

glm::ivec4 Renderer::QueryGLViewPort()
{
	glm::ivec4 viewpDim{};
	glGetIntegerv(GL_VIEWPORT, &viewpDim[0]);

	return viewpDim;
}

void Renderer::SetViewPort(const int startX, const int startY, const int width, const int height)
{
	SetViewPort({ startX , startY, width, height });
}

void Renderer::SetViewPort(const glm::ivec4& viewport)
{
	if (viewport != m_ViewPort)
	{
		m_ViewPort = viewport;
		glViewport(m_ViewPort.x, m_ViewPort.y, m_ViewPort.z, m_ViewPort.w);
	}
}

Renderer::Renderer()
{
	//CreateCubeMesh();
	//CreateTriangle();
	//CreateLine();
	//CreatePlane();
	//screenQuad.Init();
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
	if (alphaValue == m_AlphaBlending) return;

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
void Renderer::SetVSync(const bool cond)
{
	if (cond != m_VSync) m_VSync = cond;

	GLCall(glfwSwapInterval(m_VSync))
}

bool Renderer::GetVSync() const
{
	return m_VSync;
}


void Renderer::BlitTexture(FrameBuffer& frameBuffer, std::optional<FrameBuffer> target)
{
	const GLuint targetID = target ? target->GetID() : 0;
	glBindFramebuffer(GL_FRAMEBUFFER, targetID);

	ShaderManager::GetShader("framebuffer_screen").Bind();
	frameBuffer.GetTexture().Bind();
	DrawScreenQuad();
}

void Renderer::SwapBuffers() const
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
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
	glBlitFramebuffer(0, 0, SCREENWIDTH, SCREENHEIGHT, 0, 0, SCREENWIDTH, SCREENHEIGHT, type, filterMethod);
}

bool Renderer::GetAlphaBlending() const
{
	return m_AlphaBlending;
}

GLenum Renderer::GetDepthFunc() const
{
	return m_DepthFunction;
}

GLenum Renderer::GetCullingMode() const
{
	return m_CullingMode;
}

void Renderer::SetClearColor(const float r, const float g, const float b, const float a)
{
	glClearColor(r, g, b, a);
	m_ClearColor = { r,g,b,a };
}

void Renderer::SetClearColor(const glm::vec4& color)
{
	glClearColor(color.x, color.y, color.z, color.w);
	m_ClearColor = color;
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
	return  m_Window;
}



