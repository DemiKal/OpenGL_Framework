#pragma once
#include <string>
#include <iostream>
#include <filesystem>

class ShaderManager
{
private:
	void loadShaders(const std::string& shaderDirectory)
	{
		for (const auto & entry : std::filesystem::directory_iterator(shaderDirectory))
		{
			std::cout << entry.path() << std::endl;
			const std::string p = entry.path().string();
			shaders.emplace_back(GPUShader(p));
		}
	}

public:

	std::string directory;
	bool initialized = false;
	std::vector< GPUShader > shaders;

	enum shaderTypeFlags
	{
		POSITION,
		NORMAL,
		UV,
		BASIC,
		ERRORSHADER,
	};

	ShaderManager() = default;
	static unsigned int getShaderIdx(const std::string& _name);
	static GPUShader& getShaderIdx(const unsigned int idx);

	static void Init(const std::string& shaderDirectory  = "res/shaders")
	{
		auto&  instance = getInstance();
		instance.loadShaders(shaderDirectory);
		instance.initialized = true;
	}

	static ShaderManager& getInstance()
	{
		static  ShaderManager instance;
		return instance;
	}
	static void Destroy();


};

