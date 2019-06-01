#include "precomp.h"


//ShaderManager::ShaderManager()
//{
//}
//
//
//ShaderManager::~ShaderManager()
//{
//}


unsigned int ShaderManager::getShaderIdx(const std::string& _name)  
{
	const auto & inst = getInstance();
	for (unsigned int i = 0; i < inst->shaders.size(); i++)
	{
		if (inst->shaders[i]->name == _name)
		{
			return  i;
		}
	}

	return 0;
}

std::shared_ptr<GPUShader> ShaderManager::getShader(const unsigned idx)
{
	auto inst = getInstance();
	return inst->shaders[idx];
}
