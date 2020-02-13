#include "precomp.h"


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

GPUShader& ShaderManager::GetShader(const std::string& name) 
{
	const unsigned int idx = getShaderIdx(name);
	return getShaderIdx(idx);
}

unsigned int ShaderManager::getShaderIdx(const std::string& _name)
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

GPUShader& ShaderManager::getShaderIdx(const unsigned idx)
{
	auto& inst = getInstance();
	return inst.shaders[idx];
}
