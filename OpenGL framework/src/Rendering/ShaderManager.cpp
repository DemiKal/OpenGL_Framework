#include "precomp.h"
#include "ShaderManager.h"

#include "Shader.h"


//ShaderManager::ShaderManager()
//{
//}
//
//


void ShaderManager::Destroy()
{
	auto& inst = getInstance();
	for (auto& s : inst.shaders)
		s.Destroy();
}

Shader& ShaderManager::GetShader(const std::string& name)
{
	const unsigned int idx = GetShaderIdx(name);
	auto& inst = getInstance();
	return inst.shaders[idx];
}

unsigned int ShaderManager::GetShaderIdx(const std::string& _name)
{
	const auto& inst = getInstance();
	for (unsigned int i = 0; i < inst.shaders.size(); i++)
	{
		if (inst.shaders[i].name == _name)
		{
			return  i;
		}
	}

	return 0;
}

Shader& ShaderManager::GetShader(const unsigned idx)
{
	auto& inst = getInstance();
	return inst.shaders[idx];
}

std::string ShaderManager::ParseShader(const std::string& path) const
{
	std::ifstream stream(path);
	std::string shaderString((std::istreambuf_iterator<char>(stream)),
		(std::istreambuf_iterator<char>()));

	return shaderString;
}
