#pragma once

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class GPUShader
{
public:

	std::string m_FilePath;
	std::string name;
	unsigned int m_RendererID = 0;
	std::unordered_map<  std::string, int> m_UniformLocationCache;

	int GetUniformLocation(const std::string& name);
	ShaderProgramSource parseShader(const std::string& path) const;


	//Shader();
	GPUShader(const std::string& filepath);
	 
	~GPUShader();
	GPUShader(); 
	void Bind() const;
	void Unbind() const;
	void SetUniform1i(const std::string& name, int value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	unsigned int CompileShader(const unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	void SetUniformMat4f(const char* name, const glm::mat4& mat);
	void setVec3(const std::string & name, const glm::vec3 & value);
	void SetFloat(const std::string& name, float value);
	 
};

