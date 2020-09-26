#pragma once
#include "precomp.h"
#include "rendering/shader.h"

class ComputeShader : Shader
{
	unsigned int workGroupSize[2];

	ComputeShader(std::string&& src, unsigned int _workGroupSize[2])
	{

	}
};

