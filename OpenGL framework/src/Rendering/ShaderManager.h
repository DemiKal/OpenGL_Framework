#pragma once
//#include <string>
//#include <iostream>
//#include <filesystem>

#include "GPUShader.h"

class ShaderManager
{
private:

	std::unordered_map<std::string, std::tuple<ShaderType, std::string>> m_vertexShaderSources;
	std::unordered_map<std::string, std::tuple<ShaderType, std::string>> m_fragmentShaderSources;

	void loadShaders(const std::string& shaderDirectory)
	{
		for (const auto& entry : std::filesystem::directory_iterator(shaderDirectory))
		{
			std::cout << entry.path() << std::endl;
			const std::string p = entry.path().string();
			const  std::string name = std::filesystem::path(p).stem().string();


			//get extension
			std::string filePath = entry.path().string();
			size_t dot = p.find_last_of(".");
			std::string extension;

			if (dot != std::string::npos)
				extension = p.substr(dot + 1);

			ShaderType type = ShaderType::NONE;
			const std::string sourceCode = ParseShader(filePath);

			if (extension == "vert")
			{
				type = ShaderType::VERTEX;
				m_vertexShaderSources[name] = std::tie(type, sourceCode);
			}

			if (extension == "frag")
			{
				type = ShaderType::FRAGMENT;
				m_fragmentShaderSources[name] = std::tie(type, sourceCode);
			}




			//if (extension = "vert") type = ShaderType::VERTEX;
			//ShaderSources.emplace_back(  );
			//shaders.emplace_back(GPUShader(p, type));
		}

		for (auto& [vertKey, vertVal] : m_vertexShaderSources)
		{
			auto& [type, vertSrc] = vertVal;

			if (m_fragmentShaderSources.find(vertKey) != m_fragmentShaderSources.end())
			{
				auto& [name2, fragSrc] = m_fragmentShaderSources[vertKey];

				shaders.emplace_back(GPUShader(vertKey, vertSrc, fragSrc));

			}
			else continue;
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
	static GPUShader& GetShader(const std::string& name);
	static GPUShader& GetShader(const unsigned int idx);
	static unsigned int GetShaderIdx(const std::string& _name);

	static void Init(const std::string& shaderDirectory = "res/shaders")
	{
		auto& instance = getInstance();
		instance.loadShaders(shaderDirectory);
		instance.initialized = true;
	}

	static ShaderManager& getInstance()
	{
		static std::unique_ptr< ShaderManager > instance = std::make_unique<ShaderManager>();
		return *instance;
	}


	static void Destroy();


	std::string ParseShader(const std::string& path) const;

};

