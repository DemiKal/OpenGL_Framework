#pragma once
#include <shaderc/shaderc.hpp>
shaderc::Compiler;
shaderc::CompileOptions;
shaderc::SpvCompilationResult;

struct ShaderVulkan
{
	
	static void LoadShader(const std::string& path);
};

