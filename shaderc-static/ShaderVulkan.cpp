#include "pch.h"
#include "ShaderVulkan.h"
shaderc::Compiler;
shaderc::CompileOptions options;
shaderc::SpvCompilationResult vertShaderResult;

int main(void)
{
	return 0;
}
void ShaderVulkan::LoadShader(const std::string& path)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);
	
	shaderc::SpvCompilationResult vertShaderResult =
		compiler.CompileGlslToSpv(path, shaderc_glsl_vertex_shader, "vertex shader", options);
	int a = 1;
}
