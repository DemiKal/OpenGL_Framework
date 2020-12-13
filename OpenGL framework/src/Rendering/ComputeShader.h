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
};

