#pragma once


struct uniform_info_t
{
	GLint location;
	GLsizei count;
};

enum class ShaderType
{
	NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2, TESSELATION = 3
};

class Shader
{
public:
	std::unordered_map<std::string, uniform_info_t> m_uniformsInfo;
	std::string m_FilePath;
	std::string m_name;
	ShaderType m_shaderType;
	unsigned int m_RendererID;

	Shader();
	Shader(const std::string& path, const std::string& vertexSrc, const std::string& fragSrc);
	Shader(const std::string& filepath, const ShaderType shaderType);

	int GetUniformLocation(const std::string& name);
	void SetInt(const std::string& name, int value);
	void SetupUniforms();

	
	static unsigned int CompileShader(const unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	void Bind() const;
	static void Unbind();
	void Destroy() const;
	
	void SetVec4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const char* name, const glm::mat4& mat);
	void SetVec4f(const std::string& name, const glm::vec4& value);
	void SetVec3f(const std::string& name, const glm::vec3& value);
	void SetFloat(const std::string& name, float value);
};

