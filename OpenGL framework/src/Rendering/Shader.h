#pragma once






class Shader {

	struct uniform_info_t
	{
		GLint location;
		GLsizei count;
	};
public:
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2, TESSELATION = 3
	};

	Shader();
	Shader(const std::string& path, const std::string& vertexSrc, const std::string& fragSrc);
	Shader(const std::string& filepath, const ShaderType shaderType);

	std::string GetName() const;
	unsigned int GetID() const;
	int GetUniformLocation(const std::string& name);

	static unsigned int CompileShader(const unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	void SetInt(const std::string& name, int value);
	void SetupUniforms();
	void SetVec4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const char* name, const glm::mat4& mat);
	void SetVec4f(const std::string& name, const glm::vec4& value);
	void SetVec3f(const std::string& name, const glm::vec3& value);
	void SetFloat(const std::string& name, float value);

	void Destroy() const;
	void Bind() const;
	static void Unbind();

private:
	unsigned int m_RendererID;
	ShaderType m_shaderType;
	std::string m_name;
	std::string m_FilePath;
	std::unordered_map<std::string, uniform_info_t> m_uniformsInfo;
};

