#pragma once
 

 

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
public:

	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;

	int GetUniformLocation(const std::string& name);
	ShaderProgramSource parseShader(const std::string& path) const;


	//Shader();
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;
	void SetUniform1i(const std::string& name, int i);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)  ;
	unsigned int CompileShader(unsigned type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	void SetUniformMat4f(const char* name, const glm::mat4& mat);
	void setVec3(const std::string & name, const glm::vec3 & value)  ;
	void SetFloat(const std::string& name, float value)  ;
	Shader& operator=(Shader* shader);
};

