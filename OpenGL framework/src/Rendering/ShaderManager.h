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
	shaderMap m_VertexShaderSources;	//.vert
	shaderMap m_FragmentShaderSources;	//.frag
	shaderMap m_GeometryShaderSources;	//.geom
	shaderMap m_ComputeShaders;			//.cs
	shaderMap m_ShaderHelperFiles;		//.glsl

	void LoadShaders(const std::string& shaderDirectory);

public:
	static void Init(const std::string& shaderDirectory);
	static void Destroy();
	static void RemoveComments(std::string& shaderText);

	[[nodiscard]] std::string& SearchAndReplace(std::string& shaderText, const std::string& replacedWord);
	[[nodiscard]] static unsigned int GetShaderIdx(const std::string& name);
	[[nodiscard]] static ShaderManager& GetInstance();
	[[nodiscard]] static Shader& GetShader(const unsigned int idx);
	[[nodiscard]] static std::string ParseShader(const std::string& path);
	[[nodiscard]] static Shader& GetShader(const std::string& name);

	std::string directory;
	std::vector<Shader> shaders;
	bool initialized = false;

	ShaderManager() = default;


};

