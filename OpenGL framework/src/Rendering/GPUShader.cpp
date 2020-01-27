#include "precomp.h"

GPUShader::GPUShader(const std::string& filepath)
	: m_FilePath(filepath), name("not yet loaded"), m_RendererID(0),
	  m_UniformLocationCache()
{
	const ShaderProgramSource sps = parseShader(filepath);
	m_RendererID = CreateShader(sps.VertexSource, sps.FragmentSource);

	const  std::string nm = std::filesystem::path(filepath).stem().string();
	name = nm;
}

GPUShader::GPUShader()
{
}

void GPUShader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void GPUShader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void GPUShader::SetUniform1i(const std::string& name, int value)
{
	GLint l = GetUniformLocation(name);
	GLCall(glUniform1i(l, value));
}
void GPUShader::SetInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void GPUShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(int loc = GetUniformLocation(name));
	GLCall(glUniform4f(loc, v0, v1, v2, v3));
}

int GPUShader::GetUniformLocation(const std::string&  name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));

	if (location == -1)
		std::cout << "location is not set of uniform " << name << std::endl;

	m_UniformLocationCache[name] = location;

	return location;
}

unsigned int GPUShader::CompileShader(const unsigned int type, const std::string& source)
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
unsigned int GPUShader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	const unsigned int program = glCreateProgram();
	const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void GPUShader::SetUniformMat4f(const char* name, const glm::mat4& mat)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]));
}


void GPUShader::SetVec4f(const std::string &name, const glm::vec4& value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform4fv(location, 1, &value[0]));
}



void GPUShader::setVec3(const std::string &name, const glm::vec3 &value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform3fv(location, 1, &value[0]));
}
void GPUShader::SetFloat(const std::string &name, float value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform1f(location, value));
}

void GPUShader::Destroy()
{
	GLCall(glDeleteProgram(m_RendererID));

}

ShaderProgramSource GPUShader::parseShader(const std::string& path) const
{
	std::ifstream stream(path);
	std::string line;
	std::stringstream shaderFiles[2];

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else shaderFiles[(int)type] << line << "\n";

	}

	return { shaderFiles[0].str(), shaderFiles[1].str() };
}
