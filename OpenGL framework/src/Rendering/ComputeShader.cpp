#include "precomp.h"
#include "ComputeShader.h"
#include "GameObject/Components/Texture2D.h"

//template <typename func>
//void ComputeShader::Dispatch(func f)
//{
//	Bind();
//	f();
//	uint32_t w = m_AttachedImages[0].GetWidth();
//	uint32_t h = m_AttachedImages[0].GetHeight();
//	glDispatchCompute(w, h, 1);
//
//
//	//glUseProgram(0);
//	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//
//}



ComputeShader::ComputeShader(const std::string& shaderSrc, uint32_t width, uint32_t height)
{
	m_ShaderType = ShaderType::COMPUTE;
	const unsigned int program = glCreateProgram();
	const unsigned int cs = CompileShader(GL_COMPUTE_SHADER, shaderSrc);

	glAttachShader(program, cs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(cs);
	m_RendererID = program;

	SetupUniforms();
	GetAttributes();
	Texture2D attached;
	attached.GenImageTexture(width, height);

	m_AttachedImages.emplace_back(attached);
	m_Name = std::filesystem::path(shaderSrc).stem().string();
	m_FilePath = shaderSrc;


	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	GLint work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);


}

Texture2D& ComputeShader::GetComputeTexture(const uint32_t index)
{
	return m_AttachedImages[index];
}