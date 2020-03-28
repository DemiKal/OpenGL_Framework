#pragma once

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

struct uniform_info_t
{
	GLint location;
	GLsizei count;
};

class GPUShader
{
public:
	std::unordered_map<std::string, uniform_info_t> m_uniformsInfo;
	std::string m_FilePath;
	std::string name;
	unsigned int m_RendererID = 0;

	int GetUniformLocation(const std::string& name);
	ShaderProgramSource parseShader(const std::string& path) const;
	void SetInt(const std::string& name, int value);

	//Shader();
	GPUShader(const std::string& filepath);

	void SetupUniforms();

	GPUShader();
	void Bind() const;
	void Unbind() const;
	unsigned int CompileShader(const unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	void SetUniform1i(const std::string& name, int value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const char* name, const glm::mat4& mat);
	void SetVec4f(const std::string& name, const glm::vec4& value);
	void setVec3(const std::string& name, const glm::vec3& value);
	void SetFloat(const std::string& name, float value);
	//void SetUniformStruct(const std::string& name, void* myStruct);

	void Destroy();
};

