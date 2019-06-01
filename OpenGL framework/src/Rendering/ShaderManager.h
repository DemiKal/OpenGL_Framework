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
			//auto shared = std::make_shared<GPUShader>(p);
			shaders.emplace_back(std::make_shared<GPUShader>(p));
		}
	}


public:

	std::string directory;
	bool initialized = false;
	std::vector<std::shared_ptr<GPUShader>> shaders;

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
	static std::shared_ptr<GPUShader> getShader(const unsigned int idx);
	//ShaderManager();
	//~ShaderManager();
	static void Init(const std::string& shaderDirectory)
	{
		auto  instance = getInstance();
		instance->loadShaders(shaderDirectory);
		instance->initialized = true;
	}

	static std::shared_ptr<ShaderManager>& getInstance()
	{
		static std::shared_ptr<ShaderManager> instance = std::make_shared<ShaderManager>();
		return instance;
	}


	//std::unique_ptr<GPUShader>& getShader(const std::string & _name);

};

