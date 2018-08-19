#include "Shader.h"
#include <string>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Renderer.h"
#include <functional>

//Shader::Shader (){}
Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath),
	m_RendererID(0)
{
	ShaderProgramSource sps = parseShader(filepath);
	m_RendererID = CreateShader(sps.VertexSource, sps.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
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
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(int loc = GetUniformLocation(name));
	GLCall(glUniform4f(loc, v0, v1, v2, v3));
}

int Shader::GetUniformLocation(const std::string&  name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));

	if (location == -1)
		std::cout << "location is not set of uniform " << name << std::endl;

	m_UniformLocationCache[name] = location;

	return location;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void Shader::SetUniformMat4f(const char* name, const glm::mat4& mat)
{
	int location = GetUniformLocation(name);
	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]));
}

//Shader & Shader::operator=(Shader * shader)
//{
//	return *shader;
//}

 
ShaderProgramSource Shader::parseShader(const std::string& path) const
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
