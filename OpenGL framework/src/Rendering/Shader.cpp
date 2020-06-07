#include "precomp.h"
#include "Shader.h"

//TODO: extra shader type for manually constructed shader

Shader::Shader(const std::string& path, const std::string& vertexSrc, const std::string& fragSrc)
	:
	m_uniformsInfo(),
	name(std::filesystem::path(path).stem().string()),
	m_FilePath(path),
	m_RendererID(0),
	m_shaderType(ShaderType::NONE)
{
	m_RendererID = CreateShader(vertexSrc, fragSrc);	//, sps.FragmentSource);
	SetupUniforms();

}



Shader::Shader(const std::string& filepath, const ShaderType shaderType)
	:
	m_uniformsInfo(),
	m_FilePath(filepath),
	name("not yet loaded"),
	m_RendererID(0),
	m_shaderType(shaderType)
{

	//m_RendererID = CreateShader(sourceCode, );//, sps.FragmentSource);
	const  std::string nm = std::filesystem::path(filepath).stem().string();
	name = nm;

	SetupUniforms();
}

void Shader::SetupUniforms() {

	GLint uniform_count = 0;
	glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &uniform_count);

	if (uniform_count != 0)
	{
		GLint 	max_name_len = 0;
		GLsizei length = 0;
		GLsizei count = 0;
		GLenum 	type = GL_NONE;
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

		auto uniform_name = std::make_unique<char[]>(max_name_len);

		//std::unordered_map<std::string, uniform_info_t> m_uniformsInfo;

		for (GLint i = 0; i < uniform_count; ++i)
		{
			glGetActiveUniform(m_RendererID, i, max_name_len, &length, &count, &type, uniform_name.get());

			uniform_info_t uniform_info = {};
			uniform_info.location = glGetUniformLocation(m_RendererID, uniform_name.get());
			uniform_info.count = count;

			m_uniformsInfo.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));

		}
	}

}
Shader::Shader()
{
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLint l = GetUniformLocation(name);
	GLCall(glUniform1i(l, value));
}
void Shader::SetInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(int loc = GetUniformLocation(name));
	GLCall(glUniform4f(loc, v0, v1, v2, v3));
}
//TODO: ADD 4fv!
//void Shader::SetUniform4V(const std::string& name, float v0, float v1, float v2, float v3)
//{
//	GLCall(int loc = GetUniformLocation(name));
//	GLCall(glUniform4f(loc, v0, v1, v2, v3));
//}
int Shader::GetUniformLocation(const std::string& name)
{
	if (m_uniformsInfo.find(name) != m_uniformsInfo.end())
		return m_uniformsInfo[name].location;

	GLCall(const int location = glGetUniformLocation(m_RendererID, name.c_str()));

	if (location == -1)
		std::cout << "location is not set of uniform " << name << std::endl;

	uniform_info_t ut;
	ut.location = location;
	ut.count = 1; //? ?
	m_uniformsInfo[name] = ut;

	return location;
}

unsigned int Shader::CompileShader(const unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		auto message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile shader" <<
			(type == GL_VERTEX_SHADER ? "fragment" : "vertex") << " shader" << std::endl;

		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;

	}
	return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	const unsigned int program = glCreateProgram();
	const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));

	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

void Shader::SetUniformMat4f(const char* name, const glm::mat4& mat)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]));
}


void Shader::SetVec4f(const std::string& name, const glm::vec4& value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform4fv(location, 1, &value[0]));
}



void Shader::setVec3(const std::string& name, const glm::vec3& value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform3fv(location, 1, &value[0]));
}



void Shader::SetFloat(const std::string& name, float value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform1f(location, value));
}

//void Shader::SetUniformStruct(const std::string& name, void* myStruct)
//{
//	const int location = GetUniformLocation(name);
//	GLCall(glUniform4fv(location, 1, &value[0]));
//}

void Shader::Destroy()
{
	GLCall(glDeleteProgram(m_RendererID));

}

