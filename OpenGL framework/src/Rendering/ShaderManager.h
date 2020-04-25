#pragma once
//#include <string>
//#include <iostream>
//#include <filesystem>

#include "Shader.h"
enum shaderTypeFlags
{
	POSITION,
	NORMAL,
	UV,
	BASIC,
	ERRORSHADER,
};
using shaderMap = std::unordered_map<std::string, std::tuple<ShaderType, std::string>>;

class ShaderManager
{
private:

	shaderMap m_vertexShaderSources;
	shaderMap m_fragmentShaderSources;

	void LoadShaders(const std::string& shaderDirectory);

public:

	std::string directory;
	bool initialized = false;
	std::vector< Shader > shaders;

	ShaderManager() = default;
	static Shader& GetShader(const std::string& name);
	static void Init(const std::string& shaderDirectory = "res/shaders");

	static unsigned int GetShaderIdx(const std::string& name);
	static ShaderManager& GetInstance();
	static Shader& GetShader(const unsigned int idx);
	static void Destroy();
	static std::string ParseShader(const std::string& path);

};

