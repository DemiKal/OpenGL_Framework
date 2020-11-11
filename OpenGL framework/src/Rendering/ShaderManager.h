#pragma once
#include "Shader.h"


class ShaderManager
{
	enum shaderTypeFlags
	{
		POSITION,
		NORMAL,
		UV,
		BASIC,
		ERRORSHADER,
	};
	
	using shaderMap = std::unordered_map<std::string, std::tuple<Shader::ShaderType, std::string>>;
	
	


private:
	shaderMap m_vertexShaderSources;
	shaderMap m_fragmentShaderSources;
	shaderMap m_computeShaders;

	void LoadShaders(const std::string& shaderDirectory);

public:
	static void Init(const std::string& shaderDirectory = "res/shaders");
	static void Destroy();

	[[nodiscard]] static unsigned int GetShaderIdx(const std::string& name);
	[[nodiscard]] static ShaderManager& GetInstance();
	[[nodiscard]] static Shader& GetShader(const unsigned int idx);
	[[nodiscard]] static std::string ParseShader(const std::string& path);
	[[nodiscard]] static Shader& GetShader(const std::string& name);

	std::string directory;
	bool initialized = false;
	std::vector<Shader> shaders;

	ShaderManager() = default;


};

