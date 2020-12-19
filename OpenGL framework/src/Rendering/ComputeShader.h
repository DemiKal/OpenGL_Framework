#pragma once
#include "GameObject/Components/Texture2D.h"
#include "Rendering/Shader.h"


class ComputeShader : public Shader
{
private:
	uint32_t m_workGroupSize[3] = { 0, 0, 0 };
	uint32_t m_Dimensions{ 0 };

	Texture2D m_ComputeTex;
	std::vector<Texture2D> m_AttachedImages;
public:
	ComputeShader(const std::string& filepath, uint32_t width, uint32_t height);
	Texture2D& GetComputeTexture(uint32_t index = 0);
	
	template <typename func>
	void Dispatch(func f)
	{
		Bind();
		f();
		m_AttachedImages[0].Bind();
		uint32_t w = m_AttachedImages[0].GetWidth();
		uint32_t h = m_AttachedImages[0].GetHeight();
		glDispatchCompute(w / 8 , h / 8, 1);


		//glUseProgram(0);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		m_AttachedImages[0].Unbind();
		Unbind();
	}

};

